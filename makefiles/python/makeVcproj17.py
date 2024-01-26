# makes a visual studio project file for VC17

import makeVcxproj

if __name__ == "__main__":
    makeVcxproj.command_line("msvc17", "17.0", "v143")

