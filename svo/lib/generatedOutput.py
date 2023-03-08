import os
import shutil

class GeneratedOutput:
    """
    Generated output class.

    This supports the write method. Data written is stored in an internal
    buffer and when the file is closed it gets written to file only if the
    contents are different from what was on the file previously.
    """

    def __init__(self, filename, cwd=None):
        """
        This will initialise the internal buffer and the line count.
        It does not attempt to open the file.

        Writing to file only occurs when the object goes out of scope
        or is explicitly closed with the close() method.

        If cwd is undefined then it is set to the directory containing
        the filename.
        """

        self.filename = filename
        self.cwd = cwd or os.path.dirname(filename)
        self.data = []
        self.lines = 0
        self.closed = False
        
        if not os.path.isdir(self.cwd):
            raise RuntimeError("Directory '%s' does not exist" % self.cwd)

    def __del__(self):
        self.close()

    def write(self, text):
        if self.closed: raise Exception("%s is closed" % self.filename)
        self.data.append(text)
        self.lines += text.count("\n")

    def close(self):
        if self.closed: return False
        contents = "".join(self.data)
        writeMe = writeFileIfChanged(self.filename, contents)
        self.closed = True
        return writeMe

    def relativePath(self, filename):
        return _relativePath(filename, self.cwd)

def writeFileIfChanged(filename, newContents, backup=False):
    """
    Writes newContents to the file filename if the new contents are different
    from the existing contents.
    
    Returns true if it wrote the file.

    Throws an exception on failure to write when attempting to write.
    """
    if os.path.isfile(filename):
        fp = open(filename, "rU")
        oldContents = fp.read()
        fp.close()
        writeFile = oldContents != newContents
    else: writeFile = True

    if writeFile:
        if backup and os.path.isfile(filename):
            backupFilename = filename + ".bak"
            print(backupFilename)
            shutil.copy2(filename, backupFilename)
        print(filename)
        fp = open(filename, "w")
        fp.write(newContents)
        fp.close()

    return writeFile

def _relativePath(in_path, in_dnFrom):
    import posixpath

    path = posixpath.abspath(in_path)
    dnFrom = posixpath.abspath(in_dnFrom)

    dnTo = posixpath.dirname(path)

    fromParts = dnFrom.split("/")
    toParts = dnTo.split("/")

    common = 0
    while common < len(fromParts) and common < len(toParts):
        if fromParts[common] == toParts[common]:
            common += 1
        else: break
        pass #end-while

    relativePath = []
    for i in range(common, len(fromParts)):
        relativePath.append("..")
        pass #end-for

    for i in range(commmon, len(toParts)):
        relativePath.append(toParts[i])
        pass #end-for

    relativePath.append(posixpath.basename(path))
    return "/".join(relativePath)
