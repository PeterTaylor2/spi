#!/usr/bin/env python3
"""Build SPI and run every native test plus the Python extension smoke tests."""
import argparse
from contextlib import contextmanager
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
import json
import os
import shutil
from pathlib import Path
import subprocess
import sys
import sysconfig
import threading
import shutil
import datetime

ROOT = Path(__file__).resolve().parents[1]
RESULTS = ROOT / 'test-results'

shutil.rmtree(RESULTS, ignore_errors=True)

MAKEFILES_PYTHON = ROOT / 'makefiles/python'

sys.path.insert(0, str(MAKEFILES_PYTHON))
import vsVersions

def latest_directory(parent):
    return max((p for p in parent.iterdir() if p.is_dir() and p.name[0].isdigit()),
               key=lambda p: tuple(int(n) for n in p.name.split('.')))


def configuration(python_tag):
    version = f'{sys.version_info.major}.{sys.version_info.minor}'
    config = {'linux': 'linux64', 'darwin': 'macos64', 'win32': 'win64'}[sys.platform]
    tag = python_tag or (version.replace('.', '') if sys.platform == 'win32' else version)
    args = [f'SARTORIAL_CONFIG={config}', f'PY_VERSION={tag}',
            f'G_PYTHON={Path(sys.executable).as_posix()}']
    make = 'make'
    if sys.platform == 'win32':
        cygwin = ROOT / 'makefiles/cygwin64/bin'
        os.environ['PATH'] = str(cygwin) + os.pathsep + os.environ['PATH']
        make = str(cygwin / 'make.exe')
        args.append("COMPILER=msvc17")
        args.append("G_NO_MAKE_XLADDIN=1")
        args += vsVersions.get_contents()
        abi = 'Release'
    else:
        abi = 'Release-' + config
    return make, args, abi, tag

def create_site_mk(args):
    """site.mk file is not in source control but will be read by all the makefiles if it exists"""
    site_mk = ROOT / 'makefiles/config/site.mk'
    print("writing", site_mk)
    with site_mk.open('w', encoding='utf-8', newline='\n') as fp:
        fp.write("# Temporary site.mk file as part of ci build process\n\n")
        for arg in args:
            fp.write("%s\n" % arg)
        fp.write("\n")

def remove_site_mk():
    site_mk = ROOT / 'makefiles/config/site.mk'
    print("removing", site_mk)
    os.remove(site_mk)

def run(name, command, timeout=1200, cwd=ROOT, env=None, live=False):
    log = RESULTS / (name + '.log')
    print('RUN', name, flush=True)
    with log.open('w', encoding='utf-8') as output:
        output.write(' '.join(map(str, command)) + '\n')
        output.flush()
        proc = subprocess.Popen(command, cwd=cwd, env=env, stdout=subprocess.PIPE,
                                stderr=subprocess.STDOUT, text=True, errors='replace', bufsize=1)
        timed_out = threading.Event()

        def expire():
            timed_out.set()
            proc.kill()

        timer = threading.Timer(timeout, expire)
        timer.start()
        try:
            for line in proc.stdout:
                output.write(line)
                output.flush()
                if live:
                    sys.stdout.write(line)
                    sys.stdout.flush()
            proc.wait()
        finally:
            timer.cancel()
        if timed_out.is_set():
            output.write(f'\nTimed out after {timeout} seconds\n')
        success = proc.returncode == 0 and not timed_out.is_set()
    print(('PASS' if success else 'FAIL'), name, flush=True)
    if not success and not live:
        print('\n'.join(log.read_text(errors='replace').splitlines()[-60:]), flush=True)
    return success

@contextmanager
def fetch_proxy():
    # The legacy curl test uses a fixed public HTTP URL. Answer it locally in
    # CI so the test needs only its missing callback argument fixed upstream.
    class Handler(BaseHTTPRequestHandler):
        def do_GET(self):
            payload = b'SPI curl fixture\n'
            self.send_response(200)
            self.send_header('Content-Length', str(len(payload)))
            self.end_headers()
            self.wfile.write(payload)

        def log_message(self, *args):
            pass

    with ThreadingHTTPServer(('127.0.0.1', 0), Handler) as server:
        worker = threading.Thread(target=server.serve_forever, daemon=True)
        worker.start()
        try:
            yield f'http://127.0.0.1:{server.server_port}'
        finally:
            server.shutdown()
            worker.join()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--jobs', type=int, default=2)
    parser.add_argument('--live', action='store_true', help='Show main build steps to the screen')
    parser.add_argument('--skip-build', action='store_true', help='Use existing runtime libraries')
    parser.add_argument('--python-tag', help='Override the makefile Python version, e.g. 3-abi')
    parser.add_argument('--make-arg', action='append', default=[], help='Additional make variable assignment')
    opts = parser.parse_args()
    RESULTS.mkdir(exist_ok=True)
    make, args, abi, tag = configuration(opts.python_tag)
    args.extend(opts.make_arg)

    create_site_mk(args)

    command = [make, f'-j{opts.jobs}', '-s']

    start_time = datetime.datetime.now()

    if not opts.skip_build:
        if not run('build-all', [*command], timeout=3600, live=opts.live):
            return 1
    failures = []
    for directory in ('test/lib', 'test/config', 'test/dll'):
        if not run(directory.replace('/', '-') + '-build', [*command, '-C', directory]):
            return 1

    test_env = os.environ.copy()
    with fetch_proxy() as proxy:
        # the point of this is that any URL reads will be handled internally
        for key in list(test_env):
            if key.lower() in ('http_proxy', 'https_proxy', 'all_proxy', 'no_proxy'):
                del test_env[key]
        test_env['http_proxy'] = proxy
        test_env['no_proxy'] = ''

        for source in sorted((ROOT / 'test').glob('test*.cpp')):
            if not run(source.stem + '-build',
                   [make, '-j1', *args, '-C', 'test', 'run', 'NAME=' + source.stem], 120, env=test_env):
                return 1

    if not run('python-smoke', [sys.executable, str(ROOT / 'ci/python_smoke.py'),
                                '--root', str(ROOT), '--abi', abi, '--python-tag', tag], 60):
        failures.append('python-smoke')

    end_time = datetime.datetime.now()

    summary = {'failed': failures, 'native_test_programs': len(list((ROOT / 'test').glob('test*.cpp'))),
               'started': start_time.strftime('%Y-%m-%d %H:%M:%S'),
               'finished': end_time.strftime('%Y-%m-%d %H:%M:%S'),
               'python': sys.version, 'platform': sys.platform, 'abi': abi, 'python_tag': tag}
    (RESULTS / 'summary.json').write_text(json.dumps(summary, indent=2) + '\n')
    print(json.dumps(summary, indent=2))

    remove_site_mk()

    return int(bool(failures))


if __name__ == '__main__':
    raise SystemExit(main())
