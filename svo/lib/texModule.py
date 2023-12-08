import sys

if sys.version_info[0] < 3:
    from texUtils import *
    from generatedOutput import GeneratedOutput
    from texOptions import getOptions
else:
    from .texUtils import *
    from .generatedOutput import GeneratedOutput
    from .texOptions import getOptions

def _moduleHeader(service, module):
    hfn = "%s_%s.hpp" % (service.ns, module.name)
    return hfn

def _moduleClassesHeader(service, module):
    hfn = "%s_%s_classes.hpp" % (service.ns, module.name)
    return hfn

def writeTexModuleConstructs(dirname,
                             service,
                             module,
                             serviceIndex,
                             allSimpleTypes,
                             allEnums,
                             allClasses,
                             allFunctions,
                             allClassMethods,
                             typesUsed,
                             fns):

    hasClassMethods = serviceIndex.NumClassMethods() > 0

    for construct in module.constructs:
        constructType = construct.getType()

        if constructType == "SIMPLE_TYPE":
            fn = _writeTexSimpleType(construct, dirname, service, module, False, fns)
            if len(fn): allSimpleTypes.append(fn)
            _writeTexSimpleType(construct, dirname, service, module, True, fns)
        elif constructType == "ENUM":
            allEnums.append(_writeTexEnum(construct, dirname, service, module, False, fns))
            _writeTexEnum(construct, dirname, service, module, True, fns)
        elif constructType == "CLASS":
            allClasses.append(_writeTexClass(construct, dirname, service, module,
                                                allClassMethods, serviceIndex, False,
                                                typesUsed, fns))
            _writeTexClass(construct, dirname, service, module, allClassMethods,
                           serviceIndex, True, typesUsed, fns)
        elif constructType == "FUNCTION":
            allFunctions.append(_writeTexFunction(construct, dirname, service,
                                                  module, hasClassMethods, typesUsed, fns))


def _writeTexSimpleType(construct, dirname, service, module, forImport, fns):

    if construct.noDoc: return ""

    constructName = _constructName(construct, module)
    systemTypeName = _translateTypeName(construct.typeName)

    filename = "simpleType_%s.tex" % constructName

    out = GeneratedOutput(texFileName(dirname, filename, forImport))

    out.write("\\subsection{%s} " % constructName)
    out.write("\\label{type_%s}\n" % constructName)
    out.write("\n")
    out.write("%s is represented by %s in the public interface.\n" % (
        constructName, systemTypeName))

    out.write("\n")
    if len(construct.description) == 0:
        out.write("No description.\n")
    else:
        writeTexDescription(out, construct.description)

    out.write("\n\n")
    fns.add(filename)
    return filename

def _writeTexClass(cls, dirname, service, module, allClassMethods, serviceIndex,
                   forImport, typesUsed, fns):

    className = _constructName(cls, module)
    filename = "class_%s.tex" % className

    out = GeneratedOutput(texFileName(dirname, filename, forImport))
    out.write("\\subsection{%s} \\label{type_%s}\n" % (
        texEscape(className), className))
    
    out.write("\n")
    if len(cls.description) == 0:
        out.write("No description.\n")
    else: writeTexDescription(out, cls.description)

    if getOptions().writeIncludes:
        out.write("\n\\verb|#include <%s>|\n" % _moduleClassesHeader(service, module))

    if len(cls.baseClassName):
        out.write("\n")
        out.write("\\textbf{Base Class:} %s (see page \\pageref{type_%s})\n" % (
            texEscape(cls.baseClassName), cls.baseClassName))

    derivedClasses = serviceIndex.DerivedClasses(className)

    if len(derivedClasses) > 0:
        out.write("\n")
        out.write("\\textbf{Derived Classes:}\n")
        out.write("\\begin{itemize}\n")

        for derivedClass in sorted(derivedClasses):
            out.write("  \\item %s (see page \\pageref{type_%s})\n" % (
                     texEscape(derivedClass), derivedClass))

        out.write("\\end{itemize}\n")

    out.write("\n")
    out.write("\\textbf{Attributes:}\n")
    out.write("\\nopagebreak\n")

    if len(cls.attributes) == 0:
        out.write("None\n\n")
    else:
        _writeAttributesTable(out, cls.attributes, forImport)

    if len(cls.properties) > 0:
        out.write("\n")
        out.write("\\textbf{Properties:}\n")
        out.write("\\nopagebreak\n")

        _writeAttributesTable(out, cls.properties, forImport)

        out.write("Note that properties can be accessed in the same way as "
                  "attributes, but are not part of the constructor or the "
                  "serialization of the class.\n")
        out.write("\n")

    if not forImport:

        out.write("\\textbf{Methods:}\n")
        out.write("\\nopagebreak\n")

        if len(cls.methods) == 0 and len(cls.coerceTo) == 0:
            out.write("None\n\n")
        else:
            writeTexBeginTable(out, 2, 150, "Prototype", 310, "Description")

            for method in cls.methods:
                argTypes = []
                for attr in method.function.inputs:
                    name = _translateTypeName(attr.dataType.name)
                    if attr.isArray: name += "\\texttt{[]}"
                    argTypes.append(name)

                returnType = method.function.returnType
                returnTypename = _translateTypeName(returnType.name) if returnType else "void"

                out.write("\\cline{1-2}\n")
                out.write("\\raggedright ")
                if (method.isStatic):
                    out.write("static ")
                if (method.isVirtual):
                    out.write("virtual ")

                if returnType:
                    out.write(texEscape(_translateTypeName(returnType.name)))
                    if method.function.returnArrayDim == 2:
                        out.write("\\texttt{[,]}")
                    elif method.function.returnArrayDim == 1:
                        out.write("\\texttt{[]}")

                    if returnType.publicType == "CLASS":
                        out.write(" (see page \\pageref{type_%s})" % returnType.name)
                else:
                    out.write("void")

                out.write(" %s(" % texEscape(method.function.name))
                out.write(", ".join(argTypes))
                out.write(") &\n")
                out.write("\\raggedright ")
                if len(method.implements):
                    out.write("Implements %s.\n" % method.implements)
                writeTexDescription(out, method.function.description)
                out.write("\nSee page \\pageref{function_%s.%s} for further details." % (
                         className, method.function.name))
                out.write(" & \\\\\n")

                allClassMethods.append(
                    _writeTexClassMethod(
                        cls, method, dirname, service, module, typesUsed, fns))

            for coerceTo in cls.coerceTo:
                out.write("\\cline{1-2}\n")
                out.write("\\raggedright ")
                out.write("operator %s() &\n" % texEscape(coerceTo.className))
                out.write("\\raggedright ")
                out.write("Converts to %s.\n" % texEscape(coerceTo.className))

                writeTexDescription(out, coerceTo.description)

                out.write(" & \\\\\n")
            writeTexEndTable(out, 2)

        constructors = serviceIndex.Constructors(className)

        if len(constructors) > 0:
            out.write("\n")
            out.write("\\textbf{Constructors:}\n")
            out.write("\\begin{itemize}\n")

            for constructor in sorted(constructors):
                out.write("  \\item %s (see page \\pageref{function_%s})\n" % (
                    texEscape(constructor), constructor))
            out.write("\\end{itemize}\n")


        if len(cls.coerceFrom):
            out.write("\n")
            out.write("\\textbf{Coercion:}\n")
            out.write("\n")
            out.write("Instances of %s can be coerced from other types as follows:\n" % (
                texEscape(cls.name)))

            writeTexBeginTable(out, 2, 150, "Type and Name", 310, "Description")

            for cf in cls.coerceFrom:
                attr       = cf.coerceFrom
                dataType   = attr.dataType
                publicType = dataType.publicType

                out.write("\\cline{1-2}\n")
                out.write("\\raggedright %s %s" % (
                    texEscape(_translateTypeName(dataType.name)), texEscape(attr.name)))

                if attr.arrayDim == 2:
                    out.write("\\texttt{[,]}")
                elif attr.arrayDim == 1: 
                    out.write("\\texttt{[]}")
                out.write(" &\n\\raggedright ")

                writeTexDescription(out, cf.description)

                # could we do this using data type instead?
                if publicType == "ENUM":
                    out.write("\nSee %s (page \\pageref{type_%s}).\n" % (
                        texEscape(dataType.name), dataType.name))
                elif publicType == "CLASS":
                    out.write("\nSee %s (page \\pageref{type_%s}).\n" % (
                        texEscape(dataType.name), dataType.name))
                out.write(" & \\\\\n")
            writeTexEndTable(out, 2)

    if len(cls.objectName) > 0 and cls.objectName != cls.name:
        out.write("\n")
        out.write("NOTE: When this class is serialized we will use the name `%s'"
                  " instead of `%s'\n" % (
                      texEscape(cls.objectName), texEscape(cls.name)))

    fns.add(filename)
    return filename

def _writeTexClassMethod(cls, method, dirname, service, module, typesUsed, fns):

    filename = "classMethod_%s_%s.tex" % (cls.dataType.name, method.function.name)
    
    out = GeneratedOutput(texFileName(dirname, filename))

    out.write("\\subsection{%s.%s} \\label{function_%s.%s}\n" % (
       texEscape(cls.dataType.name), texEscape(method.function.name),
       cls.dataType.name, method.function.name))

    out.write("\n")

    if len(method.function.description) == 0:
        out.write("No description.\n")
    else:
        writeTexDescription(out, method.function.description)

    _writeTexFunctionArgs(out, method.function, typesUsed)

    fns.add(filename)
    return filename


def _writeAttributesTable(out, attributes, noCrossRef):
    # annoyingly close to function arguments table
    # differences are that the list of attributes is of different type
    # and there are differences such as the access level
    #
    # we use this both for attributes and properties (which have a
    # separate table if they exist) - all properties are public
    # so the issue of accessibility is irrelevant

    writeTexBeginTable(out, 2, 150, "Type and Name", 310, "Description")

    for attr in attributes:
        dataType = attr.dataType
        publicType = dataType.publicType

        out.write("\\cline{1-2}\n")
        out.write("\\raggedright %s %s" % (
           texEscape(_translateTypeName(dataType.name)), texEscape(attr.name)))

        if attr.arrayDim == 2:
            out.write("\\texttt{[,]}")
        elif attr.arrayDim == 1:
            out.write("\\texttt{[]}")

        out.write(" &\n\\raggedright ")
        
        if attr.isOptional:
            defaultValue = attr.defaultValue.docString() if attr.defaultValue else ""
            if len(defaultValue) > 0:
                out.write("Optional (default = \\texttt{%s}). " % (
                    texEscape(defaultValue, True)))
            else:
                out.write("Optional. ")

        if not attr.accessible:
            out.write("Private. ")

        writeTexDescription(out, attr.description)

        if not dataType.noDoc and not noCrossRef:
            out.write("\nSee %s (page \\pageref{type_%s}).\n" % (
                     texEscape(dataType.name), dataType.name))
        out.write(" & \\\\\n")

    writeTexEndTable(out, 2)

def _writeTexFunction(func, dirname, service, module, hasClassMethods, typesUsed, fns):

    fullName = _fullName(module.ns, func.name)
    filename = "function_%s.tex" % fullName

    out = GeneratedOutput(texFileName(dirname, filename))

    if hasClassMethods:
        out.write("\\subsection{")
    else:
        out.write("\\section{")

    out.write("%s} " % texEscape(fullName))
    out.write("\\label{function_%s}\n" % fullName)

    out.write("\n")
    if len(func.description) == 0:
        out.write("No description.\n")
    else:
        writeTexDescription(out, func.description)

    if getOptions().writeIncludes:
        out.write("\n\\verb|#include <%s>|\n" % _moduleHeader(service, module))

    _writeTexFunctionArgs(out, func, typesUsed)

    fns.add(filename)
    return filename

def _writeTexFunctionArgs(out, func, typesUsed):

    out.write("\n")
    out.write("\\textbf{Inputs:}\n")
    out.write("\\nopagebreak\n")

    if len(func.inputs) == 0:
        out.write("None\n\n")
    else:
        _writeArgsTable(out, func.inputs, typesUsed)

    if len(func.outputs) > 0:
        out.write("\\textbf{Outputs:}\\nopagebreak\n")
    else:
        out.write("\\textbf{ReturnType:} ")

    if func.returnType:
        out.write(texEscape(func.returnType.name))
        if func.returnArrayDim == 2:
            out.write("\\texttt{[,]}")
        elif func.returnArrayDim == 1:
            out.write("\\texttt{[]}")

        if func.returnType.publicType == "CLASS":
            out.write(" (see page \\pageref{type_%s})" % func.returnType.name)

        if len(func.returnTypeDescription) > 0:
            out.write("\n\n")
            writeTexDescription(out, func.returnTypeDescription)
    elif len(func.outputs) == 0:
        out.write("void")
    else:
        # although one parameter outputs look the same in Excel and Python
        # there is a significant difference in .NET build
        # hence we always show the table even if there is only one output
        # besides in this way we can also show the description of the
        # output
        _writeArgsTable(out, func.outputs, typesUsed)

    out.write("\n\n")

def _writeTexEnum(enum, dirname, service, module, forImport, fns):

    constructName = _constructName(enum, module)
    filename = "enum_%s.tex" % constructName

    out = GeneratedOutput(texFileName(dirname, filename, forImport))

    out.write("\\subsection{%s} \\label{type_%s}\n" % (constructName, constructName))
    out.write("\n")
    if len(enum.description) == 0:
        out.write("No description.\n\n")
    else: writeTexDescription(out, enum.description)

    if getOptions().writeIncludes:
        out.write("\n\\verb|#include <%s>|\n" % _moduleClassesHeader(service, module))

    out.write("\\textbf{Possible Values:}\n")
    out.write("\\nopagebreak\n")

    writeTexBeginTable(out, 3, 100, "Code", 100, "Strings", 250, "Description")

    for enumerand in enum.enumerands:
        out.write("\\cline{1-3}\n")
        out.write("\\raggedright \\verb|%s| &\n" % enumerand.code)
        out.write("\\raggedright ")
        sep = ""
        for str in enumerand.strings:
            if len(sep): out.write(sep)
            sep = ", "
            if len(str) > 0:
                out.write("\\verb|%s|" % str)
            else:
                out.write("\\verb|\"\"|")
        out.write(" &\n")
        out.write("\\raggedright ")
        writeTexDescription(out, enumerand.description)
        out.write("& \\\\\n")

    writeTexEndTable(out, 2)

    out.write("\n\n")
    fns.add(filename)
    return filename

def _writeArgsTable(out, args, typesUsed):

    writeTexBeginTable(out, 2, 150, "Type and Name", 310, "Description")

    for arg in args:
        dataType   = arg.dataType
        publicType = dataType.publicType

        out.write("\\cline{1-2}\n")
        out.write("\\raggedright %s %s" % (
            texEscape(_translateTypeName(dataType.name)),
            texEscape(arg.name)))

        if arg.arrayDim == 2:
            out.write("\\texttt{[,]}")
        elif arg.arrayDim == 1:
            out.write("\\texttt{[]}")

        out.write(" &\n\\raggedright ")

        # Note: outputs won't be optional - this is the only part of this
        # litle function which might be different for outputs rather
        # than inputs.

        if arg.isOptional:
            defaultValue = arg.defaultValue.docString() if arg.defaultValue else ""

            if len(defaultValue):
                out.write("Optional (default = \\texttt{%s}). " % texEscape(
                    defaultValue, True))
            else:
                out.write("Optional. ")

        writeTexDescription(out, arg.description)

        # for some reason imported types do not seem to provide the noDoc
        # attribute - perhaps we haven't wrapped it properly
        if not dataType.noDoc:
            out.write("\nSee %s (page \\pageref{type_%s}).\n" % (
                texEscape(dataType.name), dataType.name))

            typesUsed.add(dataType.name)

        out.write(" & \\\\\n")

    writeTexEndTable(out, 2)



def _constructName(construct, module):
    return _fullName(module.ns, construct.name)

def _fullName(*parts):
    return ".".join([part for part in parts if len(part) > 0])

_builtinTypes = {
    "bool" : "bool",
    "int" : "int",
    "double" : "double",
    "string" : "string",
    "date" : "Date",
    "char" : "char",
    "long" : "int",
    "size_t" : "unsigned int"}

def _translateTypeName(tn):
    global _builtinTypes
    return _builtinTypes.get(tn, tn)

def addModuleToServiceIndex(module, serviceIndex):

    for construct in module.constructs:
        constructType = construct.getType()

        if constructType == "CLASS":
            cls = construct
            if len(cls.baseClassName):
                parts = []
                if len(module.ns): parts.append(module.ns)
                parts.append(cls.name)
                className = ".".join(parts)
                serviceIndex.AddDerivedClass(cls.baseClassName, className)

            for method in cls.methods:
                if len(method.implements) == 0:
                    _addFunctionToServiceIndex(module, cls.name, method.function, serviceIndex)
                serviceIndex.IncrementNumClassMethods()

        elif constructType == "FUNCTION":
            func = construct
            _addFunctionToServiceIndex(module, "", func, serviceIndex)

def _addFunctionToServiceIndex(module, className, func, serviceIndex):

    parts = []
    if len(module.ns): parts.append(module.ns)
    if len(className): parts.append(className)
    parts.append(func.name)
    funcName = ".".join(parts)

    if func.returnType and func.returnType.publicType == "CLASS":
        serviceIndex.AddConstructor(func.returnType.name, funcName)

    for output in func.outputs:
        if output.dataType.publicType == "CLASS":
            serviceIndex.AddConstructor(output.dataType.name, funcName)


