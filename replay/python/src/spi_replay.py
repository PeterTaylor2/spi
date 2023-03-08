"""
No description
"""

import os
cwd = os.getcwd()
try:
    fn = os.path.abspath(__file__)
    dn = os.path.dirname(fn)
    os.chdir(dn)

    if os.path.basename(fn).startswith("__init__.py"):
        from .py_spi_replay import *
    else:
        from py_spi_replay import *
finally: os.chdir(cwd)

del(os)
del(cwd)
del(fn)
del(dn)

import sys
class TestGenerator(ReplayCodeGenerator):
    def __init__(self, ofn=None):
        self.ofn = ofn
        self.fp = None
    def __del__(self):
        if self.fp:
            sys.stdout.write("Closing %s\n" % self.ofn)
            self.fp.close()
            self.fp = None
    def _write(self, txt):
        if self.ofn and self.fp is None:
            sys.stdout.write("Opening %s\n" % self.ofn)
            self.fp = open(self.ofn, "w")
        fp = sys.stdout if self.fp is None else self.fp
        fp.write("%s\n" % txt)
        if not txt: fp.flush()
    def GenerateFunction(self, action):
        self._write("FUNCTION: %s" % MapClassName(action.inputs))
        self._write(action.to_string())
        self._write("")
    def GenerateObject(self, action):
        self._write("OBJECT: %s" % MapClassName(action.inputs))
        self._write(action.to_string())
        self._write("")
    def to_dict(self, publicOnly):
        dict = {}
        if self.ofn: dict["ofn"] = self.ofn
        return dict
    @staticmethod
    def from_dict(dict):
        ofn = dict.get("ofn", None)
        return TestGenerator(ofn = ofn)

# Object is automatically provided by py_spi_replay
Object.register_class(TestGenerator)


