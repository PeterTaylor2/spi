"""
Copyright (C) 2012-2022 Sartorial Programming Ltd.

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
"""

copyright = "Copyright (C) 2012-2022 Sartorial Programming Ltd."

# This script creates a user-guide using LaTeX from an SPI service object file.
#
# The reason that we publish these functions is so that the client has the
# option of tailoring the user-guides to their own requirements.
#
# Hence feel completely free to amend or entirely revamp the associated
# main script and imported scripts.
#
# Note that the SPI service object file is in a format understood by the spdoc
# Python extension which is provided as part of the development kit.

import sys
import os

import spdoc
import lib.texService
from lib.texOptions import Options, setOptions

if sys.version_info[0] < 3:
    input = raw_input
    
def print_usage(progname):
    sys.stderr.write("Usage: %s [-w] [-t] (docfile dirname)\n" % progname)

def run(infilename, outfilename, dirname, extras, dnImports, summaryfilename, options):
    if options.verbose: print(copyright)
    serviceDoc = spdoc.Service.from_file(infilename)
    extraServiceDocs = [spdoc.Service.from_file(extra) for extra in extras]
    setOptions(options)
    lib.texService.writeTexService(dirname, serviceDoc, dnImports, extraServiceDocs)
    serviceDoc.to_file(outfilename)
    if summaryfilename is not None:
        services = [serviceDoc]
        services.extend(extraServiceDocs)
        summary = spdoc.Service.combineSummaries(services, True)
        print("writing %s" % summaryfilename)       
        sfp = open(summaryfilename, "w")
        sfp.write("\n".join(summary))
        sfp.write("\n")
        sfp.close()
    return 0

def main():
    import getopt

    opts,args = getopt.getopt(sys.argv[1:], "S:wtvi:", ["writeIncludes", "backup"])
    
    waitAtStart = False
    options = Options()
    summaryfilename = None

    progname = os.path.basename(sys.argv[0])
    dnImports = []

    for opt in opts:
        if opt[0] == "-w": waitAtStart = True
        elif opt[0] == "-t": timings = True
        elif opt[0] == "-v": options.verbose = True
        elif opt[0] == "-S": summaryfilename = opt[1]
        elif opt[0] == "-i": dnImports.append(opt[1])
        elif opt[0] == "--writeIncludes": options.writeIncludes = True
        elif opt[0] == "--backup": options.writeBackup = True
        else:
            print_usage(progname)
            return -1
        pass #end-for

    if len(args) < 3: 
        print_usage(progname)
        return -1

    infilename = args[0]
    outfilename = args[1]
    dirname = args[2]
    if len(args) > 3:
        extras = args[3:]
    else:
        extras = []

    print("%s: Service object to LaTeX Document Generator" % progname)

    if waitAtStart: input("Enter to continue:")

    return run(infilename, outfilename, dirname, extras, dnImports, summaryfilename, options)

if __name__ == "__main__": main()
