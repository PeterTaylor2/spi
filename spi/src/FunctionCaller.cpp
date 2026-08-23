#include "../FunctionCaller.hpp"

#include "../Variant.hpp"
#include "../Value.hpp"
#include "../InputContext.hpp"
#include "../Service.hpp"

SPI_BEGIN_NAMESPACE

spi::Value FuncArg::coerce(const spi::Variant& var) const
{
    // note that this function is only called when we are constructing
    // a Function via the functionMaker (which defines all of its inputs
    // to be of type Variant) or via the functionCaller (which allows
    // one of the inputs to be amended before the call)
    //
    // so we have the FuncArg which will gives its arrayDim (0,1 or 2)
    // and we will use that to allow a Variant from Excel input (which
    // will typically be a scalar or 2-dimensional array) to be collapsed
    // to a vector
    //
    // this was purely for aesthetic purposes so that the Function object
    // once created appears to have vectors rather than matrices
    const spi::Value& value = var.GetValue();
    const InputContext* context = var.GetInputContext();

    if (!context)
        return value;

    if (value.isUndefined())
        return value; // we need to call the function caller to provide defaults

    // likewise with the object type - we will convert an object if we can
    ObjectType* ot = nullptr;

    // we use the expected arrayDim to choose the correct context function
    // to use to get the value - note that the context->ValueTo... functions
    // return the concrete type which we convert back into Value by the
    // Value constructor

    try
    {
        switch (arrayDim)
        {
        case 2:
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
        case 1:
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
        case 0:
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
            break;
        default:
            SPI_THROW_RUNTIME_ERROR("Bad value for arrayDim (" << arrayDim
                << ") for field '" << name << "'");
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