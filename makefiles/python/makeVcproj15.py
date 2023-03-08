# makes a visual studio project file for VC15

import makeVcxproj

# this needs to match the definitions in the makefiles/config directory
def abiFunc(bits, debug):
    return "%s-%s-vc15" % (debug, "x64" if bits == 64 else "x86")

if __name__ == "__main__":
    makeVcxproj.command_line("msvc15", "15.0", "v141", abiFunc)

