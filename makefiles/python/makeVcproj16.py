# makes a visual studio project file for VC16

import makeVcxproj

# this needs to match the definitions in the makefiles/config directory
def abiFunc(bits, debug):
    return "%s-%s-vc16" % (debug, "x64" if bits == 64 else "x86")

if __name__ == "__main__":
    makeVcxproj.command_line("msvc16", "16.0", "v142", abiFunc)

