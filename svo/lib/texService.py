import datetime
import os
import posixpath

import sys

if sys.version_info[0] < 2:
    import texModule
    import texIndex

    from generatedOutput import GeneratedOutput
    from texUtils import *
else:
    from . import texModule
    from . import texIndex

    from .generatedOutput import GeneratedOutput
    from .texUtils import *

def writeTexService(dirname, service, dnImports, extraServices):
    fns = set()

    for extraService in extraServices:
        if service.ns != extraService.ns:
            raise Exception("Namespace mismatch for extra service: %s and %s" % (
                service.ns, extraService.ns))

    _writeTexServiceDescription(dirname, service, fns)
    _writeTexServiceDateStamp(dirname, service, fns)
    _writeTexServiceBuiltinTypes(dirname, service, fns)

    simpleTypes = []
    enums = []
    classes = []
    functions = []
    classMethods = []
    typesUsed = set()

    serviceIndex = texIndex.ServiceIndex()

    for module in service.modules:
        texModule.addModuleToServiceIndex(module, serviceIndex)
    for module in service.modules:
        texModule.writeTexModuleConstructs(dirname, service, module, serviceIndex,
                                           simpleTypes, enums, classes, functions,
                                           classMethods, typesUsed, fns)

    for extraService in extraServices:
        for module in extraService.modules:
            texModule.addModuleToServiceIndex(module, serviceIndex)
        for module in extraService.modules:
            texModule.writeTexModuleConstructs(dirname, service, module, serviceIndex,
                                               simpleTypes, enums, classes, functions,
                                               classMethods, typesUsed, fns)


    _writeTexServiceTypes(dirname, service, simpleTypes, enums, classes, 
        typesUsed, dnImports, fns)
    _writeTexServiceFunctions(dirname, service, functions, classMethods, fns)

    _tidyup(service, dirname, fns)

def _writeTexServiceTypes(dirname, service, simpleTypes, enums, classes, typesUsed,
                          dnImports, fns):

    out = GeneratedOutput(texFileName(dirname, "types.tex"))

    undefinedTypes = typesUsed.copy()

    out.write("\n")
    out.write("Each item of data in the library is of a particular data type.\n")
    out.write("Arbitrarily complex data types can be built up of simpler "
              "types, with ultimately everything being broken down into the "
              "fundamental built-in types.\n")
    out.write("\n")
    out.write("When describing the inputs to a function or data fields in a "
              "class we will use the term \\texttt{attribute} indicating the "
              "data type and name of the input or class data field.\n")
    out.write("Attributes in addition can be an array or a matrix.\n")
    out.write("We will indicate a simple array by the notation \\texttt{[]} "
              "after the data type.\n")
    out.write("We will indicate a matrix by the notation \\texttt{[,]} after "
              "the data type.\n")
    out.write("Note that a matrix is a rectangular array where each row in the "
              "array has the same number of elements.\n")
    out.write("\n")
    out.write("In this section we describe all the possible data types "
              "available in the library, starting from the built-in types and "
              "building up to the classes available in the library.\n")
    out.write("\n")

    out.write("\\input{tex/builtinTypes.tex}\n")
    if len(simpleTypes) > 0:
        _defineTypes("simpleType", simpleTypes, undefinedTypes)
        _writeTexServiceSimpleTypes(dirname, service, simpleTypes, fns)
        out.write("\\newpage\n")
        out.write("\\input{tex/simpleTypes.tex}\n")

    if len(enums) > 0:
        _defineTypes("enum", enums, undefinedTypes)
        _writeTexServiceEnums(dirname, service, enums, fns)
        out.write("\\newpage\n")
        out.write("\\input{tex/enums.tex}\n")

    if len(classes) > 0:
        _defineTypes("class", classes, undefinedTypes)
        _writeTexServiceClasses(dirname, service, classes, fns)
        out.write("\\newpage\n")
        out.write("\\input{tex/classes.tex}\n")

    if len(undefinedTypes) > 0:
        _writeTexImportedTypes(dirname, service, undefinedTypes, dnImports, fns)
        out.write("\\newpage\n")
        out.write("\\input{tex/importedTypes.tex}\n")

    fns.add("types.tex")
    return out.close()

def _defineTypes(typeOfType, fns, undefinedTypes):
    expectedStart = typeOfType + "_"
    expectedEnd   = ".tex"

    lenS = len(expectedStart)
    lenE = len(expectedEnd)

    for fn in fns:
        assert(fn.startswith(expectedStart))
        assert(fn.endswith(expectedEnd))
        tn = fn[lenS : -lenE]
        if tn in undefinedTypes: undefinedTypes.remove(tn)

def _writeTexServiceSimpleTypes(dirname, service, filenames, fns):

    out = GeneratedOutput(texFileName(dirname, "simpleTypes.tex"))

    out.write("\\section{Simple types}\n\n")
    out.write("Simple types are types which are represented by one of the "
              "standard types in the public interface (bool, int, double, "
              "string, Date, char, unsigned int).\n\n")
    out.write("However internally to the library the type is converted to some "
              "other type understood by the library itself.\n\n")
    out.write("This means that the user sees instances of the standard type "
              "which are then converted (and possibly validated) to the type "
              "understood by the library.\n\n")

    writeTexInputList(out, filenames, True)
    fns.add("simpleTypes.tex")
    return out.close()

def _writeTexServiceEnums(dirname, service, fileNames, fns):

    out = GeneratedOutput(texFileName(dirname, "enums.tex"))
    out.write("\\section{Enumerated types}\n\n")
    out.write("Enumerated types are types which can take one of a finite number "
              "of possible values. At the C++ level, these are represented by "
              "an enum inside a class. The possible values hence are given the "
              "namespace of the class name which corresponds to the name defined "
              "in the documentation.\n\n")
    out.write("For the Excel interface, the values that can be used are strings. "
              "These will then be converted into the C++ enumerated type, and "
              "then used within the low-level library code.\n\n")
    out.write("In the documentation, we will give the name of the enumerated "
              "type, followed by a general description of how the type is used, "
              "and then followed by a list of the possible values (known as the "
              "enumerands).\n\n")

    writeTexInputList(out, fileNames, True)
    fns.add("enums.tex")
    return out.close()

def _writeTexServiceClasses(dirname, service, fileNames, fns):

    out = GeneratedOutput(texFileName(dirname, "classes.tex"))

    out.write("\\section{Classes}\n\n")
    out.write("Classes contain data represented by attributes and can have "
              "function represented by so-called class methods.\n\n")
    out.write("Attributes will have a name and data type, and contain data of "
              "the defined data type.\n"
              "Attributes can be scalars or arrays.\n"
              "The data type can be any previously defined data type, e.g "
              "another class, or a simple type, or an enumerated type, or one "
              "of the built-in primitive data types.\n\n")
    out.write("Classes can in general be serialised.\n"
              "This can be to a string format, or written to file in various "
              "formats, or transmitted via the network when using a "
              "client-server model.\n\n")
    out.write("In general, all of the attributes will be needed when we "
              "serialise the class, but not all of the attributes are needed "
              "for general information about the class.\n"
              "Hence some attributes are marked as private (only used in "
              "serialisation), and others as public (can be accessed directly "
              "from outside the class).\n"
              "At present we do not support changing the value of an attribute "
              "of class, but this may change in the future.\n\n")
    out.write("Classes can also have class methods.\n"
              "In the C++ interface these are C++ member functions.\n"
              "In the Excel interface class methods appear as functions where "
              "the name of the function starts with the class name followed by "
              "the method name.\n"
              "The first parameter of the function is the class instance (or "
              "object).\n"
              "In other interfaces to come later we will generally use the "
              "natural representation for each language supported.\n"
              "For example, Python and .NET both support classes, so we will "
              "have equivalent classes in these languages corresponding to the "
              "C++ class.\n\n")
    out.write("In the C++ interface we will use a form of shared pointer for "
              "representing the class.\n"
              "The normal constructors will be protected and you need to use "
              "the static \\texttt{Make} method for each class in order "
              "to create the shared pointer instance for the class.\n"
              "By protecting the constructor in this manner, we make it "
              "impossible for client code to use raw pointers, and this should "
              "make it easier to avoid memory leaks in client C++ code.\n\n")

    writeTexInputList(out, fileNames, True, True)
    fns.add("classes.tex")
    return out.close()

def _writeTexImportedTypes(dirname, service, undefinedTypes, dnImports, fns):

    # what we do is loop through the undefinedTypes looking for potential
    # imports - if we find an import file we copy it locally - if we don't
    # find an import file we will simply list at the end of the document
    # the imports that we could not find - at least in this manner we will
    # have a valid cross-reference

    fileNames = {}
    
    prefixList = ["enum_", "simpleType_", "class_"]
    numDnImports = len(dnImports)
    stillUndefined = []

    for undefinedType in undefinedTypes:
        found = False
        for dnImport in dnImports:
            for prefix in prefixList:
                baseName = "%s%s.tex" % (prefix, undefinedType)
                fileName = posixpath.join(dnImport, baseName)
                if posixpath.isfile(fileName):
                    found = True
                    # do we really need to copy the file?
                    # can't we just leave it where it is?
                    fileNames[undefinedType] = fileName
                    break
            if found: break
        if not found: stillUndefined.append(undefinedType)

    out = GeneratedOutput(texFileName(dirname, "importedTypes.tex"))

    out.write("\\section{Imported types}\n\n")
    out.write("Some types are defined by other services.\n"
              "For a full description (including constructors) please refer to "
              "the documentation for those services.\n\n")
    out.write("In this document we will provide more limited information about "
              "each of these (so-called) imported types.\n\n")

    for typeName in sorted(fileNames):
        # no need to sort
        # no need to amend the file name
        # hence we don't call writeTextInputList
        out.write("\\input{%s}\n" % fileNames[typeName])
    out.write("\n")

    for undefined in stillUndefined:
        out.write("Undefined type: %s \\label{type_%s}\n\n" % (undefined,undefined))

    fns.add("importedTypes.tex")
    return out.close()

def _writeTexServiceFunctions(dirname, service, functions, classMethods, fns):

    if len(classMethods) > 0:
        # we need three files - functionsAndClassMethods.tex, functions.tex, classMethods.tex
        # when we wrote the functions, each function had its own subsection
        fcmout = GeneratedOutput(texFileName(dirname, "functionsAndClassMethods.tex"))
        fcmout.write("In this library there are two types of functions.\n"
                     "We have free standing functions not attached to a "
                     "particular class, and we have class methods which are "
                     "attached to a class and for which you will need an "
                     "instance of the class (also known as an Object) in order "
                     "to make the call.\n"
                     "In addition there are also so-called \\texttt{static} "
                     "methods of a class which behave more like free standing "
                     "functions except that the name of the function also "
                     "includes the class name.\n"
                     "\n"
                     "Note that Excel does natively support the concept of class "
                     "methods and only supports functions.\n"
                     "Hence from within Excel when you have a class method you "
                     "are actually calling a function with one extra parameter "
                     "at the beginning of the argument list.\n"
                     "The function name in this case includes the class name as "
                     "well as the method name.\n"
                     "\n"
                     "For Python and .NET which do support classes, then a class "
                     "method is invoked by using the instance followed by "
                     "\\texttt{`.'} followed by the method name.\n"
                     "Note that it is a consequence of the Python language that "
                     "you can actually also use the Excel-style syntax for "
                     "calling a class method.\n"
                     "For \\texttt{static} methods in Python and .NET then you "
                     "need to use the class name instead of the instance when "
                     "calling the \\texttt{static} method.\n"
                     "\n"
                     "Another difference between Excel and the other supported "
                     "interfaces is that for Excel we represent Objects via "
                     "string handles.\n"
                     "For functions which create objects, there will be one "
                     "extra parameter to the function (or method).\n"
                     "This parameter is called \\texttt{baseName} and is needed "
                     "to provide the first part of the string which forms the "
                     "Object handle.\n"
                     "\\newpage\n"
                     "\\input{tex/functions.tex}\n"
                     "\\newpage\n"
                     "\\input{tex/classMethods.tex}\n"
                     "\n")
        fns.add("functionsAndClassMethods.tex")
        fcmout.close()

        cmout = GeneratedOutput(texFileName(dirname, "classMethods.tex"))
        cmout.write("\\section{Class methods}\n\n")
        cmout.write("These are the class methods (including \\texttt{static} "
                    "methods in the library).\n"
                    "\n"
                    "Note that when using the class method within Excel you will "
                    "need to use the overall namespace for the library as a "
                    "prefix to the full class method name.\n"
                    "\n"
                    "From within Python and .NET then you only need the last "
                    "part of the full class method name (using \\texttt{`.'} as "
                    "the separator and replace the rest of the full class method "
                    "name with an instance of that class (which is the first "
                    "part of the full class method name).\n"
                    "\n"
                    "There is an exception for \\texttt{static} methods.\n"
                    "In this case the calling convention is the same for Python "
                    "and .NET as for Excel - i.e. you need the full class "
                    "name prefixed by the overall namespace for the library.\n"
                    "\n")

        writeTexInputList(cmout, classMethods, True)
        fns.add("classMethods.tex")
        cmout.close()

    out = GeneratedOutput(texFileName(dirname, "functions.tex"))
    if len(classMethods) > 0:
        out.write("\\section{Functions}\n\n")
        out.write("These are free standing functions which are not attached to "
                  "a particular class.\n"
                  "\n")
    else:
        # since we have no class methods we have to replicate some of the
        # introductory text from above - however there are enough differences
        # to provide different content
        out.write("This section describes the functions available in the "
                  "library.\n"
                  "\n"
                  "Note that when using Excel in conjunction with Objects "
                  "(an Object is an instance of a class), we represent Objects "
                  "via string handles.\n"
                  "For functions which create objects, there will be one "
                  "extra parameter to the function (or method).\n"
                  "This parameter is called \\texttt{baseName} and is needed "
                  "to provide the first part of the string which forms the "
                  "Object handle.\n"
                  "\n")

    out.write("Note that when using the function you will need to use the "
              "overall namespace for the library as a prefix to the function "
              "name.\n"
              "\n")

    writeTexInputList(out, functions, True)
    fns.add("functions.tex")
    return out.close()


def _writeTexServiceDescription(dirname, service, fns):

    out = GeneratedOutput(texFileName(dirname, "service.tex"))
    writeTexDescription(out, service.description)
    fns.add("service.tex")
    return out.close()

_MONTHS = "January,February,March,April,May,June,July,August,September,October,November,December".split(",")

def _writeTexServiceDateStamp(dirname, service, fns):
    global _MONTHS

    out = GeneratedOutput(texFileName(dirname, "datestamp.tex"))
    today = datetime.date.today()

    out.write("\\date{%d %s %d}\n" % (today.day, _MONTHS[today.month-1], today.year))
    out.write("\n")
    out.write("\\newcommand{\\%sVersionNumber}{%s}\n" % (service.ns.replace("_",""), service.version))

    fns.add("datestamp.tex")
    return out.close()

def _writeTexServiceBuiltinTypes(dirname, service, fns):

    out = GeneratedOutput(texFileName(dirname, "builtinTypes.tex"))

    out.write("\\section{Built-in types}\n\n")
    out.write("Built-in types are data types that are provided as standard for all services.\n")
    out.write("These types are the building blocks of all data representations.\n")

    writeTexBeginTable(out, 2, 100, "Typename", 360, "Description")

    writeTexTableRow(out, 2, "bool", "True or false.")
    writeTexTableRow(out, 2, "int", "Positive or negative integer (32-bits).")
    writeTexTableRow(out, 2, "double", "Double precision real number (64-bits).")
    writeTexTableRow(out, 2, "string", "Standard string type.")
    writeTexTableRow(out, 2, "Date", "!Date type. This will be translated "
        "into the appropriate type for each context. "
        "For Excel, this is the number of days since 1900. "
        "For Python, this is the \\texttt{datetime.date} class. "
        "For .NET this is the \\texttt{System.DateTime} class.")
    writeTexTableRow(out, 2, "char", "Single character.")
    writeTexTableRow(out, 2, "unsigned int", "Integer that will always be positive")

    writeTexEndTable(out, 2)

    fns.add("builtinTypes.tex")
    return out.close()

def _tidyup(svc, dn, fns):
    for subdn in ["tex", "tex_imports"]:
        dnTex = posixpath.join(dn, subdn)
        for fn in os.listdir(dnTex):
            if fn.endswith(".tex"):
                ffn = posixpath.join(dnTex, fn)
                if fn not in fns:
                    print("Removing %s" % ffn)
                    os.remove(ffn)


