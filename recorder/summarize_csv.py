# this code was written initially by ChatGPT-4 based on the request for a script that summarized the contents
# of CSV files written by the UDP JSON server

"""
Summarize CSV logs inside a directory.

Usage:
    python summarize_csv.py --dir ./logs/csv --format text --top 10

Features:
 - Walks a directory (optionally recursive) and reads CSV files.
 - Computes: total rows, files processed, rows per file, unique users, top N users, sum/avg of `count` column if present.
 - Outputs summary as plain text or JSON.
"""
import argparse
import csv
import json
import os
from collections import Counter, defaultdict
from typing import Dict, Any


def find_csv_files(root, recursive):
    for dirpath, dirnames, filenames in os.walk(root):
        for fn in filenames:
            if fn.lower().endswith(".csv"):
                yield os.path.join(dirpath, fn)
        if not recursive:
            break


def read_csv_rows(path):
    with open(path, "r", encoding="utf-8", errors="replace", newline="") as f:
        try:
            reader = csv.DictReader(f)
        except Exception:
            return
        for row in reader:
            yield row


def summarize(dirpath, keyName, recursive=False):
    dirpath = os.path.normpath(dirpath)
    files = list(find_csv_files(dirpath, recursive))
    total_rows = 0
    rows_per_file = {}
    count_sum = 0.0

    key_count = {}

    for path in files:
        path = os.path.normpath(path)
        cpath = os.path.commonpath([dirpath,path])
        if cpath != dirpath:
            raise Exception("Mismatch between %s and %s" % (dirpath, path))
        file_rows = 0
        for row in read_csv_rows(path):
            file_rows += 1
            total_rows += 1
            key = row.get(keyName, None)
            count = row.get("count", None)

            try:
                count = int(count)
            except: count = 0

            if key is not None and key not in key_count:
                key_count[key] = 0

            key_count[key] += count
            count_sum += count

        rows_per_file[path[len(cpath)+1:]] = file_rows

    summary = {
        "files_found": len(files),
        "total_rows": total_rows,
        "rows_per_file": rows_per_file,
        keyName + "_count": key_count,
        "count_sum": count_sum}

    return summary

def format_text(summary, keyName):
    key_count = summary[keyName + "_count"]
    lines = []
    lines.append("Files found: %d" % summary['files_found'])
    lines.append("Total rows: %d" % summary['total_rows'])
    lines.append("Unique %ss: %d" % (keyName, len(key_count)))
    lines.append("")
    lines.append("Rows per file:")
    rows_per_file = summary['rows_per_file']
    for path in sorted(rows_per_file):
        lines.append("    %-60s %d" % (path, rows_per_file[path]))
    lines.append("")

    lines.append("Summary of %s count:" % keyName)
    for key in sorted(key_count):
        lines.append("    %-60s %d" % (key, key_count[key]))

    lines.append("")
    lines.append("Count sum: %d" % summary['count_sum'])

    return "\n".join(lines)


def main():
    parser = argparse.ArgumentParser(description="Summarize CSV data in a directory")
    parser.add_argument("--dir", required=True, help="Directory containing CSV files")
    parser.add_argument("--key", choices="user,context,function", default="function")
    parser.add_argument("--recursive", action="store_true", help="Recurse into subdirectories")
    parser.add_argument("--format", choices=("text", "json"), default="text", help="Output format")
    parser.add_argument("--out", help="Write output to file (default: stdout)")
    args = parser.parse_args()

    summary = summarize(args.dir, args.key, recursive=args.recursive)

    if args.format == "json":
        out = json.dumps(summary, indent=2)
    else:
        out = format_text(summary, args.key)

    if args.out:
        with open(args.out, "w", encoding="utf-8") as f:
            f.write(out)
    else:
        print(out)


if __name__ == "__main__":
    main()
