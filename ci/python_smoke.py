"""Exercise generated extensions, conversions and Python class delegation."""
import argparse
import datetime
import os
from pathlib import Path
import sys
import filecmp


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--root', type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument('--abi', required=True)
    parser.add_argument('--python-tag', required=True)
    args = parser.parse_args()
    root = args.root.resolve()
    dll_directories = []

    def import_from(directory, module):
        os.chdir(directory)
        sys.path.insert(0, str(directory))
        if hasattr(os, 'add_dll_directory'):
            dll_directories.append(os.add_dll_directory(str(directory)))
        result = __import__(module)
        print('PASS import', module, flush=True)
        return result

    spdoc = import_from(root / 'svo/spdoc/bin' / args.abi / ('py' + args.python_tag), 'spdoc')
    test_svo = str(root / 'test/config/test.svo')

    svc = spdoc.Service.from_file(test_svo)
    print('PASS read service object', test_svo)

    print("Summary of service object:\n")
    print("\n".join(svc.Summary(True)))
    print("\n")

    print('PASS summarize service object', test_svo)

    spi_replay = import_from(root / 'replay' / args.abi / ('py' + args.python_tag), 'spi_replay')

    ifn = str(root / 'ci/a.log')
    ofn = str(root / 'test-results/a.out')
    baseline = str(root / 'ci/a.out')

    replayLog = spi_replay.ReplayLog.Read(ifn)
    generator = spi_replay.TestGenerator(ofn)
    replayLog.generateCode(generator)
    del generator
    
    assert filecmp.cmp(ofn, baseline, shallow=False)
    print('PASS spi_replay.generateCode')

    print('ALL PYTHON CHECKS PASSED:', sys.version, flush=True)


if __name__ == '__main__':
    main()
