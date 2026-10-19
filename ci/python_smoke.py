"""Exercise generated extensions, conversions and Python class delegation."""
import argparse
import datetime
import os
from pathlib import Path
import sys


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

    spi = import_from(root / 'replay' / args.abi / ('py' + args.python_tag), 'spi_replay')
    values = {
        'integer': (42, 'INT'), 'float': (3.125, 'DOUBLE'),
        'boolean': (True, 'BOOL'), 'string': ('hello world', 'STRING'),
        'date': (datetime.date(2024, 2, 29), 'DATE'),
        'datetime': (datetime.datetime(2024, 2, 29, 12, 34, 56), 'DATETIME'),
    }
    sample = {name: value for name, (value, _) in values.items()}
    assert set(spi.MapFieldNames(sample)) == set(values)
    for name, (expected, expected_type) in values.items():
        actual, value_type = spi.MapGetValue(sample, name)
        assert actual == expected, (name, actual, expected)
        assert type(actual) is type(expected), (name, type(actual), type(expected))
        assert value_type == expected_type, (name, value_type, expected_type)
        print('PASS roundtrip', name, flush=True)
    original = spi.TestGenerator()
    restored = spi.Object.from_string(original.to_string())
    assert isinstance(restored, spi.TestGenerator), type(restored)
    assert restored.to_dict(True) == {}
    print('PASS Python subclass serialization', flush=True)
    import_from(root / 'svo/spdoc/bin' / (args.abi + '-py' + args.python_tag), 'spdoc')
    print('ALL PYTHON CHECKS PASSED:', sys.version, flush=True)


if __name__ == '__main__':
    main()
