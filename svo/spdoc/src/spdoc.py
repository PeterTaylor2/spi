"""
Defines the simplified types arising from the first phase of the
code generation process.

This is designed to provide enough information for all phase two
code generation.

You no longer need to know anything about the inner library or how
data is translated into the inner library.
"""

import os
import sys

cwd = os.getcwd()
try:
    fn = os.path.basename(__file__)
    dn = os.path.dirname(os.path.abspath(__file__))
    try:
       os.chdir(dn)
    except:
        # in case we are in library.zip (for example) go up to a directory
        while dn and not os.path.isdir(dn):
            dn = os.path.dirname(dn)
        if os.path.isdir(dn): os.chdir(dn)
    if fn.startswith("__init__"):
        from .py_spdoc import *
    else:
        from py_spdoc import *
finally:
    os.chdir(cwd)
    del(cwd)
    del(dn)
    del(fn)
    del(sys)
    del(os)

