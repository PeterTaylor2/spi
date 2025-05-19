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
/*
***************************************************************************
** coerceFrom.cpp
***************************************************************************
*/

#include "coerceFrom.hpp"

#include <spi/RuntimeError.hpp>
#include <spi/Service.hpp>
#include <spi/StringUtil.hpp>
#include <spi/spdoc_configTypes.hpp>

#include "classMethod.hpp"
#include "constant.hpp"
#include "innerClass.hpp"
#include "moduleDefinition.hpp"
#include "serviceDefinition.hpp"
#include "verbatim.hpp"
#include "generatorTools.hpp"

CoerceFromConstSP CoerceFrom::Make(
    const std::vector<std::string>& description,
    const AttributeConstSP& coerceFrom,
    const VerbatimConstSP&  code,
    bool convert)
{
    return CoerceFromConstSP(new CoerceFrom(description, coerceFrom, code, convert));
}

CoerceFrom::CoerceFrom(
    const std::vector<std::string>& description,
    const AttributeConstSP& coerceFrom,
    const VerbatimConstSP&  code,
    bool convert)
    :
    m_description(description),
    m_coerceFrom(coerceFrom),
    m_code(code),
    m_convert(convert)
{}

const DataTypeConstSP& CoerceFrom::getDataType() const
{
    return m_coerceFrom->dataType();
}

const std::string& CoerceFrom::getName() const
{
    return m_coerceFrom->name();
}

int CoerceFrom::arrayDim() const
{
    return m_coerceFrom->arrayDim();
}

const VerbatimConstSP& CoerceFrom::getCode() const
{
    return m_code;
}

void CoerceFrom::declare(
        GeneratedOutput& ostr,
        const std::string& className,
        const ServiceDefinitionConstSP& svc) const
{
    writeStartCommentBlock(ostr, true, 4);

    ostr << "    * Creates instance of " << className << " by coercion from "
         << m_coerceFrom->dataType()->name() << ".\n";
    if (m_description.size() > 0)
    {
        ostr << "    *\n";
        writeComments(ostr, m_description, 0, 4);
    }

    ostr << "    *\n";
    ostr << "    * @param " << m_coerceFrom->name() << "\n";
    writeComments(ostr, m_coerceFrom->description(), 4, 4);

    writeEndCommentBlock(ostr, 4);

    const DataTypeConstSP& classType = svc->getDataType(className);

    ostr << "    static " << classType->outerValueType()
         << " Coerce(";
    writeFunctionArg(ostr, false, m_coerceFrom);
    ostr << ");\n";
}

void CoerceFrom::implement(GeneratedOutput& ostr,
    const std::string& className,
    const ServiceDefinitionConstSP& svc) const
{
    const DataTypeConstSP& classType = svc->getDataType(className);

    if (!m_convert)
    {
        ostr << "\n"
            << classType->outerValueType()
            << " " << className << "::Coerce(";
        writeFunctionArg(ostr, false, m_coerceFrom);
        ostr << ")\n";

        ostr << "{";
        writeVerbatim(ostr, m_code);
    }
    else
    {
        ostr << "\n"
            << "namespace {\n"
            << "\n"
            << classType->innerValueType() << " " << className << "_Coerce(";
        writeFunctionArg(ostr, true, m_coerceFrom);
        ostr << ")\n"
            << "{";
        writeVerbatim(ostr, m_code);
        ostr << "\n} // end of anonymous namespace\n";

        ostr << "\n"
            << classType->outerValueType()
            << " " << className << "::Coerce(";
        writeFunctionArg(ostr, false, m_coerceFrom);
        ostr << ")\n";

        ostr << "{\n";

        std::vector<AttributeConstSP> inputs(1, m_coerceFrom);
        writeInnerDeclarationsAndTranslations(ostr, inputs, "i_", true);

        std::string coerceFrom = spi::StringFormat("%s_Coerce(i_%s)",
            className.c_str(), m_coerceFrom->name().c_str());

        std::string translation = classType->translateInner(coerceFrom);

        ostr << "    return " << translation << ";\n";

        ostr << "}\n";
    }
}

bool CoerceFrom::implement(
    GeneratedOutput& ostr,
    const std::string& className,
    const ServiceDefinitionConstSP& svc,
    const std::vector<CoerceFromConstSP>& coerceFrom)
{
    // the intention is for coerce_from_object to become T::Coerce
    std::string T = "const " + className;
    std::string outer_type = className + "::outer_type";
    ostr << "\n"
         << outer_type << "\n"
         << className << "::Coerce(const spi::ObjectConstSP& o)\n"
         << "{\n"
         << "    // isNull\n"
         << "    if (!o)\n"
         << "        return " << outer_type << "();\n"
         << "\n"
         << "    // isInstance\n"
         << "    " << T << "* p = dynamic_cast<" << T << "*>(o.get());\n"
         << "    if (p)\n"
         << "        return " << outer_type << "(p);\n"
         << "\n";

    bool hasCoerceFromObject = false;
    std::vector<CoerceFromConstSP> coerceFromValue;
    int count = 0;
    for (size_t i = 0; i < coerceFrom.size(); ++i)
    {
        const CoerceFromConstSP& cf = coerceFrom[i];
        const DataTypeConstSP& dataType = cf->getDataType();
        int arrayDim = cf->arrayDim();
        spdoc::PublicType publicType = dataType->publicType();
        if (arrayDim > 0 || publicType != spdoc::PublicType::CLASS)
        {
            coerceFromValue.push_back(cf);
            continue; // we can only coerce from scalar objects here
        }

        if (!hasCoerceFromObject)
        {
            ostr << "    // coerceFrom\n";
            hasCoerceFromObject = true;
        }

        ++count;
        std::string cppName = dataType->cppName();
        ostr << "    const " << cppName << "* p" << count << " = dynamic_cast<const "
            << cppName << "*>(o.get());\n"
            << "    if (p" << count << ")\n"
            << "    {\n"
            << "        try {\n"
            << "            return " << className << "::Coerce("
            << cppName << "::outer_type(p" << count << "));\n"
            << "        } catch (...) {}\n"
            << "    }\n"
            << "\n";
    }

    ostr << "    spi::ObjectType* ot = &object_type;\n"
         << "    spi::ObjectConstSP o2;\n"
         << "\n"
         << "    // service coercion if the input object is from another service\n"
         << "    if (ot->get_service() != o->get_service())\n"
         << "    {\n"
         << "        o2 = o->service_coercion(ot->get_service());\n"
         << "        if (o2)\n"
         << "        {\n"
         << "            p = dynamic_cast<" << T << "*>(o2.get());\n"
         << "            if (p)\n"
         << "                return " << outer_type << "(p);\n"
         << "        }\n"
         << "        else\n"
         << "        {\n"
         << "            o2 = o;\n"
         << "        }\n"
         << "    }\n"
         << "    else\n"
         << "    {\n"
         << "        o2 = o;\n"
         << "    }\n"
         << "\n"
         << "    // coerceTo if the input object has a different name\n"
         << "    if (strcmp(ot->get_class_name(), o2->get_class_name()) != 0)\n"
         << "    {\n"
         << "        spi::ObjectConstSP o3 = o2->coerce_to_object(ot->get_class_name());\n"
         << "        if (o3)\n"
         << "        {\n"
         << "            p = dynamic_cast<" << T << "*>(o3.get());\n"
         << "            if (p)\n"
         << "                return " << outer_type << "(p);\n"
         << "        }\n"
         << "    }\n"
         << "\n"
         << "    SPI_THROW_RUNTIME_ERROR(o->get_class_name()\n"
         << "        << \" is neither \" << ot->get_class_name()\n"
         << "        << \" nor a sub-class of \" << ot->get_class_name());\n"
         << "}\n";

    if (coerceFromValue.size() == 0)
        return false;

    ostr << "\n"
         << "spi::ObjectConstSP " << className << "::coerce_from_value(\n"
         << "    const spi::Value&  value,\n"
         << "    const spi::InputContext* context)\n"
         << "{\n";

    DataTypeConstSP classType = svc->getDataType(className);

    for (size_t i = 0; i < coerceFromValue.size(); ++i)
    {
        const CoerceFromConstSP& cf = coerceFromValue[i];
        const DataTypeConstSP& dataType = cf->getDataType();
        std::string name = cf->getName();
        if (name == "value" || name == "context" || name == "o")
            name = "v";
        int arrayDim = cf->arrayDim();

        // the plan is that we will attempt to get the particular value
        // type from the input spi::Value using the spi::InputContext
        //
        // if we cannot get that value type, then we go onto the next
        // one
        //
        // these will all be done in try...catch blocks since we don't
        // want to throw an exception simply because the value type
        // doesn't match the input
        //
        // we cannot use DataType::fromValueCode since we are going to
        // be using the InputContext to translate
        //
        // this is because coercion is late binding - we might have
        // an XLOPER which we could choose to treat as either a date
        // or a string (for example) - this is the classic case where
        // at the spreadsheet level we represent a date as either an
        // actual date or as a string representing an offset from some
        // other date - however XLOPER doesn't know what it needs to
        // be so we have for the moment some Value object which might
        // be a double if it needs to be a Date

        const std::string& decl = arrayDim > 0 ?
            dataType->outerArrayType(arrayDim) :
            dataType->outerValueType();

        spdoc::PublicType publicType = dataType->publicType();
        std::ostringstream oss;

        switch(publicType)
        {
        case spdoc::PublicType::BOOL:
            oss << "context->ValueToBool";
            if (arrayDim == 1)
                oss << "Vector";
            else if (arrayDim == 2)
                oss << "Matrix";
            oss << "(value)";
            break;
        case spdoc::PublicType::CHAR:
            oss << "context->ValueToChar";
            if (arrayDim > 0)
                throw spi::RuntimeError("Cannot coerce from char array");
            oss << "(value)";
            break;
        case spdoc::PublicType::INT:
            oss << "context->ValueToInt";
            if (arrayDim == 1)
                oss << "Vector";
            else if (arrayDim == 2)
                oss << "Matrix";
            oss << "(value)";
            break;
        case spdoc::PublicType::DOUBLE:
            oss << "context->ValueToDouble";
            if (arrayDim == 1)
                oss << "Vector";
            else if (arrayDim == 2)
                oss << "Matrix";
            oss << "(value)";
            break;
        case spdoc::PublicType::STRING:
            oss << "context->ValueToString";
            if (arrayDim == 1)
                oss << "Vector";
            else if (arrayDim == 2)
                oss << "Matrix";
            oss << "(value)";
            break;
        case spdoc::PublicType::DATE:
            oss << "context->ValueToDate";
            if (arrayDim == 1)
                oss << "Vector";
            else if (arrayDim == 2)
                oss << "Matrix";
            oss << "(value)";
            break;
        case spdoc::PublicType::DATETIME:
            oss << "context->ValueToDateTime";
            if (arrayDim == 1)
                oss << "Vector";
            else if (arrayDim == 2)
                oss << "Matrix";
            oss << "(value)";
            break;
        case spdoc::PublicType::ENUM_AS_STRING:
        case spdoc::PublicType::ENUM_AS_INT:
            throw spi::RuntimeError("Cannot coerce from enumerated type");
        case spdoc::PublicType::CLASS:
            throw spi::RuntimeError("Cannot coerce from object in coerce_from_value");
            //if (arrayDim > 0)
            //    throw spi::RuntimeError("Cannot coerce from object array");
            //oss << "context->ValueToInstance<" << dataType->getCppName() << ">"
            //    << "(value)";
            //break;
        case spdoc::PublicType::VARIANT:
            oss << "context->ValueToVariant";
            if (arrayDim > 0)
                throw spi::RuntimeError("Cannot coerce from array of variants");
            oss << "(value)";
            break;
        default:
            break;
        }

        std::string contextCode = oss.str();
        if (contextCode.length() == 0)
            continue;

        ostr << "    try {\n"
             << "        " << decl << " " << name << " = " << contextCode
             << ";\n"
             << "        " << classType->outerValueType() << " o = "
             << className << "::Coerce(" << name << ");\n"
             << "        return spi::ObjectConstSP(o.get());\n"
             << "    } catch (...) {}\n"
             << "\n";
    }

    ostr << "    return spi::ObjectConstSP();\n"
         << "}\n";

    return true;
}

spdoc::CoerceFromConstSP CoerceFrom::getDoc() const
{
    return spdoc::CoerceFrom::Make(
        m_description,
        m_coerceFrom->getDoc());
}
