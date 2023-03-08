# makes a visual studio project file for VS14 for config files

import makeVcxprojConfig

if __name__ == "__main__":
    makeVcxprojConfig.command_line("msvc15", "15.0", "v141")

