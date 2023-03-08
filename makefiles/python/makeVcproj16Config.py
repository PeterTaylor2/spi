# makes a visual studio project file for VS16 for config files

import makeVcxprojConfig

if __name__ == "__main__":
    makeVcxprojConfig.command_line("msvc16", "16.0", "v142")

