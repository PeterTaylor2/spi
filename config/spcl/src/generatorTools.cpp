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
#include "generatorTools.hpp"
#include "converter.hpp"
#include "dataType.hpp"
#include "verbatim.hpp"
#include <spgtools/commonTools.hpp>
#include <spgtools/namespaceManager.hpp>

#include <algorithm>
#include <sstream>

#include <spi/StringUtil.hpp>

static Options g_options;

void writeFunctionArgs(
    GeneratedOutput& ostr,
    bool innerContext,
    const std::vector<AttributeConstSP>& inputs,
    const std::vector<AttributeConstSP>& outputs,
    bool allowOptional,
    size_t indent,
    const char* prefix)
{
    ostr << "(";

    std::string indentString(indent, ' ');

    const char* sep = "";
    size_t firstOptional = allowOptional && outputs.size() == 0 ?
        firstOptionalInput(inputs) : inputs.size();
    for (size_t i = 0; i < inputs.size(); ++i)
    {
        ostr << sep << "\n" << indentString;
        writeFunctionArg(ostr, innerContext, inputs[i], false, prefix, i >= firstOptional);
        sep = ",";
    }
    for (size_t i = 0; i < outputs.size(); ++i)
    {
        ostr << sep << "\n" << indentString;
        writeFunctionArg(ostr, innerContext, outputs[i], true, prefix);
        sep = ",";
    }

    ostr << ")";
}

void writeFunctionInputs(
    GeneratedOutput& ostr,
    bool innerContext,
    const std::vector<AttributeConstSP>& inputs,
    bool allowOptional,
    size_t indent,
    const char* prefix)
{
    writeFunctionArgs(ostr, innerContext, inputs,
        std::vector<AttributeConstSP>(), allowOptional, indent, prefix);
}

void writeFunctionArg(
    GeneratedOutput& ostr,
    bool innerContext,
    const AttributeConstSP& arg,
    bool isOutput,
    const char* prefix,
    bool provideDefault)
{
    const DataTypeConstSP& dataType    = arg->dataType();
    const std::string&     name        = arg->name();
    int                    arrayDim    = arg->arrayDim();
    bool                   isOptional  = arg->isOptional() || arrayDim > 0;
    //bool                   isArray     = arg->isArray();

    if (arrayDim > 0)
    {
        if (!isOutput)
            ostr << "const ";
        if (innerContext)
            ostr << dataType->innerArrayType(arrayDim);
        else
            ostr << dataType->outerArrayType(arrayDim);
        ostr << "&";
    }
    else
    {
        std::string referenceType = innerContext ?
            dataType->innerReferenceType() :
            dataType->outerReferenceType();

        if (isOutput)
        {
            // this de-constification seems quite ugly
            //
            // perhaps innerReferenceType and outerReferenceType
            // should have isOutput as a parameter?
            if (spi_util::StringStartsWith(referenceType, "const "))
                referenceType = referenceType.substr(6);
            if (!spi_util::StringEndsWith(referenceType, "&"))
                referenceType += "&";
        }
        ostr << referenceType;
    }
    ostr << " " << prefix << name;
    if (provideDefault && isOptional)
    {
        if (arrayDim > 0)
        {
            ostr << " = {}";
        }
        else
        {
            ostr << " = " << arg->defaultValue()->toCode();
        }
    }
}

void writeArgsCall(
    GeneratedOutput& ostr,
    bool innerContext,
    const std::vector<AttributeConstSP>& args,
    size_t startPos,
    size_t indent,
    const char* translationPrefix)
{
    const size_t maxLine = 78;
    std::vector<std::string> argNames;

    for (size_t i = 0; i < args.size(); ++i)
    {
        const DataTypeConstSP& dataType = args[i]->dataType();
        const std::string&     name     = args[i]->name();
        const char* prefix;

        if (innerContext && dataType->needsTranslation())
            prefix = translationPrefix;
        else
            prefix = "";

        argNames.push_back(prefix + name);
    }

    // call the more general routine of the same name in genutils
    writeArgsCall(ostr, argNames, startPos, indent, maxLine);
}


void writeArgsInitialisers(
    GeneratedOutput& ostr,
    bool started,
    const std::vector<AttributeConstSP>& args,
    size_t indent,
    const char* prefix)
{
    std::string indentString(indent, ' ');

    size_t nbArgs = args.size();
    if (nbArgs > 0)
    {
        if (started)
            ostr << ",\n";
        else
            ostr << indentString << ":\n";
    }
    for (size_t i = 0; i < nbArgs; ++i)
    {
        const std::string& name = args[i]->name();
        ostr << indentString << prefix << name << "(" << name << ")";
        if (i+1 < nbArgs)
            ostr << ",\n";
    }
}

void writeDeclareArgs(
    GeneratedOutput& ostr,
    bool innerContext,
    const std::vector<AttributeConstSP>& args,
    size_t indent,
    const char* prefix,
    bool isConst,
    const char* conversionPrefix,
    bool noDeclareNoConvert)
{
    std::string indentString(indent, ' ');

    for (size_t i = 0; i < args.size(); ++i)
    {
        const DataTypeConstSP& dataType = args[i]->dataType();
        const std::string&     name     = args[i]->name();
        int arrayDim = args[i]->arrayDim();
        bool needsConversion = innerContext && dataType->needsTranslation();

        if (noDeclareNoConvert && !needsConversion)
            continue;

        ostr << indentString;

        if (isConst)
            ostr << "const ";

        if (arrayDim > 0)
        {
            if (innerContext)
                ostr << dataType->innerArrayType(arrayDim);
            else
                ostr << dataType->outerArrayType(arrayDim);
        }
        else
        {
            if (innerContext)
                ostr << dataType->innerValueType();
            else
                ostr << dataType->outerValueType();
        }

        ostr << " " << prefix;

        if (needsConversion)
            ostr << conversionPrefix;

        if (!innerContext && name == "self") // reserved name
            ostr << "self(this);\n";
        else
            ostr << name << ";" << '\n';
    }
}

std::vector<AttributeConstSP> selectArgsNeedingConversion(
    const std::vector<AttributeConstSP>& args)
{
    std::vector<AttributeConstSP> out;

    for (size_t i = 0; i < args.size(); ++i)
    {
        if (args[i]->dataType()->needsTranslation())
            out.push_back(args[i]);
    }

    return out;
}

void writeGetClassAttributesForMap(
    GeneratedOutput& ostr,
    const std::vector<ClassAttributeConstSP>& attrs,
    std::set<std::string>& done,
    size_t indent,
    bool isClosed)
{
    std::string indentString(indent, ' ');
    bool addBlankLine = false;
    for (size_t i = 0; i < attrs.size(); ++i)
    {
        const AttributeConstSP& attr    = attrs[i]->attribute();
        const DataTypeConstSP& dataType = attr->dataType();
        const std::string&     name     = attr->name();
        if (done.count(name) != 0)
            continue;
        done.insert(name);
        addBlankLine = true;
        int arrayDim = attr->arrayDim();

        ostr << indentString;
        if (arrayDim > 0)
        {
            ostr << "const " << dataType->outerArrayType(arrayDim) << "&";
        }
        else
        {
            ostr << dataType->outerReferenceType();
        }
        ostr << " " << name << " = ";
        if (isClosed)
        {
            ostr << "m_" << name << ";\n";
        }
        else
        {
            ostr << "this->" << name << "();\n";
        }
    }

    if (addBlankLine)
        ostr << "\n";
}

void writeToMap(
    GeneratedOutput& ostr,
    const std::vector<ClassAttributeConstSP>& attrs,
    const char* objectMap,
    const char* getDataFormat,
    size_t indent)
{
    std::string indentString(indent, ' ');

    bool publicOnlyBlock = false;
    // need to be able to serialize and need to be able to get
    // however should not appear in list of attributes
    // ostr << indentString << objectMap << "->SetString(\"object_id\", object_id(), true);\n";
    for (size_t i = 0; i < attrs.size(); ++i)
    {
        const AttributeConstSP& attr       = attrs[i]->attribute();
        const DataTypeConstSP& dataType    = attr->dataType();
        const std::string& name            = attr->name();
        int arrayDim                       = attr->arrayDim();
        bool canHide                       = attrs[i]->canHide();
        const std::string& hideIf          = attrs[i]->hideIf();
        const ConstantConstSP& valueToHide = attr->defaultValue();

        ClassAttributeAccess   accessLevel = attrs[i]->accessLevel();

        std::string data = spi::StringFormat(getDataFormat, name.c_str());

        SPI_PRE_CONDITION(accessLevel != ClassAttributeAccess::PROPERTY);
        if (accessLevel == ClassAttributeAccess::PRIVATE)
        {
            if (!publicOnlyBlock)
            {
                ostr << indentString << "if (!public_only)\n"
                     << indentString << "{\n";
                publicOnlyBlock = true;
            }

            ostr << indentString << "    "
                 << dataType->toMapCode(objectMap, name, data, arrayDim,
                                        canHide, valueToHide, hideIf)
                 << ";\n";
        }
        else
        {
            if (publicOnlyBlock)
            {
                ostr << indentString << "}\n";
                publicOnlyBlock = false;
            }
            ostr << indentString
                 << dataType->toMapCode(objectMap, name, data, arrayDim,
                                        canHide, valueToHide, hideIf)
                 << ";\n";
        }
    }
    if (publicOnlyBlock)
    {
        ostr << indentString << "}\n";
        publicOnlyBlock = false;
    }
}

void writePropertiesToMap(
    GeneratedOutput& ostr,
    const std::vector<ClassAttributeConstSP>& properties,
    const char* objectMap,
    const char* getDataFormat,
    size_t indent,
    bool hasDynamicProperties)
{
    if (properties.size() == 0 && !hasDynamicProperties)
        return;

    std::string indentString(indent, ' ');

    ostr << "    if (public_only)\n"
         << "    {\n";

    for (size_t i = 0; i < properties.size(); ++i)
    {
        const AttributeConstSP& attr = properties[i]->attribute();
        const DataTypeConstSP& dataType = attr->dataType();
        const std::string&     name     = attr->name();
        int arrayDim = attr->arrayDim();
        ClassAttributeAccess   accessLevel = properties[i]->accessLevel();

        std::string data = spi::StringFormat(getDataFormat, name.c_str());

        SPI_PRE_CONDITION(accessLevel == ClassAttributeAccess::PROPERTY);

        ostr << indentString << "    "
             << dataType->toMapCode(objectMap, name, data, arrayDim)
             << ";\n";
    }

    if (hasDynamicProperties)
    {
        ostr << "        dynamic_properties(obj_map);\n";
    }

    ostr << "    }\n";
}

void writeFromMap(
    GeneratedOutput& ostr,
    const std::vector<ClassAttributeConstSP>& args,
    const std::string& constructor,
    const char* objectMap,
    const char* valueToObject,
    size_t indent)
{
    std::string indentString(indent, ' ');

    for (size_t i = 0; i < args.size(); ++i)
    {
        const AttributeConstSP attr = args[i]->attribute();
        const ConverterConstSP& converter = args[i]->converter();

        const DataTypeConstSP& dataType     = attr->dataType();
        const std::string&     name         = attr->name();
        int                    arrayDim     = attr->arrayDim();
        bool                   isOptional   = attr->isOptional();
        const ConstantConstSP& defaultValue = attr->defaultValue();

        ostr << indentString;

        if (converter)
        {
            if (arrayDim > 0)
            {
                ostr << dataType->outerArrayType(arrayDim);
            }
            else
            {
                ostr << dataType->outerValueType();
            }
            ostr << " " << name << ";\n"
                << indentString << "if (!" << objectMap << "->Exists(\"" << name << "\"))\n"
                << indentString << "{\n";

            const std::vector<AttributeConstSP>& convertedAttributes = converter->attributes();
            size_t N = convertedAttributes.size();
            for (size_t j = 0; j < N; ++j)
            {
                const AttributeConstSP attr = convertedAttributes[j];
                const DataTypeConstSP& dataType = attr->dataType();
                const std::string&     name = attr->name();
                int                    arrayDim = attr->arrayDim();
                bool                   isOptional = attr->isOptional();
                const ConstantConstSP& defaultValue = attr->defaultValue();

                ostr << indentString << "  ";
                if (arrayDim > 0)
                {
                    ostr << "const " << dataType->outerArrayType(arrayDim) << "&";
                }
                else
                {
                    ostr << dataType->outerReferenceType();
                }
                if (dataType->needsTranslation())
                {
                    ostr << " o_" << name << " = "
                        << dataType->fromMapCode(objectMap, valueToObject, name,
                            arrayDim, isOptional, defaultValue)
                        << ";\n";
                }
                else
                {
                    ostr << " " << name << " = "
                        << dataType->fromMapCode(objectMap, valueToObject, name,
                            arrayDim, isOptional, defaultValue)
                        << ";\n";
                }
            }
            writeInnerDeclarationsAndTranslations(ostr, convertedAttributes, "", false, "o_", 6);
            writeVerbatim(ostr, converter->code(), indent + 2, true);

            ostr << indentString << "}\n"
                << indentString << "else\n"
                << indentString << "{\n"
                << indentString << "    " << name << " = "
                << dataType->fromMapCode(objectMap, valueToObject, name,
                    arrayDim, isOptional, defaultValue)
                << ";\n"
                << indentString << "}\n";
        }
        else
        {
            if (arrayDim > 0)
            {
                ostr << "const " << dataType->outerArrayType(arrayDim) << "&";
            }
            else
            {
                ostr << dataType->outerReferenceType();
            }
            ostr << " " << name << "\n"
                << indentString << "    = "
                << dataType->fromMapCode(objectMap, valueToObject, name,
                    arrayDim, isOptional, defaultValue)
                << ";\n";
        }
    }

    ostr << "\n"
        << indentString << "return " << constructor;

    writeArgsCall(ostr, false, ClassAttribute::getAttributeVector(args),
        constructor.length() + indent + 7,
        indent + 4);
    ostr << ";\n";

    //ostr << indentString << "std::string object_id = " << objectMap
    //    << "->GetString(\"object_id\", true);\n"
    //    << indentString << "if (!object_id.empty())\n"
    //    << indentString << "    o->set_object_id(object_id);\n"
    //    << "\n"
    //    << indentString << "return o;\n";
}

void writeInnerDeclarationsAndTranslations(
    GeneratedOutput& ostr,
    const std::vector<AttributeConstSP> args,
    const char* innerPrefix,
    bool  declareAll,
    const char* outerPrefix,
    size_t indent)
{
    std::string indentString(indent, ' ');
    std::vector<AttributeConstSP> myArgs = selectArgsNeedingConversion(args);

    writeDeclareArgs(ostr, true, myArgs, indent, innerPrefix);

    if (myArgs.size() > 0)
    {
        ostr << "\n";
        for (size_t i = 0; i < myArgs.size(); ++i)
        {
            const AttributeConstSP& arg      = myArgs[i];
            const DataTypeConstSP&  dataType = arg->dataType();
            const std::string&      argName  = arg->name();
            bool optional = arg->isOptional();
            int arrayDim = arg->arrayDim();

            switch(arrayDim)
            {
            case 2:
                {
                std::string matrixElement = outerPrefix + argName + "[i_][j_]";
                std::string validation = dataType->validateOuter(matrixElement, optional);
                std::string translation = dataType->translateOuter(matrixElement);
                ostr << indentString << "{\n"
                    << indentString << "  size_t nr_ = " << outerPrefix << argName
                    << ".Rows();\n"
                    << indentString << "  size_t nc_ = " << outerPrefix << argName
                    << ".Cols();\n"
                    << indentString << "  " << innerPrefix << argName << ".Resize(nr_, nc_);\n"
                    << indentString << "  for (size_t i_ = 0; i_ < nr_; ++i_)\n"
                    << indentString << "  {\n"
                    << indentString << "    for (size_t j_ = 0; j_ < nc_; ++j_)\n"
                    << indentString << "    {\n";

                if (!validation.empty())
                {
                    ostr << indentString << "    " << validation << ";\n";
                }

                ostr << indentString << "      " << innerPrefix << argName << "[i_][j_] = " << translation << ";\n"
                    << indentString << "    }\n"
                    << indentString << "  }\n"
                    << indentString << "}\n";
                }
                break;
            case 1:
                {
                std::string arrayElement = outerPrefix + argName + "[i_]";
                std::string validation = dataType->validateOuter(arrayElement, optional);
                std::string translation = dataType->translateOuter(arrayElement);
                ostr << indentString << "for (size_t i_ = 0; i_ < " << outerPrefix
                    << argName << ".size()" << "; ++i_)\n";

                if (!validation.empty())
                {
                    ostr << indentString << "{\n"
                        << indentString << "    " << validation << ";\n";
                }

                ostr << indentString << "    " << innerPrefix << argName << ".push_back(" << translation << ");\n";

                if (!validation.empty())
                    ostr << indentString << "}\n";

                }
                break;
            case 0:
                std::string validation = dataType->validateOuter(outerPrefix + argName, optional);
                if (!validation.empty())
                {
                    ostr << indentString << validation << ";\n";
                }
                ostr << indentString << innerPrefix << argName << " = "
                     << dataType->translateOuter(outerPrefix + argName)
                     << ";\n";
                break;
            }
        }
        //ostr << "\n";
    }

    if (declareAll)
    {
        bool newLineNeeded = true;
        for (size_t i = 0; i < args.size(); ++i)
        {
            if (args[i]->dataType()->needsTranslation())
                continue;

            // declare anyway but use a const reference to the input so that
            // no code will get executed
            const DataTypeConstSP& dataType = args[i]->dataType();
            const std::string&     name     = args[i]->name();
            int arrayDim = args[i]->arrayDim();

            if (newLineNeeded)
            {
                ostr << "\n";
                newLineNeeded = false;
            }
            ostr << indentString << "const ";
            if (arrayDim > 0)
            {
                ostr << dataType->innerArrayType(arrayDim);
            }
            else
            {
                ostr << dataType->innerValueType();
            }
            ostr << "& " << innerPrefix << name << " = "
                 << outerPrefix << name << ";\n";
        }
    }
}

void writeAccessorDeclaration(
    GeneratedOutput& ostr,
    const ClassAttributeConstSP& attr,
    const std::string& className,
    bool innerContext,
    bool usePropertyName)
{
    const std::string& name  = attr->attribute()->name();
    DataTypeConstSP dataType = attr->attribute()->dataType();
    int arrayDim = attr->attribute()->arrayDim();

    if (innerContext)
    {
        SPI_PRE_CONDITION(!usePropertyName);
        bool noConvert = attr->noConvert();
        if (!noConvert && dataType->needsTranslation())
        {
            ostr << "    static "; // in helper class
            if (arrayDim > 0)
                ostr << dataType->innerArrayType(arrayDim);
            else
                ostr << dataType->innerValueType();
            ostr << " get_" << name << "(const " << className
                 << "* o);\n"; // no const since static method in helper class
        }
    }
    else
    {
        ostr << "    ";
        if (arrayDim > 0)
            ostr << dataType->outerArrayType(arrayDim);
        else
            ostr << dataType->outerValueType();
        ostr << " " << (usePropertyName ? "" : "") << name << "() const;\n";
    }
}

void writeOpenAccessor(
    GeneratedOutput& ostr,
    const ClassAttributeConstSP& attr,
    const std::string& className,
    bool innerContext,
    bool hasVerbatim,
    bool usePropertyName,
    const char* accessorFormat)
{
    const std::string& name     = attr->attribute()->name();
    const VerbatimConstSP& code = attr->fromInnerCode();
    bool noConvert              = attr->noConvert();
    bool noCopy                 = attr->noCopy();
    DataTypeConstSP dataType    = attr->attribute()->dataType();
    int arrayDim = attr->attribute()->arrayDim();

    std::string nameAccessor = accessorFormat ?
        spi_util::StringFormat(accessorFormat, name.c_str()) :
        name;

    if (!innerContext)
    {
        // we are implementing the function in the main class
        // we are returning a value in the outer context
        ostr << "\n";
        if (arrayDim > 0)
            ostr << dataType->outerArrayType(arrayDim);
        else
            ostr << dataType->outerValueType();

        ostr << " " << className << "::" << (usePropertyName ? "" : "")
             << name << "() const\n"
             << "{";

        if (dataType->needsTranslation() && !noConvert)
        {
            // we need a helper function from the helper class
            ostr << "\n";
            // we call the helper and convert the output
            std::string value = spi::StringFormat("%s_Helper::get_%s(this)",
                                                  className.c_str(),
                                                  name.c_str());

            switch(arrayDim)
            {
            case 2:
                ostr << "    const " << dataType->innerArrayType(arrayDim)
                     << "& i_matrix = " << value << ";\n"
                     << "\n"
                     << "    size_t nr = i_matrix.Rows();\n"
                     << "    size_t nc = i_matrix.Cols();\n"
                     << "    " << dataType->outerArrayType(arrayDim)
                     << " matrix(nr, nc);\n"
                     << "\n"
                     << "    for (size_t i = 0; i < nr; ++i)\n"
                     << "        for (size_t j = 0; j < nc; ++j)\n"
                     << "            matrix[i][j] = "
                     << dataType->translateInner("i_matrix[i][j]", !noCopy) << ";\n"
                     << "\n"
                     << "    return matrix;\n";
                break;
            case 1:
                ostr << "    const " << dataType->innerArrayType(arrayDim)
                     << "& i_values = " << value << ";\n"
                     << "\n"
                     << "    " << dataType->outerArrayType(arrayDim) << " values;\n"
                     << "\n"
                     << "    for (size_t i_ = 0; i_ < i_values.size(); ++i_)\n"
                     << "        values.push_back("
                     << dataType->translateInner("i_values[i_]", !noCopy)
                     << ");\n"
                     << "\n"
                     << "    return values;\n";
                break;
            case 0:
                // one liner could do the trick for scalars, but let us
                // be safe rather than sorry and do it in two lines
                ostr << "    " << dataType->innerValueType()
                     << " i_value = " << value << ";\n"
                     << "    return " << dataType->translateInner("i_value", !noCopy)
                     << ";\n";
                break;
            }
            ostr << "}\n";
        }
        else if (noConvert)
        {
            // we don't even provide self in this case but we must have code
            if (!code)
                throw spi::RuntimeError("LOGICAL ERROR: noConvert but no code");
            writeVerbatim(ostr, code);
        }
        else
        {
            // helper not required if we don't need translation
            // we provide both self and o since the coder might not realize
            // that we don't need translation and uses o anyway
            ostr << "\n";
            if (hasVerbatim)
            {
                ostr << "    const " << className << "* o = this;\n";
            }
            ostr << "    inner_type self = get_inner();\n";

            if (!code)
            {
                ostr << "\n"
                     << "    return self->" << nameAccessor << ";\n"
                     << "}\n";
            }
            else
            {
                writeVerbatim(ostr, code);
            }
        }
    }
    else if (!noConvert && dataType->needsTranslation())
    {
        // now we are implementing the helper
        ostr << "\n";
        if (arrayDim > 0)
            ostr << dataType->innerArrayType(arrayDim);
        else
            ostr << dataType->innerValueType();

        ostr << " " << className << "_Helper::get_" << name << "(const "
             << className << "* o)\n"
             << "{\n"
             << "    " << className << "::inner_type self = o->get_inner();";

        // same implementation as above for when we didn't need
        // translation and could implement inside the main class
        if (!code)
        {
            ostr << "\n"
                 << "    return self->" << nameAccessor << ";\n"
                 << "}\n";
        }
        else
        {
            writeVerbatim(ostr, code);
        }
    }
}

void writeClosedAccessor(
    GeneratedOutput& ostr,
    const ClassAttributeConstSP& classAttr,
    const std::string& className)
{
    const AttributeConstSP& attr = classAttr->attribute();
    const std::string& name  = attr->name();
    DataTypeConstSP dataType = attr->dataType();
    int arrayDim = attr->arrayDim();

    ostr << "\n";
    if (arrayDim)
        ostr << dataType->outerArrayType(arrayDim);
    else
        ostr << dataType->outerValueType();
    ostr << " " << className << "::" << name << "() const\n"
         << "{\n"
         << "    return m_" << name << ";\n"
         << "}\n";
}

void writeObjectMethodDeclarations(
    GeneratedOutput& ostr,
    const std::string& className)
{
    ostr << "\n"
         //<< "    static " << className << "ConstSP from_string(\n"
         //<< "        const std::string& objectString);\n"
         //<< "\n"
         //<< "    static " << className << "ConstSP from_file(\n"
         //<< "        const char* filename);\n"
         //<< "\n"
         << "    SPI_DECLARE_OBJECT_TYPE(" << className << ");\n";
}

void writeObjectMethodImplementation(
    GeneratedOutput& ostr,
    const std::string& className)
{
    //ostr << "\n"
    //     << className << "ConstSP " << className << "::from_string(\n"
    //     << "    const std::string& objectString)\n"
    //     << "{\n"
    //     << "    std::istringstream iss(objectString);\n"
    //     << "    return from_stream(iss);\n"
    //     << "}\n"
    //     << "\n"
    //     << className << "ConstSP " << className << "::from_file(\n"
    //     << "    const char* filename)\n"
    //     << "{\n"
    //     << "    std::ifstream ifs(filename);\n"
    //     << "    if (!ifs)\n"
    //     << "        throw spi::RuntimeError(\"Could not open %s for reading object\", filename);\n"
    //     << "    return from_stream(ifs);\n"
    //     << "}\n";

}

namespace
{
    size_t countSpacesAtStartOfLine(const std::string& line)
    {
        size_t spaces = 0;
        while (line[spaces] == ' ')
            ++spaces;
        return spaces;
    }

    size_t countBlankLinesAtStartOfCode(const std::vector<std::string>& code)
    {
        size_t blanks = 0;
        for (size_t i = 0; i < code.size(); ++i)
        {
            std::string line = spi::StringStrip(code[i]);
            if (line.empty())
                ++blanks;
            else
                break;
        }
        return blanks;
    }

}

void writeInclude(
    GeneratedOutput& ostr,
    const std::string& header)
{
    if (header.length() == 0)
        return;

    if (header[0] == '<')
        ostr << "#include " << header << "\n";
    else
        ostr << "#include \"" << header << "\"\n";
}

void setGeneratorToolsOptions(const Options& options)
{
    g_options = options;
}

bool noGeneratedCodeNotice()
{
    return g_options.noGeneratedCodeNotice;
}

bool sessionLogging()
{
    return g_options.sessionLogging;
}

size_t firstOptionalInput(const std::vector<AttributeConstSP>& inputs)
{
    if (!g_options.optionalArgs)
    {
        return inputs.size();
    }
    size_t out = inputs.size();
    for (size_t i = inputs.size(); i > 0; --i)
    {
        SPI_PRE_CONDITION(i > 0);
        const AttributeConstSP& input = inputs[i - 1];
        bool isOptional = input->isOptional() || input->arrayDim() > 0;
        if (!isOptional)
            break;

        --out;
    }
    return out;
}

std::string tupleOutput(const std::vector<AttributeConstSP>& outputs,
    bool innerContext)
{
    SPI_PRE_CONDITION(outputs.size() > 0);

    std::ostringstream oss;

    oss << "std::tuple";
    const char* sep = "< ";

    for (size_t i = 0; i < outputs.size(); ++i)
    {
        oss << sep;

        const AttributeConstSP& output = outputs[i];
        const DataTypeConstSP& dataType = output->dataType();
        int arrayDim = output->arrayDim();

        if (innerContext)
        {
            if (arrayDim > 0)
            {
                oss << dataType->innerArrayType(arrayDim);
            }
            else
            {
                oss << dataType->innerValueType();
            }
        }
        else if (arrayDim > 0)
        {
            oss << dataType->outerArrayType(arrayDim);
        }
        else
        {
            oss << dataType->outerValueType();
        }

        sep = ", ";
    }

    oss << " >";
    return oss.str();
}

void writeCallFromTupleOutputFunction(
    GeneratedOutput& ostr,
    const std::vector<AttributeConstSP>& inputs,
    const std::vector<AttributeConstSP>& outputs,
    const std::string & name)
{
    SPI_PRE_CONDITION(outputs.size() > 0);
    // this should work both for standalone functions and class methods
    writeDeclareArgs(ostr, false, outputs, 4);

    std::vector<AttributeConstSP> args(inputs);
    args.insert(args.end(), outputs.begin(), outputs.end());

    ostr << "\n"
        << "    " << name;

    writeArgsCall(ostr, true, args, name.length() + 4, 8);
    ostr << ";\n";

    ostr << "\n"
        << "    return std::tie(";

    const char* sep = "";
    for (size_t i = 0; i < outputs.size(); ++i)
    {
        ostr << sep << outputs[i]->name();
        sep = ", ";
    }
    ostr << ");\n";
}

void writeVerbatim(
    GeneratedOutput& ostr,
    const VerbatimConstSP& verbatim,
    size_t indent,
    bool skipClosure)
{
    if (verbatim)
    {
        const std::vector<std::string>& code = verbatim->getCode();

        size_t nbLines = code.size();
        if (nbLines > 0 && skipClosure)
            --nbLines;

        bool stripLines = false;
        size_t stripLeft = 0;
        if (nbLines > 0)
        {
            stripLeft = countSpacesAtStartOfLine(code[nbLines-1]);
        }
        if (indent <= stripLeft)
        {
            stripLines = true;
            stripLeft = stripLeft - indent;
        }
        else
        {
            stripLines = false;
            stripLeft = 0;
        }

        size_t blankLines = countBlankLinesAtStartOfCode(code);
        const std::string& sourceFileName = verbatim->getSourceFilename();

        if (!g_options.noVerbatimLine && !sourceFileName.empty())
        {
            ostr << "\n#line " << verbatim->getLineNumber() + blankLines
                 << " \""  << ostr.relativePath(sourceFileName)
                 << "\"\n";
        }
        else
        {
            ostr << "\n";
        }

        if (stripLines)
        {
            for (size_t i = blankLines; i < nbLines; ++i)
            {
                const std::string& line = code[i];
                size_t spaces = countSpacesAtStartOfLine(line);
                size_t startFrom = std::min(spaces, stripLeft);
                ostr << line.substr(startFrom) << "\n";
            }
        }
        else
        {
            std::string indentString(indent, ' ');
            for (size_t i = blankLines; i < nbLines; ++i)
            {
                ostr << indentString << code[i] << "\n";
            }
        }
        if (!g_options.noVerbatimLine && !sourceFileName.empty())
        {
            ostr << "#line " << ostr.lines() + 2
                 << " \"" << ostr.relativePath(ostr.filename()) << "\"\n";
        }
    }
    else if (!skipClosure)
    {
        ostr << "}\n";
    }
}

std::string writeCallToInnerFunction(
    GeneratedOutput& ostr,
    const AttributeConstSP& returns,
    bool noConvert,
    const std::vector<AttributeConstSP>& inputs,
    const std::vector<AttributeConstSP>& outputs,
    const std::string& caller,
    bool declareOutputs)
{
    const char* translationPrefix = noConvert ? "" : "i_";
    if (outputs.size() > 0)
    {
        SPI_PRE_CONDITION(!returns);

        std::vector<AttributeConstSP> args(inputs);
        args.insert(args.end(), outputs.begin(), outputs.end());

        ostr << "\n"
             << "    " << caller;

        writeArgsCall(ostr, true, args, caller.length() + 4, 8,
            translationPrefix);
        ostr << ";\n";

        if (noConvert)
            return std::string();

        // translate the outputs (as required)
        //const char* gap = "";
        bool newLineNeeded = true;
        for (size_t i = 0; i < outputs.size(); ++i)
        {
            const Attribute* out = outputs[i].get();
            const DataType*  type = out->dataType().get();
            if (!type->needsTranslation())
                continue;

            if (newLineNeeded)
            {
                ostr << "\n";
                newLineNeeded = false;
            }

            // translate from i_<name> to name
            ostr << "    ";
            if (declareOutputs)
            {
                if (out->arrayDim() > 0)
                    ostr << type->outerArrayType(out->arrayDim());
                else
                    ostr << type->outerValueType();
                ostr << " ";
            }
            ostr << out->name();
            if (out->arrayDim() > 0)
            {
                if (!declareOutputs)
                    ostr << ".clear()";

                ostr << ";\n";
                ostr << "    for (size_t i_ = 0; i_ < i_" << out->name()
                     << ".size(); ++i_)\n"
                     << "        " << out->name() << ".push_back("
                     << type->translateInner("i_" + out->name() + "[i_]")
                     << ");\n";
                //gap = "";
            }
            else
            {
                ostr << " = "
                     << type->translateInner("i_" + out->name())
                     << ";\n";
                //gap = "\n";
            }
        }
        //ostr << gap;
        return std::string();
    }
    else if (!returns)
    {
        ostr << "    " << caller;
        writeArgsCall(ostr, true, inputs, caller.length() + 4, 8,
            translationPrefix);
        ostr << ";\n";
        return std::string();
    }
    else if (returns->arrayDim() > 0)
    {
        const int& returnArrayDim = returns->arrayDim();
        const DataTypeConstSP& returnType = returns->dataType();
        bool checkNonNull = g_options.checkNonNull && returns->checkNonNull();

        std::string oDecl = returnType->outerArrayType(returnArrayDim);
        std::string iDecl = returnType->innerArrayType(returnArrayDim);
        const std::string& decl = noConvert ? oDecl : iDecl;
        ostr << "    const " << decl << "& i_result = " << caller;
        writeArgsCall(ostr, true, inputs,
            caller.length() + decl.length() + 23, 8, translationPrefix);
        ostr << ";\n";

        if (!noConvert && returnType->needsTranslation())
        {
            switch (returnArrayDim)
            {
            case 2:
                ostr << "    " << oDecl << " o_result;\n"
                    << "    {\n"
                    << "      size_t nr = i_result.Rows();\n"
                    << "      size_t nc = i_result.Cols();\n"
                    << "      o_result.Resize(nr, nc);\n"
                    << "      for (size_t i = 0; i < nr; ++i)\n"
                    << "        for (size_t j = 0; j < nc; ++j)\n"
                    << "        {\n"
                    << "          o_result[i][j] = "
                    << returnType->translateInner("i_result[i][j]") << ";\n";
                if (checkNonNull)
                {
                    ostr << "          if (!o_result[i][j])\n"
                        << "            SPI_THROW_RUNTIME_ERROR(\"Null pointer returned @(\" << i << \",\" << j << \"));\"\n";
                }
                ostr << "        }\n"
                    << "    }\n";
                break;
            case 1:
                ostr << "    " << oDecl << " o_result;\n"
                    << "    for (size_t i_ = 0; i_ < i_result.size(); ++i_)\n"
                    << "    {\n" // strictly speaking only needed if checkNonNull
                    << "        o_result.push_back("
                    << returnType->translateInner("i_result[i_]") << ");\n";
                if (checkNonNull)
                {
                    ostr << "        if (!o_result[i_])\n"
                        << "            SPI_THROW_RUNTIME_ERROR(\"Null pointer returned @(\" << i_ << \")\");\n";
                }
                ostr << "    }\n";
                break;
            }
            return "o_result";
        }
        else if (checkNonNull)
        {
            switch (returnArrayDim)
            {
            case 2:
                ostr << "    {\n"
                    << "      size_t nr = i_result.Rows();\n"
                    << "      size_t nc = i_result.Cols();\n"
                    << "      for (size_t i = 0; i < nr; ++i)\n"
                    << "        for (size_t j = 0; j < nc; ++j)\n"
                    << "        {\n"
                    << "          if (!i_result[i][j])\n"
                    << "            SPI_THROW_RUNTIME_ERROR(\"Null pointer returned @(\" << i << \",\" << j << \"));\"\n"
                    << "        }\n"
                    << "    }\n";
                break;
            case 1:
                ostr << "    for (size_t i_ = 0; i_ < i_result.size(); ++i_)\n"
                    << "     {\n"
                    << "        if (!i_result[i_])\n"
                    << "            SPI_THROW_RUNTIME_ERROR(\"Null pointer returned @(\" << i_ << \")\");\n"
                    << "     }\n";
            }
        }
        return "i_result";
    }
    else
    {
        const DataTypeConstSP& returnType = returns->dataType();
        bool checkNonNull = g_options.checkNonNull && returns->checkNonNull();

        // because we are setting the value via the return from a function
        // we can use reference types here rather than value types
        std::string decl = noConvert ?
            returnType->outerReferenceType() :
            returnType->innerReferenceType();

        if (!spi::StringStartsWith(decl, "const "))
        {
            // the claim above is only true if the reference type is const
            decl = noConvert ?
                returnType->outerValueType() :
                returnType->innerValueType();
        }

        ostr << "    " << decl << " i_result = " << caller;
        writeArgsCall(ostr, true, inputs,
            caller.length() + decl.length() + 16, 8, translationPrefix);
        ostr << ";\n";

        if (!noConvert && returnType->needsTranslation())
        {
            ostr << "    " << returnType->outerValueType() << " o_result = "
                 << returnType->translateInner("i_result") << ";\n";
            if (checkNonNull)
            {
                ostr << "    if (!o_result)\n"
                    << "        SPI_THROW_RUNTIME_ERROR(\"Null pointer returned\");\n";
            }
            return "o_result";
        }
        if (checkNonNull)
        {
            ostr << "    if (!i_result)\n"
                << "        SPI_THROW_RUNTIME_ERROR(\"Null pointer returned\");\n";
        }
        return "i_result";
    }
}

/**
 * Write a function caller function and the corresponding FunctionCaller
 * data type.
 */
void writeFunctionCaller(
    GeneratedOutput&        ostr,
    const std::string&      ns,
    const std::string&      className,
    const std::string&      functionName,
    const DataTypeConstSP&  returnType,
    int                     returnArrayDim,
    const DataTypeConstSP&  instanceType,
    const ServiceDefinitionSP&           svc,
    const std::vector<AttributeConstSP>& inputs,
    const std::vector<AttributeConstSP>& outputs)
{
    std::string name;
    std::string rname;
    std::string caller;
    std::string functorClass;
    bool functionIsPermutable = returnArrayDim == 0;
    bool isConstructor = !className.empty() && functionName.empty();
    std::vector<AttributeConstSP> functorInputs;
    std::string fullNamespace = svc->fullNamespace(ns);

    if (className.empty())
    {
        // stand-alone function
        SPI_PRE_CONDITION(!instanceType);
        caller = fullNamespace + "::" + functionName;
        name   = functionName;
        rname  = functionName;
        //functorClass = "Func::" + functionName;
    }
    else if (functionName.empty())
    {
        // class constructor
        SPI_PRE_CONDITION(!instanceType);
        SPI_PRE_CONDITION(returnType);
        SPI_PRE_CONDITION(returnArrayDim == 0);
        functionIsPermutable = false;
        caller = fullNamespace + "::" + className + "::Make";
        name   = className;
        rname  = className;
    }
    else if (instanceType)
    {
        // regular class method
        caller = "self->" + functionName;
        name   = className + "_" + functionName;
        rname  = className + "." + functionName;

        // QUESTION: see classMethod::implement for query about using
        // 'implements' instead of 'className'

        functorClass = className + "_Helper::Func_" + functionName;
    }
    else
    {
        // static class method
        caller = fullNamespace + "::" + className + "::" + functionName;
        name   = className + "_" + functionName;
        rname  = className + "." + functionName;
        functorClass = className + "_Helper::Func_" + functionName;
    }

    // function declaration - does not appear in any header files
    // the function will be registered later in the same file
    ostr << "\n"
         << "spi::Value " << name << "_caller(\n"
         << "    const spi::InputContext*       in_context,\n"
         << "    const std::vector<spi::Value>& in_values)\n"
         << "{\n";

    size_t nbInstanceArgs = 0;

    // we are dealing with one of 4 types of function
    // regular class methods will have an instance type
    // we call it 'self' since nobody in their right mind
    // would have a parameter called self
    if (instanceType)
    {
        writeFromValueInContext(ostr, "self",
            "in_values[0]",
            instanceType,
            false,  // isArray
            false,  // isOptional
            ConstantConstSP());
        nbInstanceArgs = 1;
        functorInputs.push_back(Attribute::InstanceType(instanceType));
    }

    // these are the remaining arguments after the instance type
    for (size_t i = 0; i < inputs.size(); ++i)
    {
        const AttributeConstSP& arg = inputs[i];
        writeFromValueInContext(ostr, arg->name(),
            spi::StringFormat("in_values[%ld]", (long)(i+nbInstanceArgs)),
            arg->dataType(),
            arg->arrayDim(),
            arg->isOptional(),
            arg->defaultValue());
        functorInputs.push_back(arg);
    }

    if (inputs.size() > 0 || instanceType || outputs.size() > 0)
        ostr << "\n";

    // we have now collected all the inputs
    // now we call the function (whatever it might be)

    //if (isConstructor || noFunctor)
    {
        // we are calling the function directly
        //
        // either because we are calling a constructor (which never has
        // a functor) or because we are in a case where there is no functor
        if (outputs.size() > 0)
        {
            SPI_PRE_CONDITION(!returnType);

            writeDeclareArgs(ostr, false, outputs, 4);

            std::vector<AttributeConstSP> args(inputs);
            args.insert(args.end(), outputs.begin(), outputs.end());
            ostr << "\n"
                 << "    " << caller;
            writeArgsCall(ostr, false, args, caller.length() + 4, 8);
            ostr << ";\n";

            if (outputs.size() == 1)
            {
                const Attribute* output = outputs[0].get();
                ostr << "    return " << output->dataType()->toValueCode(
                    output->name(), output->arrayDim())
                     << ";\n";
            }
            else
            {
                ostr << "\n"
                     << "    std::vector<spi::Value> values_;\n";

                for (size_t i = 0; i < outputs.size(); ++i)
                {
                    const Attribute* output = outputs[i].get();
                    ostr << "    values_.push_back("
                         << output->dataType()->toValueCode(
                            output->name(), output->arrayDim())
                        << ");\n";
                }

                ostr << "\n"
                     << "    return spi::Value(values_);\n";
            }

        }
        else if (!returnType)
        {
            ostr << "    " << caller;
            writeArgsCall(ostr, false, inputs, caller.length() + 4, 8);
            ostr << ";\n"
                 << "    return spi::Value();\n";
        }
        else if (returnArrayDim > 0)
        {
            const std::string& decl = returnType->outerArrayType(returnArrayDim);
            ostr << "    const " << decl << "& o_result = " << caller;
            writeArgsCall(ostr, false, inputs,
                          caller.length() + decl.length() + 23, 8);
            ostr << ";\n";
            ostr << "    return "
                 << returnType->toValueCode("o_result", returnArrayDim)
                 << ";\n";
        }
        else
        {
            const std::string& decl = returnType->outerReferenceType();
            ostr << "    " << decl << " o_result = " << caller;
            writeArgsCall(ostr, false, inputs,
                          caller.length() + decl.length() + 16, 8);
            ostr << ";\n";

            ostr << "    return "
                 << returnType->toValueCode("o_result", 0)
                 << ";\n";
        }
    }
    //else
    //{
    //    // if we are not calling a constructor then it is all much simpler
    //    // we simply create the functor and call it
    //    writeMakeAndCallFunctorAndReturn(
    //        ostr, true, functorClass, svc, functorInputs,
    //        returnType, returnArrayDim, outputs);
    //}

    ostr << "}\n";

    // now we write the FunctionCaller object - only needs the inputs
    ostr << "\n"
         << "spi::FunctionCaller " << name << "_FunctionCaller = {\n"
         << "    \"" << makeNamespaceSep(ns, ".") << rname << "\",\n"
         << "    " << (inputs.size()+nbInstanceArgs) << ",\n"
         << "    {";

    if (instanceType)
    {
        ostr << "\n        {"
             << "\"self\", spi::ArgType::OBJECT"
             << ", \"" << instanceType->name() << "\""
             << ", false, false, false}";
    }

    for (size_t i = 0; i < inputs.size(); ++i)
    {
        const AttributeConstSP& arg = inputs[i];
        const DataTypeConstSP&  type = arg->dataType();
        if (nbInstanceArgs > 0 || i > 0)
            ostr << ",";
        ostr << "\n        {"
             << "\"" << arg->name() << "\""
             << ", spi::ArgType::" << type->argType()
             << ", \"" << type->name() << "\"";

        int  arrayDim     = arg->arrayDim();
        bool isOptional   = arg->isOptional();
        bool isPermutable = arrayDim == 0 && functionIsPermutable;

        ostr << ", " << (arrayDim > 0 ? "true" : "false")
             << ", " << (isOptional ? "true" : "false")
             << ", " << (isPermutable ? "true" : "false");
        ostr << "}";
    }

    if (nbInstanceArgs > 0 || inputs.size() > 0)
        ostr << "\n    ";

    ostr << "},\n"
         << "    " << name << "_caller\n"
         << "};\n";
}

void writeFromValueInContext(
    GeneratedOutput&       ostr,
    const std::string&     name,
    const std::string&     value,
    const DataTypeConstSP& dataType,
    int                    arrayDim,
    bool                   isOptional,
    const ConstantConstSP& defaultValue)
{
    spdoc::PublicType publicType = dataType->publicType();
    if (arrayDim > 0)
    {
        ostr << "    " << dataType->outerArrayType(arrayDim) << " " << name
             << " =\n        ";
        bool converter = false;
        switch(publicType)
        {
        case spdoc::PublicType::BOOL:
            ostr << "in_context->ValueToBool";
            break;
        case spdoc::PublicType::CHAR:
            throw spi::RuntimeError("Array of char is not supported");
        case spdoc::PublicType::INT:
            ostr << "in_context->ValueToInt";
            break;
        case spdoc::PublicType::DOUBLE:
            ostr << "in_context->ValueToDouble";
            break;
        case spdoc::PublicType::STRING:
            ostr << "in_context->ValueToString";
            break;
        case spdoc::PublicType::DATE:
            ostr << "in_context->ValueToDate";
            break;
        case spdoc::PublicType::DATETIME:
            ostr << "in_context->ValueToDateTime";
            break;
        case spdoc::PublicType::ENUM:
            ostr << "spi::EnumVectorFromStringVector<"
                 << dataType->cppName()
                 << ">(in_context->ValueToString";
            converter = true;
            break;
        case spdoc::PublicType::CLASS:
            ostr << "in_context->ValueToInstance";
            if (arrayDim == 1)
            {
                ostr << "Vector<";
            }
            else if (arrayDim == 2)
            {
                ostr << "Matrix<";
            }
            // FIXME: check whether dataType is really const
            ostr << dataType->cppName() << " const>(" << value;
            if (isOptional)
                ostr << ", true";
            ostr << ");\n";
            return;
        case spdoc::PublicType::MAP:
            ostr << "in_context->ValueToInstance";
            if (arrayDim == 1)
            {
                ostr << "Vector<";
            }
            else if (arrayDim == 2)
            {
                ostr << "Matrix<";
            }
            ostr << "spi::MapObject>(" << value;
            if (isOptional)
                ostr << ", true";
            ostr << ");\n";
            return;
        case spdoc::PublicType::OBJECT:
            ostr << "in_context->ValueToObject";
            if (arrayDim == 1)
            {
                ostr << "Vector";
            }
            else if (arrayDim == 2)
            {
                ostr << "Matrix";
            }
            ostr << "(" << value;
            if (isOptional)
                ostr << ", NULL, true";
            ostr << ");\n";
            return;

        case spdoc::PublicType::VARIANT:
            ostr << "in_context->ValueToVariant";
            break;
        default:
            throw spi::RuntimeError("%s: unknown enum value %d", __FUNCTION__, (int)publicType);
        }
        if (arrayDim == 1)
        {
            ostr << "Vector(" << value;
        }
        else if (arrayDim == 2)
        {
            ostr << "Matrix(" << value;
        }
        if (isOptional)
        {
            ostr << ", true";
            if (defaultValue && !defaultValue->isUndefined())
            {
                ostr << ", " << defaultValue->toCode();
            }
        }

        if (converter)
            ostr << ")";
        ostr << ");\n";
    }
    else
    {
        ostr << "    " << dataType->outerReferenceType() << " " << name
             << " =\n        in_context->ValueTo";

        switch(publicType)
        {
        case spdoc::PublicType::BOOL:
            ostr << "Bool";
            break;
        case spdoc::PublicType::CHAR:
            ostr << "Char";
            break;
        case spdoc::PublicType::INT:
            ostr << "Int";
            break;
        case spdoc::PublicType::DOUBLE:
            ostr << "Double";
            break;
        case spdoc::PublicType::STRING:
            ostr << "String";
            break;
        case spdoc::PublicType::DATE:
            ostr << "Date";
            break;
        case spdoc::PublicType::DATETIME:
            ostr << "DateTime";
            break;
        case spdoc::PublicType::ENUM:
            ostr << "String";
            break;
        case spdoc::PublicType::CLASS:
            ostr << "Instance<" << dataType->cppName() << " const>";
            break;
        case spdoc::PublicType::VARIANT:
            ostr << "Variant";
            break;
        case spdoc::PublicType::OBJECT:
            ostr << "Object";
            break;
        case spdoc::PublicType::MAP:
            ostr << "Instance<spi::MapObject>";
            break;
        default:
            throw spi::RuntimeError("%s: unknown enum value %d", __FUNCTION__, (int)publicType);
        }
        ostr << "(" << value;
        if (isOptional)
        {
            if (publicType == spdoc::PublicType::OBJECT)
                ostr << ", NULL";

            ostr << ", true";
            if (defaultValue && !defaultValue->isUndefined())
            {
                ostr << ", " << defaultValue->toCode();
            }
        }
        ostr << ");\n";
    }
}

void writeFunctionConstructor(
    GeneratedOutput& ostr,
    const std::string& name,
    const std::string& serviceName,
    const std::string& ns,
    const std::string& className,
    const std::string& functionName,
    const std::vector<AttributeConstSP>& inputs,
    int indent)
{
    std::string spaces(indent+4, ' ');

    std::ostringstream fullFunctionName;
    if (!ns.empty())
        fullFunctionName << ns << ".";
    if (!className.empty())
        fullFunctionName << className << ".";
    fullFunctionName << functionName;

    ostr << spaces << "std::vector<spi::Value> args_;\n";
    for (size_t i = 0; i < inputs.size(); ++i)
    {
        AttributeConstSP input = inputs[i];
        ostr << spaces << "args_.push_back("
             << input->dataType()->castOuterTypeToValue(
                input->name(), input->arrayDim())
             << ");\n";
    }

    ostr << spaces << "spi::FunctionConstSP " << name
         << "(new spi::Function(\n"
         << spaces << "    " << serviceName << "_service(), "
         << "\"" << fullFunctionName.str() << "\", "
         << "args_));\n";
}

void writeFunctionObjectType(
    GeneratedOutput& ostr,
    const std::string& ns,
    const std::string& name,
    const std::string& serviceNamespace,
    const std::string& className)
{
    std::string objectTypeName;
    std::string fullClassName;

    {
        // the namespace is needed here since fullClassName is used
        // to register the object type
        std::ostringstream oss;
        oss << serviceNamespace << ".";
        if (!ns.empty())
            oss << ns << ".";
        if (!className.empty())
            oss << className << ".";
        oss << name;
        fullClassName = oss.str();
    }

    {
        // this code is written inside the namespace
        // hence we don't need to put the namespace in objectTypeName
        std::ostringstream oss;
        if (!className.empty())
            oss << className << "_";
        oss << name;
        oss << "_FunctionObjectType";
        objectTypeName = oss.str();
    }

    ostr << "\n"
         << "spi::ObjectType " << objectTypeName << " =\n"
         << "    spi::FunctionObjectType(\"" << fullClassName << "\");\n";
}

void writeFunctionOutputLogging(
    GeneratedOutput& ostr,
    const std::string& serviceName,
    const DataTypeConstSP& returnType,
    int returnArrayDim,
    const std::string& returnValue,
    const std::vector<AttributeConstSP>& outputs,
    size_t indent)
{
    if (returnValue.empty() && outputs.size() == 0)
        return; // void function with no output

    if (returnType && returnValue.empty())
        SPI_THROW_RUNTIME_ERROR("No returnValue for defined returnType");

    if (returnType && outputs.size() > 0)
        SPI_THROW_RUNTIME_ERROR("Cannot mix returnType and outputs");

    std::string spaces(indent+4, ' ');
    ostr << "\n"
         << spaces << "if (isLogging)\n"
         << spaces << "{\n";

    if (returnType)
    {
        ostr << spaces << "    " << serviceName << "_service()->log_output("
             << returnType->castOuterTypeToValue(returnValue, returnArrayDim)
             << ");\n";
    }
    else
    {
        ostr << spaces << "    spi::MapSP outputs_(new spi::Map(\"\"));\n";
        for (size_t i = 0; i < outputs.size(); ++i)
        {
            const Attribute* output = outputs[i].get();
            ostr << spaces << "    outputs_->SetValue(\""
                 << output->name() << "\", "
                 << output->dataType()->castOuterTypeToValue(
                    output->name(), output->arrayDim())
                << ");\n";
        }
        ostr << spaces << "    " << serviceName << "_service()->log_outputs(outputs_);\n";
    }
    ostr << spaces << "}\n";
}

void writeConstructorCatchBlock(
    GeneratedOutput&   ostr,
    const std::string& name)
{
    // we use the same primary string in all three cases in order to
    // be more efficient when using string pooling in the compilation
    ostr << "  catch (std::exception& e)\n"
         << "  { SPI_THROW_RUNTIME_ERROR(\"Could not create " << name
         << " object:\\n\" << e.what()); }\n"
         << "  catch (...)\n"
         << "  { SPI_THROW_RUNTIME_ERROR(\"Could not create " << name
         << " object:\\n\" << \"Unknown exception\"); }\n";
}

void writeFunctionCatchBlock(
    GeneratedOutput&   ostr,
    const std::string& serviceName,
    const std::string& name,
    const std::string& className)
{
    // we use the same primary string in all three cases in order to
    // be more efficient when using string pooling in the compilation

    std::string fullName;
    if (!className.empty())
        fullName = className + "." + name;
    else
        fullName = name;

    ostr << "  catch (std::exception& e)\n"
         << "  { throw " << serviceName << "_catch_exception("
         << "isLogging, \"" << fullName << "\", e); }\n"
         << "  catch (...)\n"
         << "  { throw " << serviceName << "_catch_all("
         << "isLogging, \"" << fullName << "\"); }\n";
}
