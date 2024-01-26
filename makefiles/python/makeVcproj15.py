# makes a visual studio project file for VC15

import makeVcxproj

if __name__ == "__main__":
    makeVcxproj.command_line("msvc15", "15.0", "v141")

