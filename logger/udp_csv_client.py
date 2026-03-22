# this is a simple UDP client that reads CSV lines from a file or stdin (or generates them) and sends them as UTF-8 UDP packets to a specified server.
# It supports optional rate control and line count limits, and has debug/quiet modes for output verbosity.

"""
Simple UDP CSV client that sends CSV log lines to a UDP server.

Usage examples:
 - Send lines from a file once:
     python udp_csv_client.py --host 127.0.0.1 --port 5140 --file logs.csv
 - Stream stdin lines at 10 lines/sec:
     cat logs.csv | python udp_csv_client.py --host 127.0.0.1 --port 5140 --file - --interval 0.1
 - Generate synthetic CSV lines indefinitely:
     python udp_csv_client.py --host 127.0.0.1 --port 5140 --generate --interval 1.0

Behavior:
 - Reads CSV lines from a file or stdin (use --file - for stdin) or generates simple rows.
 - Sends each line as a UTF-8 UDP packet (no extra framing).
 - Optional rate control with --interval (seconds between sends) or --rate (lines/second).
 - Can limit number of lines with --count.
"""

import argparse
import socket
import sys
import time
from datetime import datetime
from typing import Iterator, Optional

DEFAULT_HOST = "127.0.0.1"
DEFAULT_PORT = 5140
MAX_PACKET_SIZE = 64 * 1024

def read_lines_from_file(path: str) -> Iterator[str]:
    if path == "-":
        for line in sys.stdin:
            yield line.rstrip("\n")
    else:
        with open(path, "r", encoding="utf-8", errors="replace") as f:
            for line in f:
                yield line.rstrip("\n")

def generate_csv_lines() -> Iterator[str]:
    # Simple synthetic CSV: timestamp,level,message
    levels = ["INFO", "WARN", "ERROR"]
    i = 0
    while True:
        ts = datetime.utcnow().isoformat() + "Z"
        level = levels[i % len(levels)]
        msg = f"synthetic message {i}"
        yield f'{ts},{level},"{msg}"'
        i += 1

def send_lines(host: str, port: int, lines: Iterator[str], interval: Optional[float],
               count: Optional[int], debug: bool, quiet: bool):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.setblocking(True)
    sent = 0
    start = time.time()

    try:
        for line in lines:
            if count is not None and sent >= count:
                break
            data = line.encode("utf-8", errors="replace")
            if len(data) > MAX_PACKET_SIZE:
                if debug:
                    print("Skipping line larger than max packet size:", len(data), file=sys.stderr)
                continue
            sock.sendto(data, (host, port))
            sent += 1
            if not quiet:
                print(f"sent {sent} bytes={len(data)} -> {host}:{port}")
            if interval is not None:
                time.sleep(interval)
    except KeyboardInterrupt:
        if not quiet:
            print("Interrupted by user, stopping")
    finally:
        sock.close()
        elapsed = time.time() - start
        if not quiet:
            print(f"Finished. Sent {sent} packet{'s' if sent != 1 else ''} in {elapsed:.2f}s")

def parse_args():
    p = argparse.ArgumentParser(description="UDP CSV client")
    p.add_argument("--host", default=DEFAULT_HOST, help="Server host (default: 127.0.0.1)")
    p.add_argument("--port", type=int, default=DEFAULT_PORT, help="Server UDP port (default: 5140)")
    p.add_argument("--file", help="Path to CSV file to send, or '-' for stdin. If omitted use --generate.", default=None)
    p.add_argument("--generate", action="store_true", help="Generate synthetic CSV rows if no file provided")
    grp = p.add_mutually_exclusive_group()
    grp.add_argument("--interval", type=float, help="Seconds between sends (e.g. 0.1). Mutually exclusive with --rate.")
    grp.add_argument("--rate", type=float, help="Lines per second to send (mutually exclusive with --interval).")
    p.add_argument("--count", type=int, help="Maximum number of lines to send (default: all)", default=None)
    p.add_argument("--debug", action="store_true", help="Enable debug output")
    p.add_argument("--quiet", action="store_true", help="Minimal output")
    return p.parse_args()

def main():
    args = parse_args()

    if args.rate is not None:
        if args.rate <= 0:
            raise SystemExit("rate must be > 0")
        interval = 1.0 / args.rate
    else:
        interval = args.interval

    if args.file is None and not args.generate:
        print("No --file and not --generate; use --file <path|-> or --generate", file=sys.stderr)
        raise SystemExit(2)

    if args.file is not None:
        lines = read_lines_from_file(args.file)
    else:
        lines = generate_csv_lines()

    send_lines(args.host, args.port, lines, interval, args.count, args.debug, args.quiet)

if __name__ == "__main__":
    main()