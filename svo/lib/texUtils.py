import sys
import os

if sys.version_info[0] < 3:
    from generatedOutput import GeneratedOutput
else:
    from .generatedOutput import GeneratedOutput

def texFileName(dirname, filename, forImport=False):
    tex = "tex_imports" if forImport else "tex"
    return os.path.join(dirname, tex, filename)

def writeTexInputList(out, inputs, sortInputs, addNewPage=False):
    """
    Writes a list of \input files to the output directory.
    Returns true if the file was updated.
    """
    inputsUsed = sorted(inputs) if sortInputs else inputs

    out.write("\n")
    for item in inputsUsed:
        if addNewPage: out.write("\\newpage\n")
        out.write("\\input{tex/%s}\n" % item)

    if len(inputsUsed) == 0:
        out.write("None\n")

def texEscape(plainText, keepQuotes=False):
    """
    Adds escapes to plain text to enable it to be shown in tex.
    Any genuine escapes would end up being doubled up - so only use this
    for plain text without prior escapes.
   
    Sometimes the latter problem is difficult to avoid - in that case put
    '!' at the beginning of the text and this function strips of the '!'
    and assumes that the rest of the text is in acceptable format already.
    """
    if plainText.startswith("!"): return plainText[1:]

    out = []
    inQuotes = False

    for c in plainText:
        if c == '\\':
            out.append("$\\backslash$")
        elif c in "{}$%&_#":
            out.append("\\%s" % c)
        elif c in "><":
            out.append("$%s$" % c)
        elif c == '"':
            if keepQuotes:
                out.append('"')
            elif inQuotes:
                out.append("''")
                inQuotes = False
            else:
                out.append('``')
                inQuotes = True
                pass #end-else
            pass #end-elif
        else: out.append(c)

    return "".join(out)

def writeTexDescription (out, description):
    """
    Write a block of description in Tex format to file.
   
    If the first line of the description starts with '!' then it is assumed
    that the description has already been prepared in Tex format.
   
    Otherwise the text is considered as plain text and all tex characters
    will be escaped out before printing.
    """
    if len(description) == 0: return

    for line in description:
        if line.startswith("!"):
            out.write(line[1:].rstrip())
            out.write("\n")
        else:
            out.write(texEscape(line))
            out.write("\n")

def writeTexBeginTable(out, nbColumns, *widthAndHeadings):
    """
    Writes the beginning of a table in Tex format.
   
    Variable entries are pairs of width,heading terminated by 0 for width.
    """
    widths = []
    headings = []

    if nbColumns <= 0:
        raise RuntimeError("No columns defined")

    if len(widthAndHeadings) != 2*nbColumns:
        raise RuntimeError("Wrong number of width and headings provided")

    for i in range(nbColumns):
        widths.append(widthAndHeadings[2*i])
        headings.append(widthAndHeadings[2*i+1])

    out.write("\n\n")
    out.write("\\small\n")
    out.write("\n")
    out.write("\\begin{longtable}{")

    for width in widths: out.write("|p{%dpt}" % width)

    out.write("|l}\n")
    out.write("\n")
    out.write("\\cline{1-%d} " % nbColumns)

    for heading in headings: out.write("\\textbf{%s} & " % heading)

    out.write("\n")
    out.write("\\endfirsthead\n")
    out.write("\n")
    out.write("\\cline{1-%d} " % nbColumns)

    for heading in headings: out.write("\\textbf{%s} & " % heading) 

    out.write("\\\\\n")
    out.write("\\cline{1-%d} " % nbColumns)
    out.write("\\endhead\n")
    out.write("\n")
    out.write("\\cline{1-%d} \\multicolumn{%d}{r}{{Continued on next page}}\n" % (
        nbColumns, nbColumns))
    out.write("\\endfoot\n")
    out.write("\n")
    out.write("\\cline{1-%d}\n" % nbColumns)
    out.write("\\endlastfoot\n")

def writeTexTableRow(out, nbColumns, *columnEntries):
    """
    Write one row of a table.
   
    Variable inputs are column entries as c-string terminated by 0.
    """
    if nbColumns <= 0:
        raise RuntimeError("No columns defined")

    if len(columnEntries) != nbColumns:
        raise RuntimeError("Wrong number of column entries provided")

    out.write("\n")
    out.write("\\cline{1-%d} " % nbColumns)

    for i in range(nbColumns):
        if i > 0: out.write(" & ")
        out.write(texEscape(columnEntries[i]))
    out.write(" \\\\ \n")

def writeTexEndTable(out, nbColumns):
    """
    Writes the end of a table in Tex format.
    """
    out.write("\\cline{1-%d}\n\n" % nbColumns)
    out.write("\\end{longtable}\n")
    out.write("\\normalsize\n")

