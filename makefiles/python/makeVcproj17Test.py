# makes a visual studio project file for VS17 for config files

import makeVcxprojTest

from makeVcproj17 import abiFunc

if __name__ == "__main__":
    makeVcxprojTest.command_line("msvc17", "17.0", "v143", abiFunc)

