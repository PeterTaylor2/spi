# makes a visual studio project file for VS17 for config files

import makeVcxprojConfig

if __name__ == "__main__":
    makeVcxprojConfig.command_line("msvc17", "17.0", "v143")

