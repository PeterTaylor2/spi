/*

    Sartorial Programming Interface (SPI) code generators
    Copyright (C) 2012-2023 Sartorial Programming Ltd.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/
#include "texService.hpp"
#include "texModule.hpp"
#include "texIndex.hpp"
#include "texOptions.hpp"

#include <spgtools/generatedOutput.hpp>
#include <spgtools/commonTools.hpp>
#include <spgtools/texUtils.hpp>

#include <spi/RuntimeError.hpp>
#include <spi/StringUtil.hpp>
#include <spi_util/FileUtil.hpp>

#include <spi/spdoc_configTypes.hpp>

#include <algorithm>
#include <iostream>

namespace
{
    bool writeBackup()
    {
        return getOptions().writeBackup;
    }
}

static void tidyup(
    const spdoc::ServiceConstSP& svc,
    const std::string& dn,
    const std::set<std::string>& fns)
{
    std::set<std::string>::const_iterator iter;

    const char* texdns[2] = { "tex", "tex_imports" };

    for (size_t i = 0; i < 2; ++i)
    {
        const char* texdn = texdns[i];
        spi_util::Directory d(dn + "/" + texdn);
        for (iter = d.fns.begin(); iter != d.fns.end(); ++iter)
        {
            if (spi::StringEndsWith(*iter, ".tex"))
            {
                std::string ffn = spi_util::path::join(
                    dn.c_str(), texdn, iter->c_str(), NULL);
                if (!fns.count(*iter))
                {
                    std::cout << "Removing " << ffn << std::endl;
                    remove(ffn.c_str());
                }
            }
        }
    }
}

static void defineTypes(
    const char* typeOfType,
    const std::vector<std::string>& fns,
    std::set<std::string>& undefinedTypes)
{
    std::string expectedStart = spi_util::StringFormat("%s_", typeOfType);
    std::string expectedEnd = ".tex";

    size_t lenS = expectedStart.length();
    size_t lenE = expectedEnd.length();
    for (std::vector<std::string>::const_iterator iter = fns.begin();
        iter != fns.end(); ++iter)
    {
        const std::string& fn = *iter;
        SPI_PRE_CONDITION(spi_util::StringStartsWith(fn, expectedStart));
        SPI_PRE_CONDITION(spi_util::StringEndsWith(fn, expectedEnd));
        std::string tn = fn.substr(lenS, fn.length() - lenS - lenE);
        undefinedTypes.erase(tn);
    }
}

bool writeTexServiceDescription(
    const std::string&           dirname,
    const spdoc::ServiceConstSP& service,
    std::set<std::string>&       fns)
{
    GeneratedOutput ostr(texFileName(dirname, "service.tex"), writeBackup());
    writeTexDescription(ostr, service->description);
    fns.insert("service.tex");
    return ostr.close();
}

bool writeTexServiceDateStamp(
    const std::string&           dirname,
    const spdoc::ServiceConstSP& service,
    std::set<std::string>&       fns)
{
    static const std::vector<std::string> months = spi::StringSplit(
        "January,February,March,April,May,June,July,August,September,October,November,December",
        ',');

    GeneratedOutput ostr(texFileName(dirname, "datestamp.tex"), writeBackup());
    spi::Date today = spi::Date::Today();

    ostr << "\\date{" << today.Day() << " " << months[today.Month()-1]
         << " " << today.Year() << "}\n"
         << "\n";

    ostr << "\\newcommand{\\" << spi_util::StringReplace(service->ns, "_", "") << "VersionNumber}{" 
         << service->version << "}\n";

    fns.insert("datestamp.tex");
    return ostr.close();
}

bool writeTexServiceBuiltinTypes(
    const std::string&           dirname,
    const spdoc::ServiceConstSP& service,
    std::set<std::string>&       fns)
{
    GeneratedOutput ostr(texFileName(dirname, "builtinTypes.tex"), writeBackup());

    ostr << "\\section{Built-in types}\n\n";
    ostr << "Built-in types are data types that are provided as standard for "
         << "all services.\n"
         << "These types are the building blocks of all data representations.\n";

    writeTexBeginTable(ostr, 2, 100, "Typename", 360, "Description");

    writeTexTableRow(ostr, 2, "bool", "True or false.");
    writeTexTableRow(ostr, 2, "int", "Positive or negative integer (32-bits).");
    writeTexTableRow(ostr, 2, "double", "Double precision real number (64-bits).");
    writeTexTableRow(ostr, 2, "string", "Standard string type.");
    writeTexTableRow(ostr, 2, "Date", "!Date type. This will be translated "
        "into the appropriate type for each context. "
        "For Excel, this is the number of days since 1900. "
        "For Python, this is the \\texttt{datetime.date} class. "
        "For .NET this is the \\texttt{System.DateTime} class.");
    writeTexTableRow(ostr, 2, "char", "Single character.");
    writeTexTableRow(ostr, 2, "unsigned int", "Integer that will always be positive");

    writeTexEndTable(ostr, 2);

    fns.insert("builtinTypes.tex");
    return ostr.close();
}

bool writeTexServiceSimpleTypes(
    const std::string&              dirname,
    const spdoc::ServiceConstSP&    service,
    const std::vector<std::string>& fileNames,
    std::set<std::string>&          fns)
{
    GeneratedOutput ostr(texFileName(dirname, "simpleTypes.tex"), writeBackup());
    ostr << "\\section{Simple types}\n\n";
    ostr << "Simple types are types which are represented by one of the "
         << "standard types in the public interface (bool, int, double, "
         << "string, Date, char, unsigned int).\n\n";
    ostr << "However internally to the library the type is converted to some "
         << "other type understood by the library itself.\n\n";
    ostr << "This means that the user sees instances of the standard type "
         << "which are then converted (and possibly validated) to the type "
         << "understood by the library.\n\n";
    writeTexInputList(ostr, fileNames, true);
    fns.insert("simpleTypes.tex");
    return ostr.close();
}

bool writeTexServiceEnums(
    const std::string&              dirname,
    const spdoc::ServiceConstSP&    service,
    const std::vector<std::string>& fileNames,
    std::set<std::string>&          fns)
{
    GeneratedOutput ostr(texFileName(dirname, "enums.tex"), writeBackup());
    ostr << "\\section{Enumerated types}\n\n";
    ostr << "Enumerated types are types which can take one of a finite number "
         << "of possible values. At the C++ level, these are represented by "
         << "an enum inside a class. The possible values hence are given the "
         << "namespace of the class name which corresponds to the name defined "
         << "in the documentation.\n\n";
    ostr << "For the Excel interface, the values that can be used are strings. "
         << "These will then be converted into the C++ enumerated type, and "
         << "then used within the low-level library code.\n\n";
    ostr << "In the documentation, we will give the name of the enumerated "
         << "type, followed by a general description of how the type is used, "
         << "and then followed by a list of the possible values (known as the "
         << "enumerands).\n\n";
    writeTexInputList(ostr, fileNames, true);
    fns.insert("enums.tex");
    return ostr.close();
}

bool writeTexServiceClasses(
    const std::string&              dirname,
    const spdoc::ServiceConstSP&    service,
    const std::vector<std::string>& fileNames,
    std::set<std::string>&          fns)
{
    GeneratedOutput ostr(texFileName(dirname, "classes.tex"), writeBackup());
    ostr << "\\section{Classes}\n\n";
    ostr << "Classes contain data represented by attributes and can have "
         << "function represented by so-called class methods.\n\n";
    ostr << "Attributes will have a name and data type, and contain data of "
         << "the defined data type.\n"
         << "Attributes can be scalars or arrays.\n"
         << "The data type can be any previously defined data type, e.g "
         << "another class, or a simple type, or an enumerated type, or one "
         << "of the built-in primitive data types.\n\n";
    ostr << "Classes can in general be serialised.\n"
         << "This can be to a string format, or written to file in various "
         << "formats, or transmitted via the network when using a "
         << "client-server model.\n\n";
    ostr << "In general, all of the attributes will be needed when we "
         << "serialise the class, but not all of the attributes are needed "
         << "for general information about the class.\n"
         << "Hence some attributes are marked as private (only used in "
         << "serialisation), and others as public (can be accessed directly "
         << "from outside the class).\n"
         << "At present we do not support changing the value of an attribute "
         << "of class, but this may change in the future.\n\n";
    ostr << "Classes can also have class methods.\n"
         << "In the C++ interface these are C++ member functions.\n"
         << "In the Excel interface class methods appear as functions where "
         << "the name of the function starts with the class name followed by "
         << "the method name.\n"
         << "The first parameter of the function is the class instance (or "
         << "object).\n"
         << "In other interfaces to come later we will generally use the "
         << "natural representation for each language supported.\n"
         << "For example, Python and .NET both support classes, so we will "
         << "have equivalent classes in these languages corresponding to the "
         << "C++ class.\n\n";
    ostr << "In the C++ interface we will use a form of shared pointer for "
         << "representing the class.\n"
         << "The normal constructors will be protected and you need to use "
         << "the static \\texttt{Make} method for each class in order "
         << "to create the shared pointer instance for the class.\n"
         << "By protecting the constructor in this manner, we make it "
         << "impossible for client code to use raw pointers, and this should "
         << "make it easier to avoid memory leaks in client C++ code.\n\n";
    writeTexInputList(ostr, fileNames, true, true);
    fns.insert("classes.tex");
    return ostr.close();
}

bool writeTexImportedTypes(
    const std::string&              dirname,
    const spdoc::ServiceConstSP&    service,
    const std::set<std::string>&    undefinedTypes,
    const std::vector<std::string>& dnImports,
    std::set<std::string>&          fns)
{
    // what we do is loop through the undefinedTypes looking for potential
    // imports - if we find an import file we copy it locally - if we don't
    // find an import file we will simply list at the end of the document
    // the imports that we could not find - at least in this manner we will
    // have a valid cross-reference

    std::vector<std::string> fileNames;

    const char* prefixList[3] = {"enum_", "simpleType_", "class_"};
    size_t numDnImports = dnImports.size();
    std::vector<std::string> stillUndefined;
    for (std::set<std::string>::const_iterator iter = undefinedTypes.begin();
         iter != undefinedTypes.end(); ++iter)
    {
        bool found = false;
        for (size_t i = 0; i < numDnImports && !found; ++i)
        {
            for (size_t j = 0; j < 3 && !found; ++j)
            {
                const char* prefix = prefixList[j];
                std::string baseName = spi_util::StringFormat("%s%s.tex",
                    prefix, iter->c_str());
                std::string fileName = spi_util::path::join(
                    dnImports[i].c_str(),
                    baseName.c_str(),
                    0);

                if (spi_util::path::isfile(fileName))
                {
                    found = true;
                    // do we really need to copy the file?
                    // can't we just leave it where it is?
                    fileNames.push_back(fileName);
                }
            }
        }
        if (!found)
            stillUndefined.push_back(*iter);
    }

    GeneratedOutput ostr(texFileName(dirname, "importedTypes.tex"), writeBackup());
    ostr << "\\section{Imported types}\n\n";
    ostr << "Some types are defined by other services.\n"
         << "For a full description (including constructors) please refer to "
         << "the documentation for those services.\n\n";
    ostr << "In this document we will provide more limited information about "
         << "each of these (so-called) imported types.\n\n";

    for (std::vector<std::string>::const_iterator iter = fileNames.begin();
         iter != fileNames.end(); ++iter)
    {
        // no need to sort
        // no need to amend the file name
        // hence we don't call writeTextInputList
        ostr << "\\input{" << *iter << "}\n";
    }
    ostr << "\n";

    for (std::vector<std::string>::const_iterator iter = stillUndefined.begin();
         iter != stillUndefined.end(); ++iter)
    {
        ostr << "Undefined type: " << *iter 
             << " \\label{type_" << *iter << "}\n\n";
    }

    fns.insert("importedTypes.tex");
    return ostr.close();
}

bool writeTexServiceTypes(
    const std::string&              dirname,
    const spdoc::ServiceConstSP&    service,
    const std::vector<std::string>& simpleTypes,
    const std::vector<std::string>& enums,
    const std::vector<std::string>& classes,
    const std::set<std::string>&    typesUsed,
    const std::vector<std::string>& dnImports,
    std::set<std::string>&          fns)
{
    GeneratedOutput ostr(texFileName(dirname, "types.tex"), writeBackup());

    std::set<std::string> undefinedTypes(typesUsed);

    ostr << "\n"
         << "Each item of data in the library is of a particular data type.\n"
         << "Arbitrarily complex data types can be built up of simpler "
         << "types, with ultimately everything being broken down into the "
         << "fundamental built-in types.\n"
         << "\n"
         << "When describing the inputs to a function or data fields in a "
         << "class we will use the term \\texttt{attribute} indicating the "
         << "data type and name of the input or class data field.\n"
         << "Attributes in addition can be an array or a matrix.\n"
         << "We will indicate a simple array by the notation \\texttt{[]} "
         << "after the data type.\n"
         << "We will indicate a matrix by the notation \\texttt{[,]} after "
         << "the data type.\n"
         << "Note that a matrix is a rectangular array where each row in the "
         << "array has the same number of elements.\n"
         << "\n"
         << "In this section we describe all the possible data types "
         << "available in the library, starting from the built-in types and "
         << "building up to the classes available in the library.\n"
         << "\n";

    ostr << "\\input{tex/builtinTypes.tex}\n";
    if (simpleTypes.size() > 0)
    {
        defineTypes("simpleType", simpleTypes, undefinedTypes);
        writeTexServiceSimpleTypes(dirname, service, simpleTypes, fns);
        ostr << "\\newpage\n";
        ostr << "\\input{tex/simpleTypes.tex}\n";
    }
    if (enums.size() > 0)
    {
        defineTypes("enum", enums, undefinedTypes);
        writeTexServiceEnums(dirname, service, enums, fns);
        ostr << "\\newpage\n";
        ostr << "\\input{tex/enums.tex}\n";
    }
    if (classes.size() > 0)
    {
        defineTypes("class", classes, undefinedTypes);
        writeTexServiceClasses(dirname, service, classes, fns);
        ostr << "\\newpage\n";
        ostr << "\\input{tex/classes.tex}\n";
    }
    if (undefinedTypes.size() > 0)
    {
        writeTexImportedTypes(dirname, service, undefinedTypes, dnImports, fns);
        ostr << "\\newpage\n";
        ostr << "\\input{tex/importedTypes.tex}\n";
    }

    fns.insert("types.tex");
    return ostr.close();
}


bool writeTexServiceFunctions(
    const std::string&              dirname,
    const spdoc::ServiceConstSP&    service,
    const std::vector<std::string>& functions,
    const std::vector<std::string>& classMethods,
    std::set<std::string>&          fns)
{
    if (classMethods.size() > 0)
    {
        // we need three files - functionsAndClassMethods.tex, functions.tex, classMethods.tex
        // when we wrote the functions, each function had its own subsection
        GeneratedOutput fcmstr(texFileName(dirname, "functionsAndClassMethods.tex"), writeBackup());
        fcmstr << "In this library there are two types of functions.\n"
               << "We have free standing functions not attached to a "
               << "particular class, and we have class methods which are "
               << "attached to a class and for which you will need an "
               << "instance of the class (also known as an Object) in order "
               << "to make the call.\n"
               << "In addition there are also so-called \\texttt{static} "
               << "methods of a class which behave more like free standing "
               << "functions except that the name of the function also "
               << "includes the class name.\n"
               << "\n"
               << "Note that Excel does natively support the concept of class "
               << "methods and only supports functions.\n"
               << "Hence from within Excel when you have a class method you "
               << "are actually calling a function with one extra parameter "
               << "at the beginning of the argument list.\n"
               << "The function name in this case includes the class name as "
               << "well as the method name.\n"
               << "\n"
               << "For Python and .NET which do support classes, then a class "
               << "method is invoked by using the instance followed by "
               << "\\texttt{`.'} followed by the method name.\n"
               << "Note that it is a consequence of the Python language that "
               << "you can actually also use the Excel-style syntax for "
               << "calling a class method.\n"
               << "For \\texttt{static} methods in Python and .NET then you "
               << "need to use the class name instead of the instance when "
               << "calling the \\texttt{static} method.\n"
               << "\n"
               << "Another difference between Excel and the other supported "
               << "interfaces is that for Excel we represent Objects via "
               << "string handles.\n"
               << "For functions which create objects, there will be one "
               << "extra parameter to the function (or method).\n"
               << "This parameter is called \\texttt{baseName} and is needed "
               << "to provide the first part of the string which forms the "
               << "Object handle.\n"
               << "\\newpage\n"
               << "\\input{tex/functions.tex}\n"
               << "\\newpage\n"
               << "\\input{tex/classMethods.tex}\n"
               << "\n";
        fns.insert("functionsAndClassMethods.tex");
        fcmstr.close();

        GeneratedOutput cmstr(texFileName(dirname, "classMethods.tex"), writeBackup());
        cmstr << "\\section{Class methods}\n\n";
        cmstr << "These are the class methods (including \\texttt{static} "
              << "methods in the library).\n"
              << "\n"
              << "Note that when using the class method within Excel you will "
              << "need to use the overall namespace for the library as a "
              << "prefix to the full class method name.\n"
              << "\n"
              << "From within Python and .NET then you only need the last "
              << "part of the full class method name (using \\texttt{`.'} as "
              << "the separator and replace the rest of the full class method "
              << "name with an instance of that class (which is the first "
              << "part of the full class method name).\n"
              << "\n"
              << "There is an exception for \\texttt{static} methods.\n"
              << "In this case the calling convention is the same for Python "
              << "and .NET as for Excel - i.e. you need the full class "
              << "name prefixed by the overall namespace for the library.\n"
              << "\n";

        writeTexInputList(cmstr, classMethods, true);
        fns.insert("classMethods.tex");
        cmstr.close();
    }
    GeneratedOutput ostr(texFileName(dirname, "functions.tex"), writeBackup());
    if (classMethods.size() > 0)
    {
        ostr << "\\section{Functions}\n\n";
        ostr << "These are free standing functions which are not attached to "
             << "a particular class.\n"
             << "\n";

    }
    else
    {
        // since we have no class methods we have to replicate some of the
        // introductory text from above - however there are enough differences
        // to provide different content
        ostr << "This section describes the functions available in the "
             << "library.\n"
             << "\n"
             << "Note that when using Excel in conjunction with Objects "
             << "(an Object is an instance of a class), we represent Objects "
             << "via string handles.\n"
             << "For functions which create objects, there will be one "
             << "extra parameter to the function (or method).\n"
             << "This parameter is called \\texttt{baseName} and is needed "
             << "to provide the first part of the string which forms the "
             << "Object handle.\n"
             << "\n";
    }

    ostr << "Note that when using the function you will need to use the "
         << "overall namespace for the library as a prefix to the function "
         << "name.\n"
         << "\n";

    writeTexInputList(ostr, functions, true);
    fns.insert("functions.tex");
    return ostr.close();
}

bool writeTexServiceClassMethods(
    const std::string&              dirname,
    const spdoc::ServiceConstSP&    service,
    const std::vector<std::string>& fileNames,
    std::set<std::string>&          fns)
{
    GeneratedOutput ostr(texFileName(dirname, "classMethods.tex"), writeBackup());
    ostr << "\\section{Class methods}\n\n";
    writeTexInputList(ostr, fileNames, true);
    fns.insert("classMethods.tex");
    return ostr.close();
}

void writeTexService(
    const std::string&              dirname,
    const spdoc::ServiceConstSP&    service,
    const std::vector<std::string>& dnImports,
    const std::vector<spdoc::ServiceConstSP>& extraServices)
{
    std::set<std::string> fns;

    size_t numExtraServices = extraServices.size();

    for (size_t i = 0; i < numExtraServices; ++i)
    {
        const spdoc::ServiceConstSP& extraService = extraServices[i];
        if (service->ns != extraService->ns)
        {
            SPI_THROW_RUNTIME_ERROR("Namespace mismatch for extra service: " <<
                service->ns << " and " << extraService->ns);
        }
    }

    writeTexServiceDescription(dirname, service, fns);
    writeTexServiceDateStamp(dirname, service, fns);
    writeTexServiceBuiltinTypes(dirname, service, fns);

    std::vector<std::string> simpleTypes;
    std::vector<std::string> enums;
    std::vector<std::string> classes;
    std::vector<std::string> functions;
    std::vector<std::string> classMethods;
    std::set<std::string> typesUsed;

    ServiceIndex serviceIndex;

    for (size_t i = 0; i < service->modules.size(); ++i)
    {
        addModuleToServiceIndex(service->modules[i], serviceIndex);
    }

    for (size_t i = 0; i < service->modules.size(); ++i)
    {
        const spdoc::ModuleConstSP& module = service->modules[i];

        writeTexModuleConstructs(dirname, service, module, serviceIndex, 
                                 simpleTypes, enums, classes, functions,
                                 classMethods, typesUsed, fns);
    }

    for (size_t j = 0; j < numExtraServices; ++j)
    {
        const spdoc::ServiceConstSP& extraService = extraServices[j];
        for (size_t i = 0; i < extraService->modules.size(); ++i)
        {
            addModuleToServiceIndex(extraService->modules[i], serviceIndex);
        }

        for (size_t i = 0; i < extraService->modules.size(); ++i)
        {
            const spdoc::ModuleConstSP& module = extraService->modules[i];

            writeTexModuleConstructs(dirname, service, module, serviceIndex,
                simpleTypes, enums, classes, functions,
                classMethods, typesUsed, fns);
        }
    }

    writeTexServiceTypes(dirname, service, simpleTypes, enums, classes, 
        typesUsed, dnImports, fns);
    writeTexServiceFunctions(dirname, service, functions, classMethods, fns);

    tidyup(service, dirname, fns);
}

