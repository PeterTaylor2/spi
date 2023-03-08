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

/**
****************************************************************************
* Source file: spdoc_constant.cpp
****************************************************************************
*/

#include "spdoc_constant.hpp"
#include "spdoc_constant_helper.hpp"
#include "spdoc_dll_service_manager.hpp"


SPDOC_BEGIN_NAMESPACE

/*
****************************************************************************
* Implementation of Constant
****************************************************************************
*/
Constant::Constant()
    : spi::Object(true)
{}

/*
****************************************************************************
* returns the integer value (where applicable) for the scalar
****************************************************************************
*/

int Constant::getInt() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.Constant.getInt");
  try
  {
    ConstantConstSP self(this);
    int i_result = Constant_Helper::getInt(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "Constant.getInt", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "Constant.getInt"); }
}

int Constant_Helper::getInt(
    const ConstantConstSP& in_self)
{
    const Constant* self = in_self.get();

    throw spi::RuntimeError("%s is not int",
                            self->typeName().c_str());
}

/*
****************************************************************************
* returns the date value (where applicable) for the scalar
****************************************************************************
*/

spi::Date Constant::getDate() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.Constant.getDate");
  try
  {
    ConstantConstSP self(this);
    spi::Date i_result = Constant_Helper::getDate(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "Constant.getDate", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "Constant.getDate"); }
}

spi::Date Constant_Helper::getDate(
    const ConstantConstSP& in_self)
{
    const Constant* self = in_self.get();

    throw spi::RuntimeError("%s is not date",
                            self->typeName().c_str());
}

/*
****************************************************************************
* returns the date time value (where applicable) for the scalar
****************************************************************************
*/

spi::DateTime Constant::getDateTime() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.Constant.getDateTime");
  try
  {
    ConstantConstSP self(this);
    spi::DateTime i_result = Constant_Helper::getDateTime(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "Constant.getDateTime", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "Constant.getDateTime"); }
}

spi::DateTime Constant_Helper::getDateTime(
    const ConstantConstSP& in_self)
{
    const Constant* self = in_self.get();

    throw spi::RuntimeError("%s is not datetime",
                            self->typeName().c_str());
}

/*
****************************************************************************
* returns the double value (where applicable) for the scalar
****************************************************************************
*/

double Constant::getDouble() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.Constant.getDouble");
  try
  {
    ConstantConstSP self(this);
    double i_result = Constant_Helper::getDouble(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "Constant.getDouble", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "Constant.getDouble"); }
}

double Constant_Helper::getDouble(
    const ConstantConstSP& in_self)
{
    const Constant* self = in_self.get();

    throw spi::RuntimeError("%s is not double",
                            self->typeName().c_str());
}

/*
****************************************************************************
* returns the char value (where applicable) for the scalar
****************************************************************************
*/

char Constant::getChar() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.Constant.getChar");
  try
  {
    ConstantConstSP self(this);
    char i_result = Constant_Helper::getChar(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "Constant.getChar", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "Constant.getChar"); }
}

char Constant_Helper::getChar(
    const ConstantConstSP& in_self)
{
    const Constant* self = in_self.get();

    throw spi::RuntimeError("%s is not char",
                            self->typeName().c_str());
}

/*
****************************************************************************
* returns the string value (where applicable) for the scalar
****************************************************************************
*/

std::string Constant::getString() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.Constant.getString");
  try
  {
    ConstantConstSP self(this);
    const std::string& i_result = Constant_Helper::getString(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "Constant.getString", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "Constant.getString"); }
}

std::string Constant_Helper::getString(
    const ConstantConstSP& in_self)
{
    const Constant* self = in_self.get();

    throw spi::RuntimeError("%s is not a string",
                            self->typeName().c_str());
}

/*
****************************************************************************
* returns the bool value (where applicable) for the scalar
****************************************************************************
*/

bool Constant::getBool() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.Constant.getBool");
  try
  {
    ConstantConstSP self(this);
    bool i_result = Constant_Helper::getBool(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "Constant.getBool", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "Constant.getBool"); }
}

bool Constant_Helper::getBool(
    const ConstantConstSP& in_self)
{
    const Constant* self = in_self.get();

    throw spi::RuntimeError("%s is not a bool",
                            self->typeName().c_str());
}

/*
****************************************************************************
* Implementation of IntConstant
****************************************************************************
*/
IntConstantConstSP IntConstant::Make(
    int value)
{
    spdoc_check_permission();
    return IntConstantConstSP(
        new IntConstant(value));
}

IntConstant::IntConstant(
    int value)
    :
    value(value)
{}

/*
****************************************************************************
* returns the string which should appear in documentation
****************************************************************************
*/

std::string IntConstant::docString() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.IntConstant.docString");
  try
  {
    IntConstantConstSP self(this);
    const std::string& i_result = IntConstant_Helper::docString(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "IntConstant.docString", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "IntConstant.docString"); }
}

std::string IntConstant_Helper::docString(
    const IntConstantConstSP& in_self)
{
    const IntConstant* self = in_self.get();

    return spi::StringFormat("%d", self->value);
}

/*
****************************************************************************
* returns "int"
****************************************************************************
*/

std::string IntConstant::typeName() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.IntConstant.typeName");
  try
  {
    IntConstantConstSP self(this);
    const std::string& i_result = IntConstant_Helper::typeName(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "IntConstant.typeName", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "IntConstant.typeName"); }
}

std::string IntConstant_Helper::typeName(
    const IntConstantConstSP& in_self)
{
    const IntConstant* self = in_self.get();

    return "int";
}

/*
****************************************************************************
* returns the integer value for the scalar
****************************************************************************
*/

int IntConstant::getInt() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.IntConstant.getInt");
  try
  {
    IntConstantConstSP self(this);
    int i_result = IntConstant_Helper::getInt(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "IntConstant.getInt", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "IntConstant.getInt"); }
}

int IntConstant_Helper::getInt(
    const IntConstantConstSP& in_self)
{
    const IntConstant* self = in_self.get();

    return self->value;
}

/*
****************************************************************************
* returns the double value for the scalar
****************************************************************************
*/

double IntConstant::getDouble() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.IntConstant.getDouble");
  try
  {
    IntConstantConstSP self(this);
    double i_result = IntConstant_Helper::getDouble(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "IntConstant.getDouble", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "IntConstant.getDouble"); }
}

double IntConstant_Helper::getDouble(
    const IntConstantConstSP& in_self)
{
    const IntConstant* self = in_self.get();

    return self->value;
}

/*
****************************************************************************
* Implementation of DateConstant
****************************************************************************
*/
DateConstantConstSP DateConstant::Make(
    spi::Date value)
{
    spdoc_check_permission();
    return DateConstantConstSP(
        new DateConstant(value));
}

DateConstant::DateConstant(
    spi::Date value)
    :
    value(value)
{}

/*
****************************************************************************
* returns the string which should appear in documentation
****************************************************************************
*/

std::string DateConstant::docString() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.DateConstant.docString");
  try
  {
    DateConstantConstSP self(this);
    const std::string& i_result = DateConstant_Helper::docString(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "DateConstant.docString", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "DateConstant.docString"); }
}

std::string DateConstant_Helper::docString(
    const DateConstantConstSP& in_self)
{
    const DateConstant* self = in_self.get();

    if ((int)(self->value) <= 0)
        return std::string();
    return std::string(self->value);
}

/*
****************************************************************************
* returns "date"
****************************************************************************
*/

std::string DateConstant::typeName() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.DateConstant.typeName");
  try
  {
    DateConstantConstSP self(this);
    const std::string& i_result = DateConstant_Helper::typeName(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "DateConstant.typeName", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "DateConstant.typeName"); }
}

std::string DateConstant_Helper::typeName(
    const DateConstantConstSP& in_self)
{
    const DateConstant* self = in_self.get();

    return "date";
}

/*
****************************************************************************
* returns the date value for the scalar
****************************************************************************
*/

spi::Date DateConstant::getDate() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.DateConstant.getDate");
  try
  {
    DateConstantConstSP self(this);
    spi::Date i_result = DateConstant_Helper::getDate(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "DateConstant.getDate", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "DateConstant.getDate"); }
}

spi::Date DateConstant_Helper::getDate(
    const DateConstantConstSP& in_self)
{
    const DateConstant* self = in_self.get();

    return self->value;
}

/*
****************************************************************************
* Implementation of DateTimeConstant
****************************************************************************
*/
DateTimeConstantConstSP DateTimeConstant::Make(
    spi::DateTime value)
{
    spdoc_check_permission();
    return DateTimeConstantConstSP(
        new DateTimeConstant(value));
}

DateTimeConstant::DateTimeConstant(
    spi::DateTime value)
    :
    value(value)
{}

/*
****************************************************************************
* returns the string which should appear in documentation
****************************************************************************
*/

std::string DateTimeConstant::docString() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.DateTimeConstant.docString");
  try
  {
    DateTimeConstantConstSP self(this);
    const std::string& i_result = DateTimeConstant_Helper::docString(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "DateTimeConstant.docString", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "DateTimeConstant.docString"); }
}

std::string DateTimeConstant_Helper::docString(
    const DateTimeConstantConstSP& in_self)
{
    const DateTimeConstant* self = in_self.get();

    return self->value.ToString();
}

/*
****************************************************************************
* returns "datetime"
****************************************************************************
*/

std::string DateTimeConstant::typeName() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.DateTimeConstant.typeName");
  try
  {
    DateTimeConstantConstSP self(this);
    const std::string& i_result = DateTimeConstant_Helper::typeName(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "DateTimeConstant.typeName", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "DateTimeConstant.typeName"); }
}

std::string DateTimeConstant_Helper::typeName(
    const DateTimeConstantConstSP& in_self)
{
    const DateTimeConstant* self = in_self.get();

    return "datetime";
}

/*
****************************************************************************
* returns the date value for the scalar
****************************************************************************
*/

spi::DateTime DateTimeConstant::getDateTime() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.DateTimeConstant.getDateTime");
  try
  {
    DateTimeConstantConstSP self(this);
    spi::DateTime i_result = DateTimeConstant_Helper::getDateTime(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "DateTimeConstant.getDateTime", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "DateTimeConstant.getDateTime"); }
}

spi::DateTime DateTimeConstant_Helper::getDateTime(
    const DateTimeConstantConstSP& in_self)
{
    const DateTimeConstant* self = in_self.get();

    return self->value;
}

/*
****************************************************************************
* Implementation of DoubleConstant
****************************************************************************
*/
DoubleConstantConstSP DoubleConstant::Make(
    double value)
{
    spdoc_check_permission();
    return DoubleConstantConstSP(
        new DoubleConstant(value));
}

DoubleConstant::DoubleConstant(
    double value)
    :
    value(value)
{}

/*
****************************************************************************
* returns the string which should appear in documentation
****************************************************************************
*/

std::string DoubleConstant::docString() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.DoubleConstant.docString");
  try
  {
    DoubleConstantConstSP self(this);
    const std::string& i_result = DoubleConstant_Helper::docString(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "DoubleConstant.docString", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "DoubleConstant.docString"); }
}

std::string DoubleConstant_Helper::docString(
    const DoubleConstantConstSP& in_self)
{
    const DoubleConstant* self = in_self.get();

    return spi::StringFormat("%.15g", self->value);
}

/*
****************************************************************************
* returns "double"
****************************************************************************
*/

std::string DoubleConstant::typeName() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.DoubleConstant.typeName");
  try
  {
    DoubleConstantConstSP self(this);
    const std::string& i_result = DoubleConstant_Helper::typeName(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "DoubleConstant.typeName", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "DoubleConstant.typeName"); }
}

std::string DoubleConstant_Helper::typeName(
    const DoubleConstantConstSP& in_self)
{
    const DoubleConstant* self = in_self.get();

    return "double";
}

/*
****************************************************************************
* returns the double value for the scalar
****************************************************************************
*/

double DoubleConstant::getDouble() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.DoubleConstant.getDouble");
  try
  {
    DoubleConstantConstSP self(this);
    double i_result = DoubleConstant_Helper::getDouble(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "DoubleConstant.getDouble", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "DoubleConstant.getDouble"); }
}

double DoubleConstant_Helper::getDouble(
    const DoubleConstantConstSP& in_self)
{
    const DoubleConstant* self = in_self.get();

    return self->value;
}

/*
****************************************************************************
* Implementation of CharConstant
****************************************************************************
*/
CharConstantConstSP CharConstant::Make(
    char value)
{
    spdoc_check_permission();
    return CharConstantConstSP(
        new CharConstant(value));
}

CharConstant::CharConstant(
    char value)
    :
    value(value)
{}

/*
****************************************************************************
* returns the string which should appear in documentation
****************************************************************************
*/

std::string CharConstant::docString() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.CharConstant.docString");
  try
  {
    CharConstantConstSP self(this);
    const std::string& i_result = CharConstant_Helper::docString(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "CharConstant.docString", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "CharConstant.docString"); }
}

std::string CharConstant_Helper::docString(
    const CharConstantConstSP& in_self)
{
    const CharConstant* self = in_self.get();

    return spi::StringFormat("'%c'", self->value);
}

/*
****************************************************************************
* returns "char"
****************************************************************************
*/

std::string CharConstant::typeName() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.CharConstant.typeName");
  try
  {
    CharConstantConstSP self(this);
    const std::string& i_result = CharConstant_Helper::typeName(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "CharConstant.typeName", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "CharConstant.typeName"); }
}

std::string CharConstant_Helper::typeName(
    const CharConstantConstSP& in_self)
{
    const CharConstant* self = in_self.get();

    return "char";
}

/*
****************************************************************************
* returns the char value for the scalar
****************************************************************************
*/

char CharConstant::getChar() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.CharConstant.getChar");
  try
  {
    CharConstantConstSP self(this);
    char i_result = CharConstant_Helper::getChar(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "CharConstant.getChar", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "CharConstant.getChar"); }
}

char CharConstant_Helper::getChar(
    const CharConstantConstSP& in_self)
{
    const CharConstant* self = in_self.get();

    return self->value;
}

/*
****************************************************************************
* Implementation of StringConstant
****************************************************************************
*/
StringConstantConstSP StringConstant::Make(
    const std::string& value)
{
    spdoc_check_permission();
    return StringConstantConstSP(
        new StringConstant(value));
}

StringConstant::StringConstant(
    const std::string& value)
    :
    value(value)
{}

/*
****************************************************************************
* returns the string which should appear in documentation
****************************************************************************
*/

std::string StringConstant::docString() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.StringConstant.docString");
  try
  {
    StringConstantConstSP self(this);
    const std::string& i_result = StringConstant_Helper::docString(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "StringConstant.docString", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "StringConstant.docString"); }
}

std::string StringConstant_Helper::docString(
    const StringConstantConstSP& in_self)
{
    const StringConstant* self = in_self.get();

    return spi::StringFormat("\"%s\"", self->value.c_str());
}

/*
****************************************************************************
* returns "string"
****************************************************************************
*/

std::string StringConstant::typeName() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.StringConstant.typeName");
  try
  {
    StringConstantConstSP self(this);
    const std::string& i_result = StringConstant_Helper::typeName(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "StringConstant.typeName", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "StringConstant.typeName"); }
}

std::string StringConstant_Helper::typeName(
    const StringConstantConstSP& in_self)
{
    const StringConstant* self = in_self.get();

    return "string";
}

/*
****************************************************************************
* returns the string value for the scalar
****************************************************************************
*/

std::string StringConstant::getString() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.StringConstant.getString");
  try
  {
    StringConstantConstSP self(this);
    const std::string& i_result = StringConstant_Helper::getString(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "StringConstant.getString", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "StringConstant.getString"); }
}

std::string StringConstant_Helper::getString(
    const StringConstantConstSP& in_self)
{
    const StringConstant* self = in_self.get();

    return self->value;
}

/*
****************************************************************************
* Implementation of BoolConstant
****************************************************************************
*/
BoolConstantConstSP BoolConstant::Make(
    bool value)
{
    spdoc_check_permission();
    return BoolConstantConstSP(
        new BoolConstant(value));
}

BoolConstant::BoolConstant(
    bool value)
    :
    value(value)
{}

/*
****************************************************************************
* returns the string which should appear in documentation
****************************************************************************
*/

std::string BoolConstant::docString() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.BoolConstant.docString");
  try
  {
    BoolConstantConstSP self(this);
    const std::string& i_result = BoolConstant_Helper::docString(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "BoolConstant.docString", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "BoolConstant.docString"); }
}

std::string BoolConstant_Helper::docString(
    const BoolConstantConstSP& in_self)
{
    const BoolConstant* self = in_self.get();

    return self->value ? "True" : "False";
}

/*
****************************************************************************
* returns "bool"
****************************************************************************
*/

std::string BoolConstant::typeName() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.BoolConstant.typeName");
  try
  {
    BoolConstantConstSP self(this);
    const std::string& i_result = BoolConstant_Helper::typeName(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "BoolConstant.typeName", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "BoolConstant.typeName"); }
}

std::string BoolConstant_Helper::typeName(
    const BoolConstantConstSP& in_self)
{
    const BoolConstant* self = in_self.get();

    return "bool";
}

/*
****************************************************************************
* returns the bool value for the scalar
****************************************************************************
*/

bool BoolConstant::getBool() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.BoolConstant.getBool");
  try
  {
    BoolConstantConstSP self(this);
    bool i_result = BoolConstant_Helper::getBool(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "BoolConstant.getBool", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "BoolConstant.getBool"); }
}

bool BoolConstant_Helper::getBool(
    const BoolConstantConstSP& in_self)
{
    const BoolConstant* self = in_self.get();

    return self->value;
}

/*
****************************************************************************
* Implementation of UndefinedConstant
****************************************************************************
*/
UndefinedConstantConstSP UndefinedConstant::Make()
{
    spdoc_check_permission();
    return UndefinedConstantConstSP(
        new UndefinedConstant);
}

/*
****************************************************************************
* returns the data type name for the scalar
****************************************************************************
*/

std::string UndefinedConstant::typeName() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.UndefinedConstant.typeName");
  try
  {
    UndefinedConstantConstSP self(this);
    const std::string& i_result = UndefinedConstant_Helper::typeName(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "UndefinedConstant.typeName", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "UndefinedConstant.typeName"); }
}

std::string UndefinedConstant_Helper::typeName(
    const UndefinedConstantConstSP& in_self)
{
    const UndefinedConstant* self = in_self.get();

    return std::string();
}

/*
****************************************************************************
* returns the string which should appear in documentation
****************************************************************************
*/

std::string UndefinedConstant::docString() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.UndefinedConstant.docString");
  try
  {
    UndefinedConstantConstSP self(this);
    const std::string& i_result = UndefinedConstant_Helper::docString(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "UndefinedConstant.docString", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "UndefinedConstant.docString"); }
}

std::string UndefinedConstant_Helper::docString(
    const UndefinedConstantConstSP& in_self)
{
    const UndefinedConstant* self = in_self.get();

    return std::string();
}

/*
****************************************************************************
* returns the integer value (where applicable) for the scalar
****************************************************************************
*/

int UndefinedConstant::getInt() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.UndefinedConstant.getInt");
  try
  {
    UndefinedConstantConstSP self(this);
    int i_result = UndefinedConstant_Helper::getInt(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "UndefinedConstant.getInt", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "UndefinedConstant.getInt"); }
}

int UndefinedConstant_Helper::getInt(
    const UndefinedConstantConstSP& in_self)
{
    const UndefinedConstant* self = in_self.get();

    return 0;
}

/*
****************************************************************************
* returns the date value (where applicable) for the scalar
****************************************************************************
*/

spi::Date UndefinedConstant::getDate() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.UndefinedConstant.getDate");
  try
  {
    UndefinedConstantConstSP self(this);
    spi::Date i_result = UndefinedConstant_Helper::getDate(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "UndefinedConstant.getDate", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "UndefinedConstant.getDate"); }
}

spi::Date UndefinedConstant_Helper::getDate(
    const UndefinedConstantConstSP& in_self)
{
    const UndefinedConstant* self = in_self.get();

    return spi::Date();
}

/*
****************************************************************************
* returns the date value (where applicable) for the scalar
****************************************************************************
*/

spi::DateTime UndefinedConstant::getDateTime() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.UndefinedConstant.getDateTime");
  try
  {
    UndefinedConstantConstSP self(this);
    spi::DateTime i_result = UndefinedConstant_Helper::getDateTime(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "UndefinedConstant.getDateTime", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "UndefinedConstant.getDateTime"); }
}

spi::DateTime UndefinedConstant_Helper::getDateTime(
    const UndefinedConstantConstSP& in_self)
{
    const UndefinedConstant* self = in_self.get();

    return spi::DateTime();
}

/*
****************************************************************************
* returns the double value (where applicable) for the scalar
****************************************************************************
*/

double UndefinedConstant::getDouble() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.UndefinedConstant.getDouble");
  try
  {
    UndefinedConstantConstSP self(this);
    double i_result = UndefinedConstant_Helper::getDouble(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "UndefinedConstant.getDouble", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "UndefinedConstant.getDouble"); }
}

double UndefinedConstant_Helper::getDouble(
    const UndefinedConstantConstSP& in_self)
{
    const UndefinedConstant* self = in_self.get();

    return 0.0;
}

/*
****************************************************************************
* returns the char value (where applicable) for the scalar
****************************************************************************
*/

char UndefinedConstant::getChar() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.UndefinedConstant.getChar");
  try
  {
    UndefinedConstantConstSP self(this);
    char i_result = UndefinedConstant_Helper::getChar(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "UndefinedConstant.getChar", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "UndefinedConstant.getChar"); }
}

char UndefinedConstant_Helper::getChar(
    const UndefinedConstantConstSP& in_self)
{
    const UndefinedConstant* self = in_self.get();

    throw spi::RuntimeError("Undefined char not supported");
}

/*
****************************************************************************
* returns the string value (where applicable) for the scalar
****************************************************************************
*/

std::string UndefinedConstant::getString() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.UndefinedConstant.getString");
  try
  {
    UndefinedConstantConstSP self(this);
    const std::string& i_result = UndefinedConstant_Helper::getString(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "UndefinedConstant.getString", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "UndefinedConstant.getString"); }
}

std::string UndefinedConstant_Helper::getString(
    const UndefinedConstantConstSP& in_self)
{
    const UndefinedConstant* self = in_self.get();

    return std::string();
}

/*
****************************************************************************
* returns the bool value (where applicable) for the scalar
****************************************************************************
*/

bool UndefinedConstant::getBool() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.UndefinedConstant.getBool");
  try
  {
    UndefinedConstantConstSP self(this);
    bool i_result = UndefinedConstant_Helper::getBool(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "UndefinedConstant.getBool", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "UndefinedConstant.getBool"); }
}

bool UndefinedConstant_Helper::getBool(
    const UndefinedConstantConstSP& in_self)
{
    const UndefinedConstant* self = in_self.get();

    return false;
}

SPDOC_END_NAMESPACE

