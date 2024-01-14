"""
Defines command line options used by low level functions
"""

class Options:
    def __init__(self):
        self.verbose = False
        self.writeIncludes = False
        self.writeBackup = False

_options = None

def setOptions(options):
    global _options
    _options = options


def getOptions():
    global _options
    return _options


