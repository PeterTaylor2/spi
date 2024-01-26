# makes a visual studio project file for VC16

import makeVcxproj

if __name__ == "__main__":
    makeVcxproj.command_line("msvc16", "16.0", "v142")

