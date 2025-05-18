"""
Defines the simplified types arising from the first phase of the
code generation process.

This is designed to provide enough information for all phase two
code generation.

You no longer need to know anything about the inner library or how
data is translated into the inner library.
"""

import os

cwd = os.getcwd()
try:
    os.chdir(os.path.dirname(os.path.abspath(__file__)))
    import sys
    sys.path.insert(0, os.getcwd()) # needed by python3
    try:
        from py_spdoc import *
    finally: sys.path.pop(0)
finally: os.chdir(cwd)

