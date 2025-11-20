# makes a visual studio project file for VC18

import makeVcxproj

if __name__ == "__main__":
    makeVcxproj.command_line("msvc18", "18.0", "v145")

