import spi_replay

class TestInputsGenerator(spi_replay.ReplayCodeGenerator):
    def __init__(self, ofn, func):
        self.ofn = ofn
        self.func = func
        print("Opening %s" % self.ofn)
        self.ofp = open(self.ofn, "w")
    def __del__(self):
        if self.ofp:
            print("Closing %s" % self.ofn)
            self.ofp.close()
            self.ofp = None
    def GenerateFunction(self, action):
        if spi_replay.MapClassName(action.inputs) == self.func:
            print("processing function")
            fieldNames = spi_replay.MapFieldNames(action.inputs)
            for name in fieldNames:
                value = getattr(action.inputs, name)
                self.ofp.write("%-22s" % (name + ": "))
                if isinstance(value, list):
                    self.ofp.write("[")
                    self.ofp.write(", ".join([str(v) for v in value]))
                    self.ofp.write("]")
                else:
                    self.ofp.write("%s" % value)
                self.ofp.write("\n")
            self.ofp.write("\n")
    def GenerateObject(self, action): pass

def main(ifn, ofn, func):
    replayLog = spi_replay.ReplayLog.Read(ifn)
    generator = TestInputsGenerator(ofn, func)
    replayLog.generateCode(generator)

if __name__ == "__main__":
    import sys
    import getopt

    opts, args = getopt.getopt(sys.argv[1:], "")
    main(*args)

