# makes a visual studio project file for VS15 for config files

import makeVcxprojTest

if __name__ == "__main__":
    makeVcxprojTest.command_line("msvc15", "15.0", "v141")

