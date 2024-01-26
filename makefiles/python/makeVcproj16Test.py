# makes a visual studio project file for VS16 for config files

import makeVcxprojTest

if __name__ == "__main__":
    makeVcxprojTest.command_line("msvc16", "16.0", "v142")

