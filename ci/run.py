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

ROOT = Path(__file__).resolve().parents[1]
RESULTS = ROOT / 'test-results'


def latest_directory(parent):
    return max((p for p in parent.iterdir() if p.is_dir() and p.name[0].isdigit()),
               key=lambda p: tuple(int(n) for n in p.name.split('.')))


def configuration(python_tag):
    version = f'{sys.version_info.major}.{sys.version_info.minor}'
    config = {'linux': 'linux64', 'darwin': 'macos64', 'win32': 'win64'}[sys.platform]
    tag = python_tag or (version.replace('.', '') if sys.platform == 'win32' else version)
    args = [f'SARTORIAL_CONFIG={config}', f'PY_VERSION={tag}',
            f'G_PYTHON={Path(sys.executable).as_posix()}',
            f'G_PYTHON_INCLUDES=-I{Path(sysconfig.get_path("include")).as_posix()}']
    make = 'make'
    if sys.platform == 'win32':
        cygwin = ROOT / 'makefiles/cygwin64/bin'
        os.environ['PATH'] = str(cygwin) + os.pathsep + os.environ['PATH']
        make = str(cygwin / 'make.exe')
        args.append('SHELL=' + (cygwin / 'sh.exe').as_posix() + ' -e')
        vswhere = Path(os.environ['ProgramFiles(x86)']) / 'Microsoft Visual Studio/Installer/vswhere.exe'
        installs = json.loads(subprocess.check_output([
            str(vswhere), '-latest', '-products', '*', '-version', '[17.0,18.0)',
            '-requires', 'Microsoft.VisualStudio.Component.VC.Tools.x86.x64', '-format', 'json']))
        vs = Path(installs[0]['installationPath'])
        tools = latest_directory(vs / 'VC/Tools/MSVC')
        kits = Path(os.environ['ProgramFiles(x86)']) / 'Windows Kits'
        sdk = latest_directory(kits / '10/Include')
        args += ['COMPILER=msvc17', f'G_VS17_PACKAGE_TYPE={vs.name}',
                 f'G_VS17_TOOLS_VERSION={tools.name}', f'G_VS17_KITS_VERSION={sdk.name}',
                 f'G_PYTHON_LIBS={Path(sys.base_prefix).as_posix()}/libs/python{tag}.lib']
        abi = 'Release'
    else:
        args.append('SHELL=/bin/sh -e')
        abi = 'Release-' + config
    return make, args, abi, tag


def run(name, command, timeout=1200, cwd=ROOT, env=None):
    log = RESULTS / (name + '.log')
    print('RUN', name, flush=True)
    with log.open('w', encoding='utf-8') as output:
        output.write(' '.join(map(str, command)) + '\n')
        output.flush()
        try:
            result = subprocess.run(command, cwd=cwd, stdout=output, stderr=subprocess.STDOUT,
                                    timeout=timeout, env=env)
            success = result.returncode == 0
        except subprocess.TimeoutExpired:
            output.write(f'\nTimed out after {timeout} seconds\n')
            success = False
    print(('PASS' if success else 'FAIL'), name, flush=True)
    if not success:
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
    parser.add_argument('--skip-build', action='store_true', help='Use existing runtime libraries')
    parser.add_argument('--python-tag', help='Override the makefile Python version, e.g. 3-abi')
    parser.add_argument('--make-arg', action='append', default=[], help='Additional make variable assignment')
    opts = parser.parse_args()
    RESULTS.mkdir(exist_ok=True)
    make, args, abi, tag = configuration(opts.python_tag)
    args.extend(opts.make_arg)
    command = [make, f'-j{opts.jobs}', *args]
    if not opts.skip_build:
        if not run('gendep-build', [*command, 'gendep']):
            return 1
        directories = subprocess.check_output(
            [make, '-s', '--no-print-directory', *args, '-C', 'code-generators',
             '-f', 'Makefile', '-f', (ROOT / 'ci/query.mk').as_posix(),
             'CI_VARIABLE=BUILD_DIRS', 'ci-value'], cwd=ROOT, text=True).split()
        for directory in directories:
            # Office COM automation needs Excel installed; all native Excel
            # bindings are still built by the runtime target on Windows.
            if directory == '../makeXLAddin':
                continue
            if not run('generator-' + directory.replace('../', '').replace('/', '-'),
                       [*command, '-C', 'code-generators/' + directory]):
                return 1
        # All generators are built; use the original installation recipe.
        if not run('generators-install', [*command, '-C', 'code-generators', 'all', 'BUILD_DIRS=']):
            return 1
        if not run('runtime-build', [*command, 'runtime']):
            return 1
    failures = []
    if not run('build-system-smoke', [*command, '-C', 'makefiles/test', 'run']):
        failures.append('build-system-smoke')
    for directory in ('test/lib', 'test/config', 'test/dll'):
        if not run(directory.replace('/', '-') + '-build', [*command, '-C', directory]):
            return 1
    # Preserve the original tests' relative paths in an isolated test directory.
    # testReadFile reads arbitrary binary data; it does not parse the PDF format.
    work = RESULTS / 'work'
    test_dir = work / 'test'
    test_dir.mkdir(parents=True, exist_ok=True)
    (work / 'doc').mkdir(exist_ok=True)
    (work / 'doc/spi-user-guide.pdf').write_bytes(bytes(range(256)) * 4)
    shutil.copyfile(ROOT / 'test/testStream.cpp', test_dir / 'testStream.cpp')
    build_dir = subprocess.check_output(
        [make, '-s', '--no-print-directory', *args, '-C', 'test',
         'show-var', 'VAR=G_BUILD_DIR'], cwd=ROOT, text=True).strip()
    binaries = ROOT / 'test' / build_dir
    test_env = os.environ.copy()
    if sys.platform == 'win32':
        test_env['PATH'] = str(binaries) + os.pathsep + test_env['PATH']
    elif sys.platform == 'linux':
        test_env['LD_LIBRARY_PATH'] = str(binaries) + os.pathsep + test_env.get('LD_LIBRARY_PATH', '')
    with fetch_proxy() as proxy:
        for key in list(test_env):
            if key.lower() in ('http_proxy', 'https_proxy', 'all_proxy', 'no_proxy'):
                del test_env[key]
        test_env['http_proxy'] = proxy
        test_env['no_proxy'] = ''
        for source in sorted((ROOT / 'test').glob('test*.cpp')):
            # Copy dependent DLLs before make checks the executable prerequisites.
            built = run(source.stem + '-build',
                        [make, '-j1', *args, '-C', 'test', 'target', 'NAME=' + source.stem], 120)
            executable = binaries / (source.stem + ('.exe' if sys.platform == 'win32' else ''))
            if not built or not run(source.stem, [str(executable)], 120, cwd=test_dir, env=test_env):
                failures.append(source.stem)
    if not run('python-smoke', [sys.executable, str(ROOT / 'ci/python_smoke.py'),
                                '--root', str(ROOT), '--abi', abi, '--python-tag', tag], 60):
        failures.append('python-smoke')
    summary = {'failed': failures, 'native_test_programs': len(list((ROOT / 'test').glob('test*.cpp'))),
               'python': sys.version, 'platform': sys.platform, 'abi': abi, 'python_tag': tag}
    (RESULTS / 'summary.json').write_text(json.dumps(summary, indent=2) + '\n')
    print(json.dumps(summary, indent=2))
    return int(bool(failures))


if __name__ == '__main__':
    raise SystemExit(main())
