# makes a visual studio project file for VS18 for config files

import makeVcxprojConfig

if __name__ == "__main__":
    makeVcxprojConfig.command_line("msvc18", "18.0", "v145")

