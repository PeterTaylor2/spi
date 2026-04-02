# this code was written initially by ChatGPT-4 based on the request for a script that provided
# a web server to show the CSV files written by the UDP JSON server

"""
Simple web server to browse and preview logs directory.

Usage:
    python log_server.py --dir ./logs --host 127.0.0.1 --port 8080

Features:
 - Serves a simple HTML index of files and subdirectories under the logs directory.
 - Preview CSV files as an HTML table (first N rows) and plain files as text (head).
 - JSON API endpoints: /api/files and /api/file?path=<relpath>
 - Protects against path traversal by resolving and validating requested paths.
 - Optionally shows a summary using `summarize_csv.summarize` if available.
"""

from http.server import HTTPServer, BaseHTTPRequestHandler
import argparse
import html
import io
import json
import mimetypes
import os
import posixpath
import urllib.parse
from datetime import datetime
import csv

try:
    import summarize_csv
    HAVE_SUMMARIZE = True
except Exception:
    HAVE_SUMMARIZE = False

MAX_PREVIEW_ROWS = 500
MAX_TEXT_BYTES = 100_000


def safe_path(base_dir: str, rel_path: str) -> str:
    # Prevent path traversal; rel_path is URL-quoted possibly
    unquoted = urllib.parse.unquote(rel_path)
    # Strip leading slash
    if unquoted.startswith("/"):
        unquoted = unquoted[1:]
    joined = os.path.normpath(os.path.join(base_dir, unquoted))
    base_dir = os.path.normpath(base_dir)
    if os.path.commonpath([base_dir, joined]) != base_dir:
        raise ValueError("Invalid path")
    return joined


class LogHTTPRequestHandler(BaseHTTPRequestHandler):
    server_version = "LogServer/0.1"

    def do_GET(self):
        parsed = urllib.parse.urlparse(self.path)
        path = parsed.path
        qs = urllib.parse.parse_qs(parsed.query)

        try:
            if path.startswith("/api/"):
                self.handle_api(path, qs)
            elif path == "/":
                self.handle_index()
            elif path.startswith("/file/"):
                rel = path[len("/file/"):]
                self.handle_file(rel, qs)
            else:
                # map to file preview: allow direct /some/path.csv
                rel = path.lstrip("/")
                self.handle_file(rel, qs)
        except ValueError:
            self.send_error(400, "Bad request")
        except FileNotFoundError:
            self.send_error(404, "Not found")
        except Exception as e:
            self.send_error(500, "Server error")

    def handle_api(self, path, qs):
        if path == "/api/files":
            self.api_list_files(qs)
        elif path == "/api/file":
            self.api_file(qs)
        else:
            self.send_error(404, "API not found")

    def api_list_files(self, qs):
        # return a JSON list of files and directories under root
        root = self.server.logs_dir
        result = []
        for dirpath, dirnames, filenames in os.walk(root):
            rel_dir = os.path.relpath(dirpath, root)
            if rel_dir == ".":
                rel_dir = ""
            for d in sorted(dirnames):
                p = os.path.join(rel_dir, d) if rel_dir else d
                result.append({"path": p, "type": "dir"})
            for f in sorted(filenames):
                p = os.path.join(rel_dir, f) if rel_dir else f
                abspath = os.path.join(dirpath, f)
                st = os.stat(abspath)
                result.append({"path": p, "type": "file", "size": st.st_size, "mtime": st.st_mtime})
            break  # only top-level by default
        self.send_json(result)

    def api_file(self, qs):
        if "path" not in qs:
            self.send_error(400, "Missing path")
            return
        rel = qs.get("path")[0]
        abspath = safe_path(self.server.logs_dir, rel)
        if not os.path.exists(abspath):
            raise FileNotFoundError()
        if os.path.isdir(abspath):
            # list directory
            items = []
            for name in sorted(os.listdir(abspath)):
                p = os.path.join(abspath, name)
                items.append({"name": name, "is_dir": os.path.isdir(p), "size": os.path.getsize(p)})
            self.send_json({"path": rel, "items": items})
            return
        # file: return small preview
        if abspath.lower().endswith(".csv"):
            rows = []
            with open(abspath, "r", encoding="utf-8", errors="replace", newline="") as f:
                reader = csv.reader(f)
                for i, row in enumerate(reader):
                    rows.append(row)
                    if i >= MAX_PREVIEW_ROWS:
                        break
            self.send_json({"path": rel, "preview_rows": rows})
        else:
            with open(abspath, "rb") as f:
                data = f.read(MAX_TEXT_BYTES)
            try:
                text = data.decode("utf-8")
            except Exception:
                text = data.decode("utf-8", errors="replace")
            self.send_json({"path": rel, "preview_text": text})

    def handle_index(self):
        root = self.server.logs_dir
        entries = []
        for name in sorted(os.listdir(root)):
            p = os.path.join(root, name)
            st = os.stat(p)
            entries.append({
                "name": name,
                "is_dir": os.path.isdir(p),
                "size": st.st_size,
                "mtime": st.st_mtime,
            })
        # build simple HTML
        parts = ["<html><head><meta charset=\"utf-8\"><title>Logs</title></head><body>"]
        parts.append(f"<h1>Logs: {html.escape(root)}</h1>")
        if HAVE_SUMMARIZE:
            try:
                summary = summarize_csv.summarize(root, "function")
                parts.append("<h2>Summary (csv/)</h2>")
                parts.append("<pre>" + html.escape(json.dumps(summary, sort_keys=True, indent=2)) + "</pre>")
            except Exception:
                pass
        parts.append("<ul>")
        for e in entries:
            n = html.escape(e["name"])
            if e["is_dir"]:
                parts.append(f"<li>[DIR] <a href=\"/file/{urllib.parse.quote(e['name'])}\">{n}</a> - {e['size']} bytes</li>")
            else:
                parts.append(f"<li>[FILE] <a href=\"/file/{urllib.parse.quote(e['name'])}\">{n}</a> - {e['size']} bytes</li>")
        parts.append("</ul>")
        parts.append("<p>API: <a href=\"/api/files\">/api/files</a></p>")
        parts.append("</body></html>")
        body = "\n".join(parts).encode("utf-8")
        self.send_response(200)
        self.send_header("Content-Type", "text/html; charset=utf-8")
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def handle_file(self, rel, qs):
        abspath = safe_path(self.server.logs_dir, rel)
        if os.path.isdir(abspath):
            # list directory
            items = []
            for name in sorted(os.listdir(abspath)):
                p = os.path.join(abspath, name)
                items.append((name, os.path.isdir(p), os.path.getsize(p)))
            parts = ["<html><head><meta charset=\"utf-8\"><title>Dir</title></head><body>"]
            parts.append(f"<h1>Directory: {html.escape(rel or '/')}</h1>")
            parts.append("<ul>")
            for name, is_dir, size in items:
                display = html.escape(name)
                q = urllib.parse.quote(os.path.join(rel, name))
                if is_dir:
                    parts.append(f"<li>[DIR] <a href=\"/file/{q}\">{display}</a></li>")
                else:
                    parts.append(f"<li>[FILE] <a href=\"/file/{q}\">{display}</a> - {size} bytes</li>")
            parts.append("</ul>")
            parts.append("</body></html>")
            body = "\n".join(parts).encode("utf-8")
            self.send_response(200)
            self.send_header("Content-Type", "text/html; charset=utf-8")
            self.send_header("Content-Length", str(len(body)))
            self.end_headers()
            self.wfile.write(body)
            return

        if not os.path.exists(abspath):
            raise FileNotFoundError()

        # If CSV, render table preview
        if abspath.lower().endswith(".csv"):
            with open(abspath, "r", encoding="utf-8", errors="replace", newline="") as f:
                reader = csv.reader(f)
                rows = []
                for i, row in enumerate(reader):
                    rows.append(row)
                    if i >= MAX_PREVIEW_ROWS:
                        break
            parts = ["<html><head><meta charset=\"utf-8\"><title>CSV Preview</title></head><body>"]
            parts.append(f"<h1>CSV: {html.escape(rel)}</h1>")
            parts.append("<table border=1 cellspacing=0 cellpadding=4>")
            for r in rows:
                parts.append("<tr>" + "".join(f"<td>{html.escape(str(c))}</td>" for c in r) + "</tr>")
            parts.append("</table>")
            if len(rows) >= MAX_PREVIEW_ROWS:
                parts.append(f"<p>Showing first {MAX_PREVIEW_ROWS+1} rows</p>")
            parts.append("</body></html>")
            body = "\n".join(parts).encode("utf-8")
            self.send_response(200)
            self.send_header("Content-Type", "text/html; charset=utf-8")
            self.send_header("Content-Length", str(len(body)))
            self.end_headers()
            self.wfile.write(body)
            return

        # otherwise show as text (head)
        with open(abspath, "rb") as f:
            data = f.read(MAX_TEXT_BYTES)
        try:
            text = data.decode("utf-8")
        except Exception:
            text = data.decode("utf-8", errors="replace")
        body = ("<html><head><meta charset=\"utf-8\"><title>File</title></head><body>"
                f"<h1>{html.escape(rel)}</h1>"
                f"<pre>{html.escape(text)}</pre>"
                "</body></html>").encode("utf-8")
        self.send_response(200)
        self.send_header("Content-Type", "text/html; charset=utf-8")
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def send_json(self, obj):
        b = json.dumps(obj, default=str, indent=2).encode("utf-8")
        self.send_response(200)
        self.send_header("Content-Type", "application/json; charset=utf-8")
        self.send_header("Content-Length", str(len(b)))
        self.end_headers()
        self.wfile.write(b)

    def log_message(self, format, *args):
        # reduce noise: print to stdout in a concise form
        msg = "%s - - [%s] %s\n" % (self.client_address[0], datetime.now().isoformat(sep=" ", timespec="seconds"), format % args)
        print(msg, end="")


class LogHTTPServer(HTTPServer):
    def __init__(self, server_address, RequestHandlerClass, logs_dir):
        super().__init__(server_address, RequestHandlerClass)
        self.logs_dir = os.path.abspath(logs_dir)


def run(host: str, port: int, logs_dir: str):
    srv = LogHTTPServer((host, port), LogHTTPRequestHandler, logs_dir)
    print(f"Serving {logs_dir} on http://{host}:{port}/")
    try:
        srv.serve_forever()
    except KeyboardInterrupt:
        print("Stopping server")
        srv.server_close()


def main():
    parser = argparse.ArgumentParser(description="Serve logs directory over HTTP for quick browsing")
    parser.add_argument("--dir", required=True, help="Logs directory to serve")
    parser.add_argument("--host", default="127.0.0.1", help="Host to bind (default 127.0.0.1)")
    parser.add_argument("--port", type=int, default=8080, help="Port to bind (default 8080)")
    args = parser.parse_args()
    if not os.path.isdir(args.dir):
        print("Directory not found:", args.dir)
        return
    run(args.host, args.port, args.dir)


if __name__ == "__main__":
    main()
