#include "../FunctionCaller.hpp"

#include "../Variant.hpp"
#include "../Value.hpp"
#include "../InputContext.hpp"
#include "../Service.hpp"

SPI_BEGIN_NAMESPACE

spi::Value FuncArg::coerce(const spi::Variant& var) const
{
    const spi::Value& value = var.GetValue();
    const InputContext* context = var.GetInputContext();

    if (!context)
        return value;

    if (value.isUndefined())
        return value; // we need to call the function caller to provide defaults

    // likewise with the object type - we will convert an object if we can
    ObjectType* ot = nullptr;

    try
    {
        if (value.getType() == Value::ARRAY)
        {
            const IArrayConstSP& array = value.getArray();
            const std::vector<size_t> dims = array->dimensions();

            // optionality for an array is tricky - if we don't know the default
            // value then we can't really fill in except with zeroes which might
            // not be correct
            switch (dims.size())
            {
            case 1: // vector
                switch (argType)
                {
                case ArgType::BOOL:
                    return context->ValueToBoolVector(value, isOptional);
                case ArgType::CHAR:
                    SPI_THROW_RUNTIME_ERROR("Cannot convert to char vector");
                case ArgType::INT:
                    return context->ValueToIntVector(value, isOptional);
                case ArgType::DOUBLE:
                    return context->ValueToDoubleVector(value, isOptional);
                case ArgType::STRING:
                    return context->ValueToStringVector(value, isOptional);
                case ArgType::DATE:
                    return context->ValueToDateVector(value, isOptional);
                case ArgType::DATETIME:
                    return context->ValueToDateTimeVector(value, isOptional);
                case ArgType::ENUM:
                    return context->ValueToStringVector(value, isOptional);
                case ArgType::OBJECT:
                    return context->ValueToObjectVector(value, ot, isOptional);
                case ArgType::VARIANT:
                    return context->ValueToVariantVector(value, isOptional);
                }
                break;
            case 2: // matrix
                switch (argType)
                {
                case ArgType::BOOL:
                    return context->ValueToBoolMatrix(value, isOptional);
                case ArgType::CHAR:
                    SPI_THROW_RUNTIME_ERROR("Cannot convert to char matrix");
                case ArgType::INT:
                    return context->ValueToIntMatrix(value, isOptional);
                case ArgType::DOUBLE:
                    return context->ValueToDoubleMatrix(value, isOptional);
                case ArgType::STRING:
                    return context->ValueToStringMatrix(value, isOptional);
                case ArgType::DATE:
                    return context->ValueToDateMatrix(value, isOptional);
                case ArgType::DATETIME:
                    return context->ValueToDateTimeMatrix(value, isOptional);
                case ArgType::ENUM:
                    return context->ValueToStringMatrix(value, isOptional);
                case ArgType::OBJECT:
                    return context->ValueToObjectMatrix(value, ot, isOptional);
                case ArgType::VARIANT:
                    return context->ValueToVariantMatrix(value, isOptional);
                }
                break;
            default:
                SPI_THROW_RUNTIME_ERROR("Cannot convert array of dimension "
                    << dims.size());
            }
        }
        else
        {
            // for scalars we don't need to worry about isOptional
            // if the field is missing then we have already returned UNDEFINED
            switch (argType)
            {
            case ArgType::BOOL:
                return context->ValueToBool(value);
            case ArgType::CHAR:
                return context->ValueToChar(value);
            case ArgType::INT:
                return context->ValueToInt(value);
            case ArgType::DOUBLE:
                return context->ValueToDouble(value);
            case ArgType::STRING:
                return context->ValueToString(value);
            case ArgType::DATE:
                return context->ValueToDate(value);
            case ArgType::DATETIME:
                return context->ValueToDateTime(value);
            case ArgType::ENUM:
                return context->ValueToString(value);
            case ArgType::OBJECT:
                return context->ValueToObject(value, ot);
            case ArgType::VARIANT:
                return context->ValueToVariant(value);
            }
        }
    } 
    catch (...)
    {
        // in some cases failure to coerce can be caught later
        // for example if we have an object which can be coerced
        // from a string then the function caller should handle it
        return value;
    }

    return value;
}

SPI_END_NAMESPACE