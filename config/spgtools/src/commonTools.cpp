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
#include "commonTools.hpp"
#include "generatedOutput.hpp"
#include "templateLexer.hpp"
#include <spi_util/FileUtil.hpp>
#include <spi/StringUtil.hpp>

std::string TranslateFromValue(
    spdoc::PublicType  publicType,
    const std::string& dataTypeName,
    const std::string& value,
    int arrayDim)
{
    std::ostringstream oss;
    // let us make all the casts explicit for clarity (FIXME)

    //const char* arrayPrefix = "";
    //switch(arrayDim)
    //{
    //case 1: arrayPrefix = "Vector"; break;
    //case 2: arrayPrefix = "Matrix"; break;
    //}

    switch(publicType)
    {
    case spdoc::PublicType::BOOL:
        switch (arrayDim)
        {
        case 0:
            return value;
        case 1:
            oss << value << ".getBoolVector()";
            break;
        case 2:
            oss << "spi::MatrixData<bool>::FromValue(" << value << ")";
            break;
        default:
            throw spi::RuntimeError("PROGRAM_BUG");
        }
        break;
    case spdoc::PublicType::CHAR:
        if (arrayDim == 0)
            return value;
        throw spi::RuntimeError("Array of char is not supported");
    case spdoc::PublicType::INT:
        switch (arrayDim)
        {
        case 0:
            return value;
        case 1:
            oss << value << ".getIntVector()";
            break;
        case 2:
            oss << "spi::MatrixData<int>::FromValue(" << value << ")";
            break;
        default:
            throw spi::RuntimeError("PROGRAM_BUG");
        }
        break;
    case spdoc::PublicType::DOUBLE:
        switch (arrayDim)
        {
        case 0:
            return value;
        case 1:
            oss << value << ".getDoubleVector()";
            break;
        case 2:
            oss << "spi::MatrixData<double>::FromValue(" << value << ")";
            break;
        default:
            throw spi::RuntimeError("PROGRAM_BUG");
        }
        break;
    case spdoc::PublicType::STRING:
        switch (arrayDim)
        {
        case 0:
            oss << value << ".getString()";
            break;
        case 1:
            oss << value << ".getStringVector()";
            break;
        case 2:
            oss << "spi::MatrixData<std::string>::FromValue(" << value << ")";
            break;
        default:
            throw spi::RuntimeError("PROGRAM_BUG");
        }
        break;
    case spdoc::PublicType::DATE:
        switch (arrayDim)
        {
        case 0:
            return value;
        case 1:
            oss << value << ".getDateVector()";
            break;
        case 2:
            oss << "spi::MatrixData<spi::Date>::FromValue(" << value << ")";
            break;
        default:
            throw spi::RuntimeError("PROGRAM_BUG");
        }
        break;
    case spdoc::PublicType::DATETIME:
        switch (arrayDim)
        {
        case 0:
            return value;
        case 1:
            oss << value << ".getDateTimeVector()";
            break;
        case 2:
            oss << "spi::MatrixData<spi::DateTime>::FromValue(" << value << ")";
            break;
        default:
            throw spi::RuntimeError("PROGRAM_BUG");
        }
        break;
    case spdoc::PublicType::ENUM_AS_STRING:
        if (arrayDim > 0)
        {
            // FIXME
            throw spi::RuntimeError("%s: Array of enum is not supported", __FUNCTION__);
        }
        // unfortunately we need to represent an enum as a STRING
        // whenever we use Value since we might need to log it
         oss << spi::StringReplace(dataTypeName, ".", "::")
             << "(" << value << ".getString())";
        //oss << "(" << spi::StringReplace(dataTypeName, ".", "::")
        //    << "::Enum)(" << value << ".getInt())";

        break;
    case spdoc::PublicType::ENUM_AS_INT:
        if (arrayDim > 0)
        {
            // FIXME
            throw spi::RuntimeError("%s: Array of enum is not supported", __FUNCTION__);
        }
        // unfortunately we need to represent an enum as a STRING
        // whenever we use Value since we might need to log it
        oss << spi::StringReplace(dataTypeName, ".", "::")
            << "(" << value << ".getInt())";
        //oss << "(" << spi::StringReplace(dataTypeName, ".", "::")
        //    << "::Enum)(" << value << ".getInt())";

        break;
    case spdoc::PublicType::CLASS:
        switch (arrayDim)
        {
        case 0:
            // TBA: should we use spi::CoerceFromObject instead?
            // presumably they are identical in implementation
            oss << spi::StringReplace(dataTypeName, ".", "::") << "::Coerce("
                << value << ".getObject())";
            break;
        case 1:
            // FIXME: should know whether it needs const
            oss << "spi::CoerceObjectVector<"
                << spi::StringReplace(dataTypeName, ".", "::") << " const>("
                << value << ".getObjectVector())";
            break;
        case 2:
            throw spi::RuntimeError("Matrix of object not supported");
        default:
            throw spi::RuntimeError("PROGRAM_BUG");
        }
        break;
    case spdoc::PublicType::OBJECT:
        switch (arrayDim)
        {
        case 0:
            // TBA: should we use spi::CoerceFromObject instead?
            // presumably they are identical in implementation
            oss << value << ".getObject()";
            break;
        case 1:
            oss << value << ".getObjectVector()";
            break;
        case 2:
            throw spi::RuntimeError("Matrix of generic object not supported");
        default:
            throw spi::RuntimeError("PROGRAM_BUG");
        }
        break;
    case spdoc::PublicType::MAP:
        switch (arrayDim)
        {
        case 0:
            // TBA: should we use spi::CoerceFromObject instead?
            // presumably they are identical in implementation
            oss << "spi::MapObject::Coerce("
                << value << ".getObject())";
            break;
        case 1:
            // FIXME: should know whether it needs const
            oss << "spi::CoerceFromObjectVector<spi::MapObject>("
                << value << ".getObjectVector())";
            break;
        case 2:
            throw spi::RuntimeError("Matrix of object not supported");
        default:
            throw spi::RuntimeError("PROGRAM_BUG");
        }
        break;

    case spdoc::PublicType::VARIANT:
        return value;
    default:
        throw spi::RuntimeError("%s: unknown enum value %d", __FUNCTION__, (int)publicType);
    }

    return oss.str();
}

std::string TranslateToValue(
    spdoc::PublicType  publicType,
    const std::string& dataTypeName,
    const std::string& value,
    int arrayDim)
{
    std::ostringstream oss;
    // let us make all the casts explicit for clarity (FIXME)

    switch(publicType)
    {
    case spdoc::PublicType::BOOL:
    case spdoc::PublicType::INT:
    case spdoc::PublicType::DOUBLE:
    case spdoc::PublicType::STRING:
    case spdoc::PublicType::DATE:
    case spdoc::PublicType::DATETIME:
    case spdoc::PublicType::CHAR:
    case spdoc::PublicType::OBJECT:
        // for fundamental types we can use the standard Value constructor
        // or else the template constructor for a vector
        // or else the template cast operator for a matrix
        switch(arrayDim)
        {
        case 0:
        case 1:
            oss << "::spi::Value" << "(" << value << ")";
            break;
        case 2:
            oss << "(" << "::spi::Value" << ")(" << value << ")";
            break;
        default:
            throw spi::RuntimeError("arrayDim out of range");
        }
        break;
    case spdoc::PublicType::ENUM_AS_STRING:
        // for an enum although it seems inefficient we need to convert the
        // enumerated value to a string first
        switch (arrayDim)
        {
        case 0:
            oss << "::spi::Value" << "(" << value << ".to_string())";
            break;
        case 1:
            oss << "::spi::Value" << "(::spi::EnumVectorToStringVector(" << value << "))";
            break;
        case 2:
            // FIXME
            throw spi::RuntimeError("%s: Matrix of enum is not supported", __FUNCTION__);
        default:
            throw spi::RuntimeError("arrayDim out of range");
        }
        break;
    case spdoc::PublicType::ENUM_AS_INT:
        switch (arrayDim)
        {
        case 0:
            oss << "::spi::Value" << "(" << value << ".to_int())";
            break;
        case 1:
            oss << "::spi::Value" << "(::spi::EnumVectorToIntVector(" << value << "))";
            break;
        case 2:
            // FIXME
            throw spi::RuntimeError("%s: Matrix of enum is not supported", __FUNCTION__);
        default:
            throw spi::RuntimeError("arrayDim out of range");
        }
        break;
    case spdoc::PublicType::CLASS:
    case spdoc::PublicType::MAP:
        // for an object we have a constructor of Value from the base class
        // but not for the derived classes
        switch (arrayDim)
        {
        case 0:
            oss << value << "->to_value()";
            break;
        case 1:
            oss << "::spi::Object::to_value(::spi::MakeObjectVector<::"
                << spi::StringReplace(dataTypeName, ".", "::")
                << ">(" << value << "))";
            break;
        case 2:
            throw spi::RuntimeError("Matrix of object not supported");
        default:
            throw spi::RuntimeError("PROGRAM_BUG");
        }
        break;
    case spdoc::PublicType::VARIANT:
        if (arrayDim == 0)
            return value;
        // FIXME
        throw spi::RuntimeError("%s: array of Variant not supported", __FUNCTION__);
        break;
    default:
        throw spi::RuntimeError("%s: unknown enum value %d", __FUNCTION__, (int)publicType);
    }

    return oss.str();
}

std::string GetFirstParagraph(const std::vector<std::string>& description)
{
    std::vector<std::string> first;

    for (size_t i = 0; i < description.size(); ++i)
    {
        std::string line = spi::StringStrip(description[i]);
        if (line.length() == 0)
        {
            if (first.size() > 0)
                break;
        }
        else
        {
            first.push_back(line);
        }
    }

    return spi::StringJoin(" ", first);
}


std::string StringReplace(
    const std::string& in,
    const char* oldString,
    const char* newString)
{
    return spi::StringJoin(newString, spi::StringSplit(in, oldString));
}


/**
 * Splits a description into blocks where a blank line indicates a new
 * new paragraph.
 *
 * In addition each line of the description will be stripped of leading and
 * trailing white space.
 *
 * You will always get at least one block returned even if there are no
 * contents in the description.
 */
std::vector< std::vector<std::string> > SplitDescription(
    const std::vector<std::string>& description)
{
    std::vector< std::vector<std::string> > blocks;
    std::vector<std::string> block;

    for (size_t i = 0; i < description.size(); ++i)
    {
        const std::string& line = spi::StringStrip(description[i]);
        if (line.empty())
        {
            if (block.size() > 0)
                blocks.push_back(block);
            block.clear();
        }
        else
        {
            block.push_back(line);
        }
    }
    if (block.size() > 0 || blocks.size() == 0)
        blocks.push_back(block);

    return blocks;
}

/**
 * When writing the parameters for a function call, using this function will
 * try to fit as much into one line as possible for each line of the function
 * call.
 *
 * Will open and close the brackets.
 * Can be used for most target languages.
 */
void writeArgsCall(
    GeneratedOutput& ostr,
    const std::vector<std::string>& args,
    size_t startPos,
    size_t indent,
    size_t maxLine)
{
    ostr << "(";
    ++startPos;

    std::string indentString(indent, ' ');

    for (size_t i = 0; i < args.size(); ++i)
    {
        if (i > 0)
        {
            ostr << ',';
            ++startPos;
        }

        const std::string& name = args[i];

        if (name.length() + startPos >= maxLine)
        {
            ostr << "\n" << indentString;
            startPos = indent;
        }
        else if (i > 0)
        {
            ostr << " ";
            ++startPos;
        }

        ostr << name;
        startPos += name.length();
    }

    ostr << ")";
}


void generateFromTemplate(
    std::istream& istr,
    const std::string& ifn,
    const std::map<std::string, std::string>& values,
    const std::string& ofn,
    const std::string& cwd,
    bool writeBackup)
{
    TemplateLexer lexer(ifn, &istr);

    TemplateLexer::Token token = lexer.getToken();

    GeneratedOutput ostr(ofn, cwd, writeBackup);
    std::map<std::string, std::string>::const_iterator iter;

    while (token.type)
    {
        switch(token.type)
        {
        case TEMPLATE_TOKEN_TYPE_CHAR:
            ostr << token.value.aChar;
            break;
        case TEMPLATE_TOKEN_TYPE_NAME:
            iter = values.find(token.value.aName);
            if (iter == values.end())
                throw spi_util::RuntimeError("No value defined for '%s'",
                    token.value.aName);

            ostr << iter->second;
            break;
        default:
            throw spi_util::RuntimeError("Unknown token: %s",
                token.toString().c_str());
        }
        token = lexer.getToken();
    }
}

bool copyNewFile(
    const std::string& ifn,
    const std::string& ofn)
{
    double iUpdateTime = spi_util::FileLastUpdateTime(ifn);
    double oUpdateTime = spi_util::FileLastUpdateTime(ofn);

    if (iUpdateTime <= oUpdateTime)
        return false; // also deals with case that ifn == ofn

    std::string odn = spi_util::path::dirname(ofn);
    if (!odn.empty() && !spi_util::path::isdir(odn))
    {
        throw spi_util::RuntimeError("%s: '%s' is not a directory", 
            __FUNCTION__, odn.c_str());
    }

    std::ifstream ifs(ifn.c_str(), std::ios_base::binary);
    if (!ifs)
        throw spi_util::RuntimeError("%s: Cannot read input file '%s'", 
            __FUNCTION__, ifn.c_str());

    std::ofstream ofs(ofn.c_str(), std::ios_base::binary);
    if (!ofs)
        throw spi_util::RuntimeError("%s: Cannot write output file '%s'",
            __FUNCTION__, ofn.c_str());

    ofs << ifs.rdbuf();
    ofs.close();

    return true;
}
