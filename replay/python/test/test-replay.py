"""
As part of the spi_replay package we provide an example python class TestGenerator
sub-classed from ReplayCodeGenerator. Part of the purpose of this project is to
show class delegation in action.

So, what you should do is run "make install" in the python directory to copy the spi_replay
python extension to your usersitepackages directory, and then you can run this test.
"""

import spi_replay

import site
import os

print("usersitepackages: %s" % site.getusersitepackages())
print("spi_replay dn:    %s" % (os.path.dirname(spi_replay.__file__)))

def main(ifn, ofn):
    replayLog = spi_replay.ReplayLog.Read(ifn)
    generator = spi_replay.TestGenerator(ofn)
    replayLog.generateCode(generator)

if __name__ == "__main__":
    import sys
    import getopt

    opts, args = getopt.getopt(sys.argv[1:], "w")

    for opt in opts:
        if opt[0] == "-w":
            input("enter to continue:")

    main(*args)

