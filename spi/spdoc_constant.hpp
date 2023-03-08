/*

    Sartorial Programming Interface (SPI) runtime libraries
    Copyright (C) 2012-2023 Sartorial Programming Ltd.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
    USA

*/

#ifndef _SPDOC_CONSTANT_HPP_
#define _SPDOC_CONSTANT_HPP_

/**
****************************************************************************
* Header file: spdoc_constant.hpp
****************************************************************************
*/

#include <spi/spi.hpp>
#include "spdoc_publicType.hpp"

SPDOC_BEGIN_NAMESPACE

SPI_DECLARE_OBJECT_CLASS(Constant);
SPI_DECLARE_OBJECT_CLASS(IntConstant);
SPI_DECLARE_OBJECT_CLASS(DateConstant);
SPI_DECLARE_OBJECT_CLASS(DateTimeConstant);
SPI_DECLARE_OBJECT_CLASS(DoubleConstant);
SPI_DECLARE_OBJECT_CLASS(CharConstant);
SPI_DECLARE_OBJECT_CLASS(StringConstant);
SPI_DECLARE_OBJECT_CLASS(BoolConstant);
SPI_DECLARE_OBJECT_CLASS(UndefinedConstant);

/**
****************************************************************************
* Interface class defining a constant scalar value.
****************************************************************************
*/
class SPI_IMPORT Constant : public spi::Object
{
public:

    /**
    ************************************************************************
    * returns the data type name for the scalar
    ************************************************************************
    */
    virtual std::string typeName() const = 0;

    /**
    ************************************************************************
    * returns the string which should appear in documentation
    ************************************************************************
    */
    virtual std::string docString() const = 0;

    /**
    ************************************************************************
    * returns the integer value (where applicable) for the scalar
    ************************************************************************
    */
    virtual int getInt() const;

    /**
    ************************************************************************
    * returns the date value (where applicable) for the scalar
    ************************************************************************
    */
    virtual spi::Date getDate() const;

    /**
    ************************************************************************
    * returns the date time value (where applicable) for the scalar
    ************************************************************************
    */
    virtual spi::DateTime getDateTime() const;

    /**
    ************************************************************************
    * returns the double value (where applicable) for the scalar
    ************************************************************************
    */
    virtual double getDouble() const;

    /**
    ************************************************************************
    * returns the char value (where applicable) for the scalar
    ************************************************************************
    */
    virtual char getChar() const;

    /**
    ************************************************************************
    * returns the string value (where applicable) for the scalar
    ************************************************************************
    */
    virtual std::string getString() const;

    /**
    ************************************************************************
    * returns the bool value (where applicable) for the scalar
    ************************************************************************
    */
    virtual bool getBool() const;

    typedef spi::ObjectSmartPtr<Constant> outer_type;

    SPI_DECLARE_OBJECT_TYPE(Constant);

protected:

    Constant();
};

/**
****************************************************************************
* Integer constant defined in the configuration file.
****************************************************************************
*/
class SPI_IMPORT IntConstant : public Constant
{
public:
    static IntConstantConstSP Make(
        int value);

    /**
    ************************************************************************
    * returns the string which should appear in documentation
    ************************************************************************
    */
    std::string docString() const;

    /**
    ************************************************************************
    * returns "int"
    ************************************************************************
    */
    std::string typeName() const;

    /**
    ************************************************************************
    * returns the integer value for the scalar
    ************************************************************************
    */
    int getInt() const;

    /**
    ************************************************************************
    * returns the double value for the scalar
    ************************************************************************
    */
    double getDouble() const;

    typedef spi::ObjectSmartPtr<IntConstant> outer_type;

    SPI_DECLARE_OBJECT_TYPE(IntConstant);

protected:

    IntConstant(
        int value);

public:

    const int value;
};

/**
****************************************************************************
* Date constant defined in the configuration file.
****************************************************************************
*/
class SPI_IMPORT DateConstant : public Constant
{
public:
    static DateConstantConstSP Make(
        spi::Date value);

    /**
    ************************************************************************
    * returns the string which should appear in documentation
    ************************************************************************
    */
    std::string docString() const;

    /**
    ************************************************************************
    * returns "date"
    ************************************************************************
    */
    std::string typeName() const;

    /**
    ************************************************************************
    * returns the date value for the scalar
    ************************************************************************
    */
    spi::Date getDate() const;

    typedef spi::ObjectSmartPtr<DateConstant> outer_type;

    SPI_DECLARE_OBJECT_TYPE(DateConstant);

protected:

    DateConstant(
        spi::Date value);

public:

    const spi::Date value;
};

/**
****************************************************************************
* DateTime constant defined in the configuration file.
****************************************************************************
*/
class SPI_IMPORT DateTimeConstant : public Constant
{
public:
    static DateTimeConstantConstSP Make(
        spi::DateTime value);

    /**
    ************************************************************************
    * returns the string which should appear in documentation
    ************************************************************************
    */
    std::string docString() const;

    /**
    ************************************************************************
    * returns "datetime"
    ************************************************************************
    */
    std::string typeName() const;

    /**
    ************************************************************************
    * returns the date value for the scalar
    ************************************************************************
    */
    spi::DateTime getDateTime() const;

    typedef spi::ObjectSmartPtr<DateTimeConstant> outer_type;

    SPI_DECLARE_OBJECT_TYPE(DateTimeConstant);

protected:

    DateTimeConstant(
        spi::DateTime value);

public:

    const spi::DateTime value;
};

/**
****************************************************************************
* Double constant defined in the configuration file.
****************************************************************************
*/
class SPI_IMPORT DoubleConstant : public Constant
{
public:
    static DoubleConstantConstSP Make(
        double value);

    /**
    ************************************************************************
    * returns the string which should appear in documentation
    ************************************************************************
    */
    std::string docString() const;

    /**
    ************************************************************************
    * returns "double"
    ************************************************************************
    */
    std::string typeName() const;

    /**
    ************************************************************************
    * returns the double value for the scalar
    ************************************************************************
    */
    double getDouble() const;

    typedef spi::ObjectSmartPtr<DoubleConstant> outer_type;

    SPI_DECLARE_OBJECT_TYPE(DoubleConstant);

protected:

    DoubleConstant(
        double value);

public:

    const double value;
};

/**
****************************************************************************
* Character constant defined in the configuration file.
****************************************************************************
*/
class SPI_IMPORT CharConstant : public Constant
{
public:
    static CharConstantConstSP Make(
        char value);

    /**
    ************************************************************************
    * returns the string which should appear in documentation
    ************************************************************************
    */
    std::string docString() const;

    /**
    ************************************************************************
    * returns "char"
    ************************************************************************
    */
    std::string typeName() const;

    /**
    ************************************************************************
    * returns the char value for the scalar
    ************************************************************************
    */
    char getChar() const;

    typedef spi::ObjectSmartPtr<CharConstant> outer_type;

    SPI_DECLARE_OBJECT_TYPE(CharConstant);

protected:

    CharConstant(
        char value);

public:

    const char value;
};

/**
****************************************************************************
* String constant defined in the configuration file.
****************************************************************************
*/
class SPI_IMPORT StringConstant : public Constant
{
public:
    static StringConstantConstSP Make(
        const std::string& value);

    /**
    ************************************************************************
    * returns the string which should appear in documentation
    ************************************************************************
    */
    std::string docString() const;

    /**
    ************************************************************************
    * returns "string"
    ************************************************************************
    */
    std::string typeName() const;

    /**
    ************************************************************************
    * returns the string value for the scalar
    ************************************************************************
    */
    std::string getString() const;

    typedef spi::ObjectSmartPtr<StringConstant> outer_type;

    SPI_DECLARE_OBJECT_TYPE(StringConstant);

protected:

    StringConstant(
        const std::string& value);

public:

    const std::string value;
};

/**
****************************************************************************
* Bool constant defined in the configuration file.
****************************************************************************
*/
class SPI_IMPORT BoolConstant : public Constant
{
public:
    static BoolConstantConstSP Make(
        bool value);

    /**
    ************************************************************************
    * returns the string which should appear in documentation
    ************************************************************************
    */
    std::string docString() const;

    /**
    ************************************************************************
    * returns "bool"
    ************************************************************************
    */
    std::string typeName() const;

    /**
    ************************************************************************
    * returns the bool value for the scalar
    ************************************************************************
    */
    bool getBool() const;

    typedef spi::ObjectSmartPtr<BoolConstant> outer_type;

    SPI_DECLARE_OBJECT_TYPE(BoolConstant);

protected:

    BoolConstant(
        bool value);

public:

    const bool value;
};

/**
****************************************************************************
* No description.
****************************************************************************
*/
class SPI_IMPORT UndefinedConstant : public Constant
{
public:
    static UndefinedConstantConstSP Make();

    /**
    ************************************************************************
    * returns the data type name for the scalar
    ************************************************************************
    */
    std::string typeName() const;

    /**
    ************************************************************************
    * returns the string which should appear in documentation
    ************************************************************************
    */
    std::string docString() const;

    /**
    ************************************************************************
    * returns the integer value (where applicable) for the scalar
    ************************************************************************
    */
    int getInt() const;

    /**
    ************************************************************************
    * returns the date value (where applicable) for the scalar
    ************************************************************************
    */
    spi::Date getDate() const;

    /**
    ************************************************************************
    * returns the date value (where applicable) for the scalar
    ************************************************************************
    */
    spi::DateTime getDateTime() const;

    /**
    ************************************************************************
    * returns the double value (where applicable) for the scalar
    ************************************************************************
    */
    double getDouble() const;

    /**
    ************************************************************************
    * returns the char value (where applicable) for the scalar
    ************************************************************************
    */
    char getChar() const;

    /**
    ************************************************************************
    * returns the string value (where applicable) for the scalar
    ************************************************************************
    */
    std::string getString() const;

    /**
    ************************************************************************
    * returns the bool value (where applicable) for the scalar
    ************************************************************************
    */
    bool getBool() const;

    typedef spi::ObjectSmartPtr<UndefinedConstant> outer_type;

    SPI_DECLARE_OBJECT_TYPE(UndefinedConstant);
};

SPDOC_END_NAMESPACE

#endif /* _SPDOC_CONSTANT_HPP_*/

