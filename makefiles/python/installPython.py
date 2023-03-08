import os
import sys
import filecmp
import shutil

# this code is windows specific
import getpass
import subprocess

def _set_permission(ofn, username):
    args = ["icacls"]
    args.append(os.path.normpath(ofn))
    args.append("/grant")
    args.append("%s:F" % username)
    if os.path.isdir(ofn): args.append("/T")

    FNULL = open(os.devnull, "w")
    result = subprocess.call(args, stdout=FNULL)
    FNULL.close()

    if result != 0:
        raise Exception("Failed to set permissions for %s" % ofn)

def installFile(src, dst):
    if not os.path.isfile(src):
        raise Exception("Source file %s is not a file" % os.path.normpath(src))
    if not os.path.isfile(dst):
        install = True
    else: install = not filecmp.cmp(src, dst)

    if install:
        print("installing %s" % dst)
        shutil.copy2(src, dst)
        if sys.platform == "win32":
            _set_permission(dst, getpass.getuser())

    return install

def getSitePackages():
    # assume we are using the same version of Python that we are targetting
    try:
        import site
        return site.getusersitepackages()
    except: pass

    pythonHome = os.path.dirname(os.path.abspath(sys.executable))
    if sys.platform == "win32":
        return os.path.join(pythonHome, "Lib", "site-packages")

    # on Linux we want to find /usr/local/lib/python2.7/dist-packages
    if sys.platform == "linux2":
        pythonV = "python%s.%s" % (
            sys.version_info.major, sys.version_info.minor)
        return os.path.join("/usr", "local", "lib", pythonV, "dist-packages")

    # on Mac we want to find /Library/Python/2.7/site-packages
    if sys.platform == "darwin": # mac
        pythonV = "%s.%s" % (sys.version_info.major, sys.version_info.minor)
        return os.path.join("/Library", "Python", pythonV, "site-packages")

    raise Exception("Unsupported platform %s" % sys.platform)

def main(name, source, fns, package=None, clean=True):

    bfns = [os.path.basename(fn) for fn in fns]
    sitePackages = getSitePackages()
    package = package or name
    target = os.path.join(sitePackages, package)
    if not os.path.isdir(target):
        print("creating %s" % target)
        os.makedirs(target)
    if not os.path.isdir(target):
        raise Exception("Could not create %s directory" % target)

    if package == name:
        for fn in os.listdir(target):
            ffn = os.path.join(target, fn)
            if os.path.isfile(ffn) and fn != "__init__.py" and fn not in bfns:
                if clean:
                    print("removing %s" % ffn)
                    os.remove(ffn)
                else:
                    print("not removing %s" % ffn)

    for fn in bfns + ["%s.%s" % (name,suffix) for suffix in ["py","pyc"]]:
        ffn = os.path.join(sitePackages, fn)
        if os.path.isfile(ffn):
            print("removing %s" % ffn)
            os.remove(ffn)

    if package == name:
        installFile(os.path.join(source, "%s.py" % name),
                    os.path.join(target, "__init__.py"))
    else:
        installFile(os.path.join(source, "%s.py" % name),
                    os.path.join(target, "%s.py" % name))
        packageInit = os.path.join(target, "__init__.py")
        if not os.path.isfile(packageInit):
            print("creating %s" % packageInit)
            fp = open(packageInit, "w")
            fp.write("\n")
            fp.close()

    for fn in fns:
        if fn.startswith("."):
            bfn = os.path.basename(fn)
            installFile(fn, os.path.join(target, bfn))
        else: installFile(os.path.join(source, fn), os.path.join(target, fn))

if __name__ == "__main__":
    import sys
    import getopt

    kwargs = {}
    opts,args = getopt.getopt(sys.argv[1:], "p:", ["noclean"])
    for opt in opts:
        if opt[0] == "-p":
            kwargs["package"] = opt[1]
        if opt[0] == "--noclean":
            kwargs["clean"] = False

    if len(args) < 3:
        print(" ".join(sys.argv))
        raise Exception("Expecting 3+ arguments: name source files")

    name      = args[0]
    source    = args[1]
    fns       = args[2:]

    try:
        main(name, source, fns, **kwargs)
    except Exception as e:
        sys.stderr.write("ERROR: %s\n" % str(e))
        raise SystemExit(-1)

