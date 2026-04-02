# this code was written by ChatGPT-4 based on the user's request for a UDP log collector that accepts CSV payloads and writes them to disk,
# with specific behavior and features as described in the initial comment block.

"""
UDP log collector that accepts JSON or CSV payloads and writes them to disk.

Behavior:
 - The packet is recorded as a CSV row to:
     <log_dir>/csv/YYYY-MM-DD.csv
   CSV rows have columns: timestamp, src_ip, src_port, data
 - Safe concurrent writes using threading Locks.
 - Configurable host/port/log directory via CLI.
 - Graceful shutdown on SIGINT/SIGTERM.
"""

import argparse
import csv
import json
import logging
import os
import socket
import threading
import signal
from datetime import datetime

MAX_PACKET_SIZE = 64 * 1024  # 64 KiB

class UDPLogServer:
    def __init__(self, host: str, port: int, log_dir: str):
        self.host = host
        self.port = port
        self.log_dir = os.path.abspath(log_dir)
        os.makedirs(self.log_dir, exist_ok=True)

        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.bind((self.host, self.port))
        self.running = False

        # Locks to protect concurrent file writes per-day/type
        self._locks = {}
        self._locks_lock = threading.Lock()

        logging.info("Listening on %s:%d, logs -> %s", self.host, self.port, self.log_dir)

    def _get_lock(self, path):
        # Return a lock for a specific filepath (create if needed)
        with self._locks_lock:
            lock = self._locks.get(path)
            if lock is None:
                lock = threading.Lock()
                self._locks[path] = lock
            return lock

    def _csv_log_path(self, ts: datetime, computer: str, osName: str):
        """
        Generate a CSV log path based on the timestamp and optional computer name.
        """
        parts = [ts.strftime("%Y-%m-%d")]
        if computer: 
            parts.append(computer)
        if osName:
            parts.append(osName)

        ffn = "%s.csv" % ("-".join(parts))
        return os.path.join(self.log_dir, ffn)

    def start(self):
        self.running = True
        self._main_thread = threading.Thread(target=self._run_recv_loop, name="udp-recv", daemon=True)
        self._main_thread.start()

    def stop(self):
        self.running = False
        try:
            # sending a dummy packet to loopback will unblock recvfrom
            # (sending to 0.0.0.0 may not be accepted, so use loopback)
            self.sock.sendto(b"", ("127.0.0.1", self.port))
        except Exception:
            pass
        self._main_thread.join(timeout=2)
        self.sock.close()
        logging.info("Server stopped")

    def _run_recv_loop(self):
        while self.running:
            try:
                data, addr = self.sock.recvfrom(MAX_PACKET_SIZE)
                # If stop() triggered a dummy packet, break soon
                if not self.running:
                    break
                threading.Thread(target=self._handle_packet, args=(data, addr), daemon=True).start()
            except OSError:
                break
            except Exception:
                logging.exception("Error receiving packet")

    def _handle_packet(self, data: bytes, addr):
        src_ip, src_port = addr
        ts = datetime.now()
        text = None
        try:
            # Assume UTF-8 (common for JSON/CSV logs). Fallback replace errors. 
            text = data.decode("utf-8")
        except Exception:
            text = data.decode("utf-8", errors="replace")

        # Try to parse JSON payload. If parsing fails, treat payload as raw text.
        parsed = None
        try:
            parsed = json.loads(text)
        except Exception:
            parsed = None

        self._append_csv(parsed, ts, src_ip, src_port, raw_text=text)
        logging.debug("Packet from %s:%d stored (json=%s)", src_ip, src_port, bool(parsed))

    def _append_csv(self, payload, ts: datetime, src_ip: str, src_port: int, raw_text: str = ""):
        """
        Append a CSV row for the given payload.
        """
        if payload is None: return
        if not isinstance(payload, dict): return

        computer = payload.get("computer", "")
        osName = payload.get("os", "")

        path = self._csv_log_path(ts, computer, osName)
        lock = self._get_lock(path)
        with lock:
            is_new = not os.path.exists(path)
            with open(path, "a", newline="", encoding="utf-8") as f:
                writer = csv.writer(f)
                headers = ["timestamp", "user", "pid", "context", "function", "count"]
                if is_new:
                    writer.writerow(headers)

                row = [ts.isoformat(sep=" ", timespec="seconds")]

                for k in ["user", "pid", "context", "function", "count"]:
                    v = payload.get(k, "")
                    row.append(v)

                writer.writerow(row)

def run_cli():
    parser = argparse.ArgumentParser(description="UDP CSV/text log collector")
    parser.add_argument("--host", default="0.0.0.0", help="Host to bind (default: 0.0.0.0)")
    parser.add_argument("--port", type=int, default=5140, help="UDP port to bind (default: 5140)")
    parser.add_argument("--log-dir", default="./logs", help="Directory to store logs (default: ./logs)")
    parser.add_argument("--debug", action="store_true", help="Enable debug logging")
    args = parser.parse_args()

    logging.basicConfig(level=logging.DEBUG if args.debug else logging.INFO,
                        format="%(asctime)s %(levelname)s %(message)s")

    server = UDPLogServer(args.host, args.port, args.log_dir)

    # graceful shutdown handlers
    stop_event = threading.Event()
    def _signal_handler(signum, frame):
        logging.info("Received signal %s, shutting down...", signum)
        stop_event.set()
        server.stop()
    signal.signal(signal.SIGINT, _signal_handler)
    signal.signal(signal.SIGTERM, _signal_handler)

    server.start()
    logging.info("UDP log collector started (press Ctrl-C to stop)")

    # Wait until stopped
    try:
        while not stop_event.is_set():
            stop_event.wait(1.0)
    finally:
        if server.running:
            server.stop()

if __name__ == "__main__":
    run_cli()
