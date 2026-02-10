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
/*
***************************************************************************
** xlService.cpp
**
** Defines the Excel service class. This is linked to the Service class
** for the underlying C++ outer library.
***************************************************************************
*/

#include "xlService.hpp"
#include "xlUtil.hpp"
#include "xlFuncWizard.hpp"
#include "xlInput.hpp"
#include "xlOutput.hpp"
#include "xlValue.hpp"

#include <time.h>

#include <spi/StringUtil.hpp>
#include <spi/RuntimeError.hpp>
#include <spi_util/FileUtil.hpp>
#include <spi/spdoc_configTypes.hpp>
#include <spi/spdoc_dll_service.hpp>

#undef NEW
#define NEW(T) (T*)calloc(sizeof(T),1)

#include "framewrk.h"

SPI_BEGIN_NAMESPACE

namespace 
{

std::string PrefixUpper(const std::string& name, const char* sep)
{
    std::vector<std::string> parts = StringSplit(name, sep);
    parts[0] = StringUpper(parts[0]);
    return StringJoin(sep, parts);
}

bool g_errorPopups = false;

}

ExcelService::ExcelService(
    const ServiceSP& service,
    const std::string& xllName,
    const char* sep,
    bool upperCase,
    bool optionalBaseName,
    bool errNA,
    bool nsUpperCase)
    :
    m_service(service),
    m_registeredFunctions(),
    m_dirname(spi_util::path::dirname(xllName)),
    m_doc(),
    m_inputContext(InputContext::ExcelContext()),
    m_sep(sep),
    m_upperCase(upperCase),
    m_optionalBaseName(optionalBaseName),
    m_errNA(errNA),
    m_nsUpperCase(nsUpperCase),
    m_timing(false),
    m_timings()
{
    // m_service->set_startup_directory(m_dirname);
    m_service->implement_read_cache(true);
    m_service->add_client("EXCEL");
}

/*
 * Helper functions
 */
static XLOPER* TempString(const std::string& str);
static XLOPER* TempHelp(const std::vector<std::string>& argsHelp, size_t i);
static XLOPER* GetXLL();
static void FreeXLL(XLOPER*);

#if SPI_XL_VERSION >= 12
static XLOPER12* TempString12(const std::string& str);
#endif

std::string GetXLLName()
{
    XLOPER* xll = GetXLL();
    if (!xll)
        throw RuntimeError("Could not get XLL name");

    try
    {
        std::string xllName = xloperToString(xll, "XLL");
        FreeXLL(xll);
        return xllName;
    }
    catch (...)
    {
        FreeXLL(xll);
        throw;
    }
}

static XLOPER* GetXLL()
{
    int     xlret;
    XLOPER* xll = NEW(XLOPER);

    if (!xll)
        return 0;

    xlret = Excel(xlGetName, xll, 0);
    if (xlret != xlretSuccess)
    {
        Excel(xlcAlert, 0, 2,
            TempStrConst("Excel get DLL name function failed"),
            TempInt(2));
        free(xll);
        return 0;
    }

#ifdef XL_AUTO_OPEN_ALERT
    {
        std::string xllName = xloperToString(xll, "XLL");
        std::string buf = "Loading " + xllName;
        Excel(xlcAlert, 0, 2, TempStrConst(buf.c_str()), TempInt(2));
    }
#endif

    return xll;
}

static void FreeXLL(XLOPER* xll)
{
    if (xll)
    {
        Excel(xlFree, 0, 1, xll);
        free(xll);
    }
}

void StartupError(const std::string& xllName, const char* errmsg)
{
    std::ostringstream oss;
    oss << "Error when loading " << xllName << "\n";
    oss << errmsg;

    Excel(xlcAlert, 0, 2, TempString(oss.str()), TempInt(3));
}

InputContext* ExcelService::getInputContext()
{
    return m_inputContext;
}

/**
 * Registers the standard functions that are provided for all libraries.
 */
void ExcelService::RegisterStandardFunctions(const std::string& xll,
    const std::string& helpFunc,
    const std::string& helpFuncList,
    const std::string& helpEnum, 
    const std::string& objectCoerce, 
    const std::string& startLogging, 
    const std::string& stopLogging, 
    const std::string& startTiming, 
    const std::string& stopTiming, 
    const std::string& clearTimings, 
    const std::string& getTimings, 
    const std::string& setErrorPopups,
    const std::string& objectToString, 
    const std::string& objectFromString, 
    const std::string& objectGet, 
    const std::string& objectPut,
    const std::string& objectPutMetaData,
    const std::string& objectToFile, 
    const std::string& objectFromFile, 
    const std::string& objectCount, 
    const std::string& objectFree, 
    const std::string& objectFreeAll, 
    const std::string& objectList, 
    const std::string& objectClassName, 
    const std::string& objectSHA) 
{
    std::vector<std::string> args;
    std::vector<std::string> help;

    std::string ns = m_service->get_namespace(); 
    std::string nsReg = (m_upperCase || m_nsUpperCase) ? StringUpper(ns) : ns;

    if (!helpFunc.empty())
    {
        std::string regFunc = m_upperCase ? StringUpper(helpFunc) : helpFunc;
        args.clear();
        help.clear();
        args.push_back("funcName?");
        help.push_back("Function name, or leave blank to select all functions");
        RegisterFunction(
            xll,
            StringFormat("xl_%s_help_func", ns.c_str()),
            StringFormat("%s%s%s", nsReg.c_str(), m_sep, regFunc.c_str()),
            nsReg,
            args,
            "Returns all function names or details about a particular function",
            help);
    }

    if (!helpFuncList.empty())
    {
        std::string regFunc = m_upperCase ? StringUpper(helpFuncList) : helpFuncList;
        args.clear();
        help.clear();
        RegisterFunction(
            xll,
            StringFormat("xl_%s_help_func_list", ns.c_str()),
            StringFormat("%s%s%s", nsReg.c_str(), m_sep, regFunc.c_str()),
            nsReg,
            args,
            "Lists all the functions in the library",
            help);
    }

    if (!helpEnum.empty())
    {
        std::string regFunc = m_upperCase ? StringUpper(helpEnum) : helpEnum;
        args.clear();
        help.clear();
        args.push_back("enumName?");
        help.push_back("Enumerated type name, or leave blank to select all enumerated types in the library.");
        RegisterFunction(xll,
            StringFormat("xl_%s_help_enum", ns.c_str()),
            StringFormat("%s%s%s", nsReg.c_str(), m_sep, regFunc.c_str()),
            nsReg,
            args,
            "Find enumerated types or enumerands",
            help);
    }

    if (!objectToString.empty())
    {
        std::string regFunc = m_upperCase ? StringUpper(objectToString) : objectToString;
        args.clear();
        help.clear();
        args.push_back("handle");
        help.push_back("Object handle string");
        args.push_back("format?");
        help.push_back("Object format");
        args.push_back("options?");
        help.push_back("Formatting options - format specific");
        args.push_back("metaData?");
        help.push_back("meta data for the object");
        args.push_back("mergeMetaData?");
        help.push_back("whether to merge the existing meta data when creating the string");
        RegisterFunction(
            xll,
            StringFormat("xl_%s_object_to_string", ns.c_str()),
            StringFormat("%s%s%s", nsReg.c_str(), m_sep, regFunc.c_str()),
            nsReg,
            args,
            "Returns the text representation of an object",
            help);
    }

    if (!objectFromString.empty())
    {
        std::string regFunc = m_upperCase ? StringUpper(objectFromString) : objectFromString;
        args.clear();
        help.clear();
        args.push_back("strings[]");
        help.push_back("Text representation of an object");
        args.push_back("baseName?");
        help.push_back("Base name of object handle string");
        RegisterFunction(
            xll,
            StringFormat("xl_%s_object_from_string", ns.c_str()),
            StringFormat("%s%s%s", nsReg.c_str(), m_sep, regFunc.c_str()),
            nsReg,
            args,
            "Creates an object from the text representation of an object",
            help);
    }

    if (!objectGet.empty())
    {
        std::string regFunc = m_upperCase ? StringUpper(objectGet) : objectGet;
        args.clear();
        help.clear();
        args.push_back("handle");
        help.push_back("Object handle string");
        args.push_back("name");
        help.push_back("Field name - leave blank to see all field names");
        RegisterFunction(
            xll,
            StringFormat("xl_%s_object_get", ns.c_str()),
            StringFormat("%s%s%s", nsReg.c_str(), m_sep, regFunc.c_str()),
            nsReg,
            args,
            "Get individual fields from an object (or the list of all fields)",
            help);
    }

    if (!objectPut.empty())
    {
        std::string regFunc = m_upperCase ? StringUpper(objectPut) : objectPut;
        args.clear();
        help.clear();
        args.push_back("handle?");
        help.push_back("Object handle string");
        args.push_back("baseName?");
        help.push_back("Base name of object handle string");
        args.push_back("names[]");
        help.push_back("List of field names to be added - can include type specification.");
        args.push_back("v1?");
        help.push_back("Value to be added for corresponding name");
        args.push_back("v2?");
        args.push_back("v3?");
        args.push_back("v4?");
        args.push_back("v5?");
        args.push_back("v6?");
        args.push_back("v7?");
        args.push_back("v8?");
        args.push_back("v9?");
        args.push_back("v10?");
        args.push_back("v11?");
        args.push_back("v12?");
        args.push_back("v13?");
        args.push_back("v14?");
        args.push_back("v15?");
        args.push_back("v16?");
        args.push_back("v17?");
        args.push_back("v18?");
        args.push_back("v19?");
        args.push_back("v20?");
        args.push_back("v21?");
        args.push_back("v22?");
        args.push_back("v23?");
        args.push_back("v24?");
        args.push_back("v25?");
        RegisterFunction(
            xll,
            StringFormat("xl_%s_object_put", ns.c_str()),
            StringFormat("%s%s%s", nsReg.c_str(), m_sep, regFunc.c_str()),
            nsReg,
            args,
            "Puts fields into an object returning a new object",
            help);
    }

    if (!objectPutMetaData.empty())
    {
        std::string regFunc = m_upperCase ? StringUpper(objectPutMetaData) : objectPutMetaData;
        args.clear();
        help.clear();
        args.push_back("handle?");
        help.push_back("Object handle string");
        args.push_back("baseName?");
        help.push_back("Base name of object handle string");
        args.push_back("names[]");
        help.push_back("List of field names to be added to meta data - can include type specification.");
        args.push_back("v1?");
        help.push_back("Value to be added for corresponding name");
        args.push_back("v2?");
        args.push_back("v3?");
        args.push_back("v4?");
        args.push_back("v5?");
        args.push_back("v6?");
        args.push_back("v7?");
        args.push_back("v8?");
        args.push_back("v9?");
        args.push_back("v10?");
        args.push_back("v11?");
        args.push_back("v12?");
        args.push_back("v13?");
        args.push_back("v14?");
        args.push_back("v15?");
        args.push_back("v16?");
        args.push_back("v17?");
        args.push_back("v18?");
        args.push_back("v19?");
        args.push_back("v20?");
        args.push_back("v21?");
        args.push_back("v22?");
        args.push_back("v23?");
        args.push_back("v24?");
        args.push_back("v25?");
        RegisterFunction(
            xll,
            StringFormat("xl_%s_object_put_meta_data", ns.c_str()),
            StringFormat("%s%s%s", nsReg.c_str(), m_sep, regFunc.c_str()),
            nsReg,
            args,
            "Updates the meta_data of an object",
            help);
    }

    if (!objectToFile.empty())
    {
        std::string regFunc = m_upperCase ? StringUpper(objectToFile) : objectToFile;
        args.clear();
        help.clear();
        args.push_back("handle");
        help.push_back("Object handle string");
        args.push_back("fileName");
        help.push_back("Name of file where we will save the object");
        args.push_back("format?");
        help.push_back("Object format");
        args.push_back("options?");
        help.push_back("Formatting options - format specific");
        args.push_back("metaData?");
        help.push_back("meta data for the object");
        args.push_back("mergeMetaData?");
        help.push_back("whether to merge the existing meta data when writing to file");
        RegisterFunction(
            xll,
            StringFormat("xl_%s_object_to_file", ns.c_str()),
            StringFormat("%s%s%s", nsReg.c_str(), m_sep, regFunc.c_str()),
            nsReg,
            args,
            "Saves an object to file",
            help);
    }

    if (!objectFromFile.empty())
    {
        std::string regFunc = m_upperCase ? StringUpper(objectFromFile) : objectFromFile;
        args.clear();
        help.clear();
        args.push_back("fileName");
        help.push_back("Name of file from where we will read the object");
        args.push_back("baseName?");
        help.push_back("Base name of object handle string");
        RegisterFunction(
            xll,
            StringFormat("xl_%s_object_from_file", ns.c_str()),
            StringFormat("%s%s%s", nsReg.c_str(), m_sep, regFunc.c_str()),
            nsReg,
            args,
            "Reads an object to file",
            help);
    }

    if (!objectCoerce.empty())
    {
        std::string regFunc = m_upperCase ? StringUpper(objectCoerce) : objectCoerce;
        args.clear();
        help.clear();
        args.push_back("className");
        help.push_back("Name of required class");
        args.push_back("value");
        help.push_back("Arbitrary value to be coerced to the given class");
        args.push_back("baseName");
        help.push_back("Base name of object handle string");
        RegisterFunction(
            xll,
            StringFormat("xl_%s_object_coerce", ns.c_str()),
            StringFormat("%s%s%s", nsReg.c_str(), m_sep, regFunc.c_str()),
            nsReg,
            args,
            "Coerces an object from an arbitrary value",
            help);
    }

    if (!objectCount.empty())
    {
        std::string regFunc = m_upperCase ? StringUpper(objectCount) : objectCount;
        args.clear();
        help.clear();
        args.push_back("className?");
        help.push_back("Class name (optional)");
        RegisterFunction(
            xll,
            StringFormat("xl_%s_object_count", ns.c_str()),
            StringFormat("%s%s%s", nsReg.c_str(), m_sep, regFunc.c_str()),
            nsReg,
            args,
            "Counts the objects in the object manager",
            help,
            false, true); // volatile
    }

    if (!objectFree.empty())
    {
        std::string regFunc = m_upperCase ? StringUpper(objectFree) : objectFree;
        args.clear();
        help.clear();
        args.push_back("handle");
        help.push_back("Handle of the object to be deleted");
        RegisterFunction(
            xll,
            StringFormat("xl_%s_object_free", ns.c_str()),
            StringFormat("%s%s%s", nsReg.c_str(), m_sep, regFunc.c_str()),
            nsReg,
            args,
            "Frees an object given its handle",
            help,
            true, false); // hidden
    }

    if (!objectFreeAll.empty())
    {
        std::string regFunc = m_upperCase ? StringUpper(objectFreeAll) : objectFreeAll;
        args.clear();
        help.clear();
        RegisterFunction(
            xll,
            StringFormat("xl_%s_object_free_all", ns.c_str()),
            StringFormat("%s%s%s", nsReg.c_str(), m_sep, regFunc.c_str()),
            nsReg,
            args,
            "Frees all objects",
            help,
            true, false); // hidden
    }

    if (!objectList.empty())
    {
        std::string regFunc = m_upperCase ? StringUpper(objectList) : objectList;
        args.clear();
        help.clear();
        args.push_back("baseName?");
        help.push_back("Base name of objects selected (optional)");
        args.push_back("className?");
        help.push_back("Class name of objects selected (optional)");
        RegisterFunction(
            xll,
            StringFormat("xl_%s_object_list", ns.c_str()),
            StringFormat("%s%s%s", nsReg.c_str(), m_sep, regFunc.c_str()),
            nsReg,
            args,
            "Lists objects in the object manager",
            help,
            false, true); // volatile
    }

    if (!objectClassName.empty())
    {
        std::string regFunc = m_upperCase ? StringUpper(objectClassName) : objectClassName;
        args.clear();
        help.clear();
        args.push_back("handle");
        help.push_back("Object handle");
        RegisterFunction(
            xll,
            StringFormat("xl_%s_object_class_name", ns.c_str()),
            StringFormat("%s%s%s", nsReg.c_str(), m_sep, regFunc.c_str()),
            nsReg,
            args,
            "Returns the class name for an object handle",
            help);
    }

    if (!objectSHA.empty())
    {
        std::string regFunc = m_upperCase ? StringUpper(objectSHA) : objectSHA;
        args.clear();
        help.clear();
        args.push_back("handle");
        help.push_back("Object handle");
        args.push_back("version?");
        help.push_back("1 (default) or 256");
        RegisterFunction(
            xll,
            StringFormat("xl_%s_object_sha", ns.c_str()),
            StringFormat("%s%s%s", nsReg.c_str(), m_sep, regFunc.c_str()),
            nsReg,
            args,
            "Returns the SHA hash value (as string) for an object",
            help);
    }

    if (!startLogging.empty())
    {
        std::string regFunc = m_upperCase ? StringUpper(startLogging) : startLogging;
        args.clear();
        help.clear();
        args.push_back("filename");
        help.push_back("Name of file which will contain the function log");
        args.push_back("options");
        help.push_back("");
        RegisterFunction(
            xll,
            StringFormat("xl_%s_start_logging", ns.c_str()),
            StringFormat("%s%s%s", nsReg.c_str(), m_sep, regFunc.c_str()),
            nsReg,
            args,
            "Start function logging - returns filename on success",
            help);
    }

    if (!stopLogging.empty())
    {
        std::string regFunc = m_upperCase ? StringUpper(stopLogging) : stopLogging;
        args.clear();
        help.clear();
        RegisterFunction(
            xll,
            StringFormat("xl_%s_stop_logging", ns.c_str()),
            StringFormat("%s%s%s", nsReg.c_str(), m_sep, regFunc.c_str()),
            nsReg,
            args,
            "Stops function logging - returns whether logging was on previously",
            help);
    }

    if (!startTiming.empty())
    {
        std::string regFunc = m_upperCase ? StringUpper(startTiming) : startTiming;
        args.clear();
        help.clear();
        RegisterFunction(
            xll,
            StringFormat("xl_%s_start_timing", ns.c_str()),
            StringFormat("%s%s%s", nsReg.c_str(), m_sep, regFunc.c_str()),
            nsReg,
            args,
            "Starts timing function calls",
            help);
    }

    if (!stopTiming.empty())
    {
        std::string regFunc = m_upperCase ? StringUpper(stopTiming) : stopTiming;
        args.clear();
        help.clear();
        RegisterFunction(
            xll,
            StringFormat("xl_%s_stop_timing", ns.c_str()),
            StringFormat("%s%s%s", nsReg.c_str(), m_sep, regFunc.c_str()),
            nsReg,
            args,
            "Stops timing function calls",
            help);
    }

    if (!clearTimings.empty())
    {
        std::string regFunc = m_upperCase ? StringUpper(clearTimings) : clearTimings;
        args.clear();
        help.clear();
        RegisterFunction(
            xll,
            StringFormat("xl_%s_clear_timings", ns.c_str()),
            StringFormat("%s%s%s", nsReg.c_str(), m_sep, regFunc.c_str()),
            nsReg,
            args,
            "Clears the existing results of timing function calls",
            help);
    }

    if (!getTimings.empty())
    {
        std::string regFunc = m_upperCase ? StringUpper(getTimings) : getTimings;
        args.clear();
        help.clear();
        RegisterFunction(
            xll,
            StringFormat("xl_%s_get_timings", ns.c_str()),
            StringFormat("%s%s%s", nsReg.c_str(), m_sep, regFunc.c_str()),
            nsReg,
            args,
            "Gets the results of timing function calls - volatile function",
            help,
            false, true); // volatile
    }

    if (!setErrorPopups.empty())
    {
        std::string regFunc = m_upperCase ? StringUpper(setErrorPopups) : setErrorPopups;
        args.clear();
        help.clear();
        args.push_back("errorPopups");
        help.push_back("Set to true to turn error popups on, set to false to turn error popups off.");
        RegisterFunction(
            xll,
            StringFormat("xl_%s_set_error_popups", ns.c_str()),
            StringFormat("%s%s%s", nsReg.c_str(), m_sep, regFunc.c_str()),
            nsReg,
            args,
            "Set flag to determine whether error messages pop-up in a window or not.",
            help);
    }
}

void ExcelService::RegisterFunction(
    const std::string&              xllName,
    const std::string&              cFuncName,
    const std::string&              xlFuncName,
    const std::string&              xlCatName,
    const std::vector<std::string>& args,
    const std::string&              funcHelp,
    const std::vector<std::string>& argsHelp,
    bool hiddenFromWizard,
    bool volatileInExcel,
    const char* xlArgTypes)
{
    try
    {
        bool failed = false;
        int xlFailureCode = 0;
#if SPI_XL_VERSION == 4
        size_t numArgs;
        size_t maxArgs = 31;
        int    maxLenArgNames = 254;
        const size_t maxHelp = 19;
        const char* maxHelpMsg = "20 or more inputs - no help available. ";

        // there is a case for us concatenating something from the service
        // with the function name at this point - it would save us from
        // having to do it outside

        maxLenArgNames -= (xlFuncName.length() + 2);
        numArgs = args.size() + 1; // inputs and outputs
        if (numArgs > maxArgs)
        {
            throw RuntimeError("Too many arguments (%d) to Excel function %s",
                               numArgs, xlFuncName.c_str());
        }
        std::string argTypes(numArgs, 'P');
        if (xlArgTypes && strlen(xlArgTypes) == numArgs)
            argTypes = std::string(xlArgTypes);

        std::string argNames = StringJoin(",", args);
        if ((int)argNames.length() > maxLenArgNames)
        {
            int oversize = (int)argNames.length() - maxLenArgNames;

            std::vector<std::string> shortArgs(args);
            for (size_t i = shortArgs.size(); oversize > 0 && i > 0; --i)
            {
                const std::string& arg = args[i-1];
                std::string shortArg;

                if (spi::StringEndsWith(arg, "?"))
                    shortArg = spi::StringFormat("arg%d?", (int)i);
                else
                    shortArg = spi::StringFormat("arg%d", (int)i);

                shortArgs[i-1] = shortArg;
                oversize += shortArg.length();
                oversize -= arg.length();
            }

            argNames = StringJoin(",", shortArgs);
            SPI_POST_CONDITION((int)argNames.length() <= maxLenArgNames);
        }

        FreeAllTempMemory();

        std::vector<XLOPER*> xInputs;

        if (volatileInExcel)
            argTypes += "!";

        xInputs.push_back(TempString(xllName));
        xInputs.push_back(TempString(cFuncName));
        xInputs.push_back(TempString(argTypes));
        if (m_upperCase)
        {
            xInputs.push_back(TempString(PrefixUpper(xlFuncName, m_sep)));
        }
        else
        {
            xInputs.push_back(TempString(xlFuncName));
        }
        xInputs.push_back(TempString(argNames));
        if (hiddenFromWizard)
            xInputs.push_back(TempNum(0));
        else
            xInputs.push_back(TempNum(1));
        xInputs.push_back(TempString(xlCatName));
        xInputs.push_back(TempStrConst(""));
        xInputs.push_back(TempStrConst(""));
        xInputs.push_back(TempString(funcHelp));
        for (size_t i = 0; i < argsHelp.size() && i < maxHelp; ++i)
        {
            std::string argHelp;
            if (argsHelp[i].empty() && i < args.size())
            {
                argHelp = args[i];
                if (StringEndsWith(argHelp, "?"))
                    argHelp = argHelp.substr(0, argHelp.length()-1) + " (optional)";
            }
            else
            {
                argHelp = argsHelp[i];
            }
            if (i+1 == argsHelp.size())
                argHelp = argHelp + ". ";

            xInputs.push_back(TempString(argHelp));
        }

        if (argsHelp.size() > maxHelp)
        {
            xInputs.push_back(TempString(maxHelpMsg));
        }

        int numInputs = (int)xInputs.size();
        if (numInputs > 30)
            numInputs = 30;

        XLOPER res;
        int xlret = Excel4v (xlfRegister,
                &res, /* not interested in any return details */
                numInputs,
                &xInputs[0]);

        if (res.xltype == xltypeErr)
        {
            failed = true;
            xlFailureCode = res.val.err;
        }

#elif SPI_XL_VERSION >= 12
        size_t numArgs;
        size_t maxArgs = 64; // the actual limit is 255
        // however we are shortening the argument names to be 255
        // which with the arguments shortened to just numbers with possible
        // query (for optional) and dividing comma gives us 64 in practice
        // tried allowing maxLenArgNames to be 256*256-1 but something went
        // wrong with Excel anyway

        // in the Excel12 interface the registered string of argNames is
        // still limited to 255, but the second limit of argNames + function name
        // does not seem to be relevant
        constexpr int  maxLenArgNames = 255;
        // there is effectively no limit to the number of help messages given
        // that we have maintained a limit on the number of arguments at 64

        // there is a case for us concatenating something from the service
        // with the function name at this point - it would save us from
        // having to do it outside

        numArgs = args.size() + 1; // inputs and outputs
        if (numArgs > maxArgs)
        {
            throw RuntimeError("Too many arguments (%d) to Excel function %s",
                               numArgs, xlFuncName.c_str());
        }
        std::string argTypes(numArgs, 'P');
        if (xlArgTypes && strlen(xlArgTypes) == numArgs)
            argTypes = std::string(xlArgTypes);

        std::string argNames = StringJoin(",", args);
        size_t firstUnnamed = args.size();

        if ((int)argNames.length() > maxLenArgNames)
        {
            int oversize = (int)argNames.length() - maxLenArgNames;

            std::vector<std::string> shortArgs(args);
            for (size_t i = shortArgs.size(); oversize > 0 && i > 0; --i)
            {
                const std::string& arg = args[i-1];
                std::string shortArg;

                if (spi::StringEndsWith(arg, "?"))
                    shortArg = spi::StringFormat("%d?", (int)i);
                else
                    shortArg = spi::StringFormat("%d", (int)i);

                firstUnnamed = i - 1;
                shortArgs[i-1] = shortArg;
                oversize += (int)shortArg.length();
                oversize -= (int)arg.length();
            }

            argNames = StringJoin(",", shortArgs);
            if ((int)argNames.length() > maxLenArgNames)
            {
                throw RuntimeError("%s: Shortened argNames are still too long (%d)",
                    xlFuncName.c_str(), argNames.length());
            }
        }

        FreeAllTempMemory();

        std::vector<XLOPER12*> xInputs;

        if (volatileInExcel)
            argTypes += "!";

        xInputs.push_back(TempString12(xllName));
        xInputs.push_back(TempString12(cFuncName));
        xInputs.push_back(TempString12(argTypes));
        if (m_upperCase)
        {
            xInputs.push_back(TempString12(PrefixUpper(xlFuncName, m_sep)));
        }
        else
        {
            xInputs.push_back(TempString12(xlFuncName));
        }
        // if we wanted to enable multi-thread then we add "$" to the end 
        // of the argNames - we don't want to do that as things stand
        xInputs.push_back(TempString12(argNames));
        if (hiddenFromWizard)
            xInputs.push_back(TempNum12(0));
        else
            xInputs.push_back(TempNum12(1));
        xInputs.push_back(TempString12(xlCatName));
        xInputs.push_back(TempStr12(L""));
        xInputs.push_back(TempStr12(L""));
        xInputs.push_back(TempString12(funcHelp));
        for (size_t i = 0; i < argsHelp.size(); ++i)
        {
            std::string argHelp = argsHelp[i];
            std::string argName = i < args.size() ? args[i] : "";

            bool optional = StringEndsWith(argName, "?");
            if (optional)
                argName = argName.substr(0, argName.length() - 1);

            // unnamed in this context is that the function wizard
            // has not been told the name of this field
            if (i >= firstUnnamed)
            {
                // unnamed fields need to include the argName as well as the argHelp
                if (argHelp.empty())
                {
                    if (optional)
                        argHelp = argName + " (optional)";
                    else
                        argHelp = argName;
                }
                else if (!argName.empty())
                {
                    // this is an unnamed field with help
                    // we want to see the name as well as the help
                    if (optional)
                    {
                        argHelp = argName + " (optional): " + argHelp;
                    }
                    else
                    {
                        argHelp = argName + ": " + argHelp;
                    }
                }
                else if (optional)
                {
                    argHelp = "(optional) " + argHelp;
                }
            }
            else if (optional)
            {
                argHelp = "(optional) " + argHelp;
            }

            if (i + 1 == argsHelp.size())
            {
                // this is handling a well-known Excel bug
                argHelp = argHelp + ". ";
            }

            xInputs.push_back(TempString12(argHelp));
        }

        int numInputs = (int)xInputs.size();

        XLOPER12 res;
        int xlret = Excel12v (xlfRegister,
                &res, /* not interested in any return details */
                numInputs,
                &xInputs[0]);

        if (res.xltype == xltypeErr)
        {
            failed = true;
            xlFailureCode = res.val.err;
        }

#else
#error "Excel version must be 4 or at least 12"
#endif
        if (failed)
        {
            std::string errmsg = StringFormat("Failed to register %s with code (%d)",
                xlFuncName, xlFailureCode);
            Excel(xlcAlert, 0, 2, TempStrConst(errmsg.c_str()), TempInt(2));
        }
        else
        {
            m_registeredFunctions.push_back(xlFuncName);
        }
        FreeAllTempMemory();
    } catch (std::exception& e) {
        Excel (xlcAlert, 0, 2, TempStrConst(e.what()), TempInt(2));
    }
}

void ExcelService::UnregisterAllFunctions()
{
    size_t size = m_registeredFunctions.size();
    for (size_t i = 0; i < size; ++i)
        Excel(xlfSetName, 0, 1, TempString(m_registeredFunctions[i]));
    m_registeredFunctions.clear();
}

/*
 * Make an XLOPER error object. If we have error popups, this will also
 * show up in a pop-up window when we re-calculated the last failure.
 */
XLOPER* ExcelService::ErrorHandler(const char* err)
{
    // static time_t timeOfLastError = 0;
    static std::string lastCellName;

    std::string cellName = xlCellName();

    // when called from Excel the last error will also contain the cellName
    std::string errorPlusCell;
    if (cellName == "None")
    {
        errorPlusCell = err;
    }
    else
    {
        errorPlusCell = StringFormat(
            "Failed at %s\n%s", cellName.c_str(), err);
    }
    m_service->set_last_error(errorPlusCell);

    // the idea is that we pop-up the error window as follows:
    //
    // 1. not called from Visual Basic (cellName != "None")
    // 2. the user recalcs the offending cell
    if (errorPopups() && cellName != "None" && cellName == lastCellName)
    {
        // we definitely don't want error pop-ups while we are in
        // the function wizard - that way lies madness
        if(!inExcelFunctionWizard())
        {
            std::string title = "Excel Error at " + cellName;
            ::MessageBox(NULL, err, title.c_str(), MB_SETFOREGROUND);
        }
    }

    lastCellName = cellName;
    XLOPER* xlo = NEW(XLOPER);

    // Note that the free bit is added afterwards - and this is why we cannot
    // try to return a static
    //
    // we will have to hope we have the memory to allocated a single XLOPER
    // otherwise we are in deep trouble anyway :)

    xlo->xltype  = xltypeErr;
    if (m_errNA)
        xlo->val.err = xlerrNA;
    else
        xlo->val.err = xlerrNum;

    return xlo;
}

FunctionCaller* ExcelService::getFunctionCaller(const char* name)
{
    return m_service->get_function_caller(name);
}

/*
***************************************************************************
***************************************************************************
** Helper functions for Excel registration
***************************************************************************
***************************************************************************
*/
static XLOPER* TempString(
    const std::string& str)
{
    return TempStrConst(str.c_str());
}

#if SPI_XL_VERSION >= 12

static XLOPER12* TempString12(
    const std::string& str)
{
    std::wstring tmp(str.begin(), str.end());
    return TempStr12(tmp.c_str());
}
#endif

static XLOPER* TempHelp(const std::vector<std::string>& argsHelp, size_t i)
{
    if (i < argsHelp.size())
    {
        return TempString(argsHelp[i]);
    }

    return TempStrConst("");
}

XLOPER* ExcelService::StartLogging(XLOPER* xl_filename, XLOPER* xl_options)
{
    XLOPER* xlo;
    try
    {
        Value output = spi::StartLogging(
            m_service,
            xloperToValue(xl_filename),
            xloperToValue(xl_options),
            getInputContext());

        xlo = xloperMakeFromValue(output);
    }
    catch (ExcelInputError&)
    {
        return xloperInputError();
    }
    catch (std::exception &e)
    {
        return ErrorHandler(e.what());
    }

    return xloperOutput(xlo);
}

XLOPER* ExcelService::StopLogging(void)
{
    XLOPER* xlo = NULL;

    try
    {
        Value output = spi::StopLogging(m_service);
        xlo = xloperMakeFromValue(output);
    }
    catch (ExcelInputError&)
    {
        return xloperInputError();
    }
    catch (std::exception &e)
    {
        return ErrorHandler(e.what());
    }

    return xloperOutput(xlo);
}

XLOPER* ExcelService::IsLogging(void)
{
    XLOPER* xlo = NULL;

    try
    {
        Value output = spi::IsLogging(m_service);
        xlo = xloperMakeFromValue(output);
    }
    catch (ExcelInputError&)
    {
        return xloperInputError();
    }
    catch (std::exception &e)
    {
        return ErrorHandler(e.what());
    }

    return xloperOutput(xlo);
}

XLOPER* ExcelService::SetErrorPopups(XLOPER* xl_errorPopups)
{
    XLOPER* xlo = NULL;

    try
    {
        bool errorPopups = xloperToBool(xl_errorPopups, "errorPopups");
        g_errorPopups = errorPopups;
        xlo = xloperFromBool(errorPopups);
    }
    catch (ExcelInputError&)
    {
        return xloperInputError();
    }
    catch (std::exception &e)
    {
        return ErrorHandler(e.what());
    }

    return xloperOutput(xlo);
}

XLOPER* ExcelService::HelpFunc(XLOPER* xl_name)
{
    XLOPER *xlo = NULL;

    try
    {
        Value name = xloperToValue(xl_name);
        Value output = spi::HelpFunc(m_service, name, getInputContext());
        if (name.getString(true).empty())
            xlo = xloperMakeFromValue(output, true);
        else
            xlo = xloperMakeFromValue(output, true, 4);
    }
    catch (ExcelInputError&)
    {
        return xloperInputError();
    }
    catch (std::exception &e)
    {
        return ErrorHandler(e.what());
    }

    return xloperOutput(xlo);
}


XLOPER* ExcelService::HelpEnum(XLOPER* xl_name)
{
    XLOPER *xlo = NULL;

    try
    {
        Value output = spi::HelpEnum(
            m_service,
            xloperToValue(xl_name),
            getInputContext());
        xlo = xloperMakeFromValue(output, true);
    }
    catch (ExcelInputError&)
    {
        return xloperInputError();
    }
    catch (std::exception &e)
    {
        return ErrorHandler(e.what());
    }

    return xloperOutput(xlo);
}

XLOPER* ExcelService::StartTiming()
{
    XLOPER* xlo = NULL;

    try
    {
        xlo = xloperFromBool(m_timing);
        m_timing = true;
    }
    catch (ExcelInputError&)
    {
        return xloperInputError();
    }
    catch (std::exception &e)
    {
        return ErrorHandler(e.what());
    }

    return xloperOutput(xlo);
}

XLOPER* ExcelService::StopTiming()
{
    XLOPER* xlo = NULL;

    try
    {
        xlo = xloperFromBool(m_timing);
        m_timing = false;
    }
    catch (ExcelInputError&)
    {
        return xloperInputError();
    }
    catch (std::exception &e)
    {
        return ErrorHandler(e.what());
    }

    return xloperOutput(xlo);
}

XLOPER* ExcelService::ClearTimings()
{
    XLOPER* xlo = NULL;

    try
    {
        xlo = xloperFromBool(true);
        m_timings.clear();
    }
    catch (ExcelInputError&)
    {
        return xloperInputError();
    }
    catch (std::exception &e)
    {
        return ErrorHandler(e.what());
    }

    return xloperOutput(xlo);
}

XLOPER* ExcelService::GetTimings()
{
    XLOPER* xlo = NULL;

    try
    {
        // returns array with four columns: name, numCalls, numFailures, totalTime
        if (m_timings.size() == 0)
            throw spi::RuntimeError("No functions have been timed");

        try
        {
            xlo = xloperMakeEmpty();
            XLOPER* x = xloperSetArray(xlo, (int)m_timings.size(), 4, false);

            std::map<std::string,ExcelTimings>::const_iterator iter;
            for (iter = m_timings.begin(); iter != m_timings.end(); ++iter)
            {
                xloperSetString(x, iter->first);
                ++x;
                x->xltype  = xltypeNum;
                x->val.num = iter->second.numCalls;
                ++x;
                x->xltype  = xltypeNum;
                x->val.num = iter->second.numFailures;
                ++x;
                x->xltype  = xltypeNum;
                x->val.num = iter->second.totalTime;
                ++x;
            }
        }
        catch (...)
        {
            xloperFree(xlo);
            xlo = NULL;
            throw;
        }
    }
    catch (ExcelInputError&)
    {
        return xloperInputError();
    }
    catch (std::exception &e)
    {
        return ErrorHandler(e.what());
    }

    return xloperOutput(xlo);
}

XLOPER* ExcelService::ObjectToString(
    XLOPER* xl_handle,
    XLOPER* xl_format,
    XLOPER* xl_options,
    XLOPER* xl_hMetaData,
    XLOPER* xl_mergeMetaData)
{
    XLOPER *xlo = NULL;

    try
    {
        Value output = spi::ObjectToString(
            xloperToValue(xl_handle),
            xloperToValue(xl_format),
            xloperToValue(xl_options),
            xloperToValue(xl_hMetaData),
            xloperToValue(xl_mergeMetaData),
            getInputContext(),
            true);
        xlo = xloperMakeFromValue(output, true);
    }
    catch (ExcelInputError&)
    {
        return xloperInputError();
    }
    catch (std::exception &e)
    {
        return ErrorHandler(e.what());
    }

    return xloperOutput(xlo);
}

XLOPER* ExcelService::ObjectFromString(
    XLOPER* xl_baseName,
    XLOPER* xl_strings)
{
    XLOPER* xlo = NULL;

    try
    {
        Value output = spi::ObjectFromString(
            m_service,
            xloperToValue(xl_strings),
            getInputContext(),
            true);
        xlo = xloperMakeFromValue(output, false, 1, xloperToValue(xl_baseName), mandatoryBaseName());
    }
    catch (ExcelInputError&)
    {
        return xloperInputError();
    }
    catch (std::exception &e)
    {
        return ErrorHandler(e.what());
    }
    return xloperOutput(xlo);
}

#if SPI_XL_VERSION >= 12

XLOPER* ExcelService::ObjectFromString(XLOPER* xl_baseName, XLOPER12* xl_str)
{
    XLOPER* xlo = NULL;

    try
    {
        Value output = spi::ObjectFromString(
            m_service,
            xloper12ToValue(xl_str),
            getInputContext(),
            true);
        xlo = xloperMakeFromValue(output, false, 1, xloperToValue(xl_baseName), mandatoryBaseName());
    }
    catch (ExcelInputError&)
    {
        return xloperInputError();
    }
    catch (std::exception& e)
    {
        return ErrorHandler(e.what());
    }
    return xloperOutput(xlo);
}

#endif

XLOPER* ExcelService::ObjectToFile(
    XLOPER* xl_handle,
    XLOPER* xl_fileName,
    XLOPER* xl_format,
    XLOPER* xl_options,
    XLOPER* xl_hMetaData,
    XLOPER* xl_mergeMetaData)
{
    XLOPER *xlo = NULL;

    try
    {
        Value output = spi::ObjectToFile(
            xloperToValue(xl_handle),
            xloperToValue(xl_fileName),
            xloperToValue(xl_format),
            xloperToValue(xl_options),
            xloperToValue(xl_hMetaData),
            xloperToValue(xl_mergeMetaData),
            getInputContext());
        xlo = xloperMakeFromValue(output);
    }
    catch (ExcelInputError&)
    {
        return xloperInputError();
    }
    catch (std::exception &e)
    {
        return ErrorHandler(e.what());
    }

    return xloperOutput(xlo);
}

XLOPER* ExcelService::ObjectFromFile(
    XLOPER* xl_baseName,
    XLOPER* xl_fileName)
{
    XLOPER* xlo = NULL;

    try
    {
        Value output = spi::ObjectFromFile(
            m_service,
            xloperToValue(xl_fileName),
            getInputContext());
        xlo = xloperMakeFromValue(output, false, 1, xloperToValue(xl_baseName), mandatoryBaseName());
    }
    catch (ExcelInputError&)
    {
        return xloperInputError();
    }
    catch (std::exception &e)
    {
        return ErrorHandler(e.what());
    }
    return xloperOutput(xlo);
}

XLOPER* ExcelService::ObjectFromURL(
    XLOPER* baseName, XLOPER* url, XLOPER* timeout, XLOPER* names, 
    XLOPER* v1, XLOPER* v2, XLOPER* v3, XLOPER* v4, XLOPER* v5,
    XLOPER* v6, XLOPER* v7, XLOPER* v8, XLOPER* v9, XLOPER* v10,
    XLOPER* v11, XLOPER* v12, XLOPER* v13, XLOPER* v14, XLOPER* v15,
    XLOPER* v16, XLOPER* v17, XLOPER* v18, XLOPER* v19, XLOPER* v20)
{
    XLOPER* xlo = NULL;

    try
    {
        Value output = spi::ObjectFromURL(
            m_service,
            xloperToValue(url),
            xloperToValue(timeout),
            xloperToValue(names),
            xloperToValue(v1),
            xloperToValue(v2),
            xloperToValue(v3),
            xloperToValue(v4),
            xloperToValue(v5),
            xloperToValue(v6),
            xloperToValue(v7),
            xloperToValue(v8),
            xloperToValue(v9),
            xloperToValue(v10),
            xloperToValue(v11),
            xloperToValue(v12),
            xloperToValue(v13),
            xloperToValue(v14),
            xloperToValue(v15),
            xloperToValue(v16),
            xloperToValue(v17),
            xloperToValue(v18),
            xloperToValue(v19),
            xloperToValue(v20),
            getInputContext());
        xlo = xloperMakeFromValue(
            output, false, 1, xloperToValue(baseName), mandatoryBaseName());
    }
    catch (ExcelInputError&)
    {
        return xloperInputError();
    }
    catch (std::exception &e)
    {
        return ErrorHandler(e.what());
    }
    return xloperOutput(xlo);
}

XLOPER* ExcelService::ObjectGet(XLOPER* xl_handle, XLOPER* xl_name)
{
    XLOPER* xlo = NULL;

    try
    {
        Value name = xloperToValue(xl_name);
        Value handle = xloperToValue(xl_handle);
        bool fillBlank = true;
        Value output = spi::ObjectGet(handle, name, getInputContext());
        // what if the output is an object?
        // in that case we will use the original base name + the name of the field

        // if ObjectGet succeeded then handle must be a scalar string
        std::string stringHandle = spi_util::StringStrip(handle.getString(true));
        // this definition matches what is in xloperSetFromValue
        std::string oldBaseName = spi_util::StringSplit(stringHandle, '#')[0];
        std::string baseNamePrefix;
        if (!oldBaseName.empty())
            baseNamePrefix = oldBaseName + ".";

        if (name.getType() == Value::ARRAY)
        {
            xlo = xloperMakeFromValue(output, true, name.getArray()->size(),
                name, mandatoryBaseName(), baseNamePrefix, fillBlank);
        }
        else
        {
            xlo = xloperMakeFromValue(output, true, 1, name, mandatoryBaseName(),
                baseNamePrefix, fillBlank);
        }
    }
    catch (ExcelInputError&)
    {
        return xloperInputError();
    }
    catch (std::exception &e)
    {
        return ErrorHandler(e.what());
    }
    return xloperOutput(xlo);
}

XLOPER* ExcelService::ObjectPut(XLOPER* baseName, XLOPER* handle, XLOPER* names,
    XLOPER* v1, XLOPER* v2, XLOPER* v3, XLOPER* v4, XLOPER* v5,
    XLOPER* v6, XLOPER* v7, XLOPER* v8, XLOPER* v9, XLOPER* v10,
    XLOPER* v11, XLOPER* v12, XLOPER* v13, XLOPER* v14, XLOPER* v15,
    XLOPER* v16, XLOPER* v17, XLOPER* v18, XLOPER* v19, XLOPER* v20,
    XLOPER* v21, XLOPER* v22, XLOPER* v23, XLOPER* v24, XLOPER* v25)
{
    XLOPER* xlo = NULL;

    try
    {
        Value output = spi::ObjectPut(
            xloperToValue(handle),
            xloperToValue(names),
            xloperToValue(v1), xloperToValue(v2), xloperToValue(v3),
            xloperToValue(v4), xloperToValue(v5), xloperToValue(v6),
            xloperToValue(v7), xloperToValue(v8), xloperToValue(v9),
            xloperToValue(v10), xloperToValue(v11), xloperToValue(v12),
            xloperToValue(v13), xloperToValue(v14), xloperToValue(v15),
            xloperToValue(v16), xloperToValue(v17), xloperToValue(v18),
            xloperToValue(v19), xloperToValue(v20), xloperToValue(v21),
            xloperToValue(v22), xloperToValue(v23), xloperToValue(v24),
            xloperToValue(v25),
            getInputContext());

        xlo = xloperMakeFromValue(output, false, 1,
            xloperToValue(baseName), mandatoryBaseName());
    }
    catch (ExcelInputError&)
    {
        return xloperInputError();
    }
    catch (std::exception &e)
    {
        return ErrorHandler(e.what());
    }
    return xloperOutput(xlo);

}

XLOPER* ExcelService::ObjectPutMetaData(XLOPER* baseName, XLOPER* handle, XLOPER* names,
    XLOPER* v1, XLOPER* v2, XLOPER* v3, XLOPER* v4, XLOPER* v5,
    XLOPER* v6, XLOPER* v7, XLOPER* v8, XLOPER* v9, XLOPER* v10,
    XLOPER* v11, XLOPER* v12, XLOPER* v13, XLOPER* v14, XLOPER* v15,
    XLOPER* v16, XLOPER* v17, XLOPER* v18, XLOPER* v19, XLOPER* v20,
    XLOPER* v21, XLOPER* v22, XLOPER* v23, XLOPER* v24, XLOPER* v25)
{
    XLOPER* xlo = NULL;

    try
    {
        Value output = spi::ObjectPutMetaData(
            xloperToValue(handle),
            xloperToValue(names),
            xloperToValue(v1), xloperToValue(v2), xloperToValue(v3),
            xloperToValue(v4), xloperToValue(v5), xloperToValue(v6),
            xloperToValue(v7), xloperToValue(v8), xloperToValue(v9),
            xloperToValue(v10), xloperToValue(v11), xloperToValue(v12),
            xloperToValue(v13), xloperToValue(v14), xloperToValue(v15),
            xloperToValue(v16), xloperToValue(v17), xloperToValue(v18),
            xloperToValue(v19), xloperToValue(v20), xloperToValue(v21),
            xloperToValue(v22), xloperToValue(v23), xloperToValue(v24),
            xloperToValue(v25),
            getInputContext());

        xlo = xloperMakeFromValue(output, false, 1,
            xloperToValue(baseName), mandatoryBaseName());
    }
    catch (ExcelInputError&)
    {
        return xloperInputError();
    }
    catch (std::exception& e)
    {
        return ErrorHandler(e.what());
    }
    return xloperOutput(xlo);

}



XLOPER* ExcelService::ObjectToMap(XLOPER* baseName, XLOPER* handle)
{
    XLOPER* xlo = NULL;

    try
    {
        Value output = spi::ObjectToMap(
            xloperToValue(handle),
            getInputContext());
        xlo = xloperMakeFromValue(
            output, false, 1, xloperToValue(baseName), mandatoryBaseName());
    }
    catch (ExcelInputError&)
    {
        return xloperInputError();
    }
    catch (std::exception &e)
    {
        return ErrorHandler(e.what());
    }
    return xloperOutput(xlo);
}

XLOPER* ExcelService::ObjectCount(XLOPER* xl_className)
{
    XLOPER* xlo = NULL;

    try
    {
        Value output = spi::ObjectCount(
            xloperToValue(xl_className),
            getInputContext());
        xlo = xloperMakeFromValue(output);
    }
    catch (ExcelInputError&)
    {
        return xloperInputError();
    }
    catch (std::exception &e)
    {
        return ErrorHandler(e.what());
    }
    return xloperOutput(xlo);
}

XLOPER* ExcelService::ObjectFree(XLOPER* xl_handle)
{
    XLOPER* xlo = NULL;

    try
    {
        Value output = spi::ObjectFree(
            xloperToValue(xl_handle),
            getInputContext());
        xlo = xloperMakeFromValue(output);
    }
    catch (ExcelInputError&)
    {
        return xloperInputError();
    }
    catch (std::exception &e)
    {
        return ErrorHandler(e.what());
    }
    return xloperOutput(xlo);
}

XLOPER* ExcelService::ObjectFreeAll()
{
    XLOPER* xlo = NULL;

    try
    {
        Value output = spi::ObjectFreeAll();
        m_service->clear_read_cache();
        xlo = xloperMakeFromValue(output);
    }
    catch (ExcelInputError&)
    {
        return xloperInputError();
    }
    catch (std::exception &e)
    {
        return ErrorHandler(e.what());
    }
    return xloperOutput(xlo);
}

XLOPER* ExcelService::ObjectList(XLOPER* xl_prefix, XLOPER* xl_className)
{
    XLOPER* xlo = NULL;

    try
    {
        Value output = spi::ObjectList(
            xloperToValue(xl_prefix),
            xloperToValue(xl_className),
            getInputContext());
        xlo = xloperMakeFromValue(output);
    }
    catch (ExcelInputError&)
    {
        return xloperInputError();
    }
    catch (std::exception &e)
    {
        return ErrorHandler(e.what());
    }
    return xloperOutput(xlo);
}

XLOPER* ExcelService::ObjectClassName(XLOPER* xl_handle)
{
    XLOPER* xlo = NULL;

    try
    {
        Value output = spi::ObjectClassName(
            xloperToValue(xl_handle),
            getInputContext());
        xlo = xloperMakeFromValue(output);
    }
    catch (ExcelInputError&)
    {
        return xloperInputError();
    }
    catch (std::exception &e)
    {
        return ErrorHandler(e.what());
    }
    return xloperOutput(xlo);
}

XLOPER* ExcelService::ObjectCoerce(XLOPER* xl_baseName, XLOPER* xl_className, XLOPER* xl_value)
{
    XLOPER* xlo = NULL;

    try
    {
        Value output = spi::ObjectCoerce(
            m_service,
            xloperToValue(xl_className),
            xloperToValue(xl_value),
            getInputContext());

        xlo = xloperMakeFromValue(output, false, 1, xloperToValue(xl_baseName));
    }
    catch (ExcelInputError&)
    {
        return xloperInputError();
    }
    catch (std::exception &e)
    {
        return ErrorHandler(e.what());
    }
    return xloperOutput(xlo);
}

XLOPER * ExcelService::ObjectSHA(XLOPER * handle, XLOPER * version)
{
    XLOPER* xlo = NULL;

    try
    {
        Value output = spi::ObjectSHA(
            xloperToValue(handle),
            xloperToValue(version),
            getInputContext());

        xlo = xloperMakeFromValue(output);
    }
    catch (ExcelInputError&)
    {
        return xloperInputError();
    }
    catch (std::exception &e)
    {
        return ErrorHandler(e.what());
    }
    return xloperOutput(xlo);
}

XLOPER* ExcelService::UrlCacheSize()
{
    XLOPER* xlo = NULL;

    try
    {
        Value output = spi::UrlCacheSize(m_service);
        xlo = xloperMakeFromValue(output);
    }
    catch (ExcelInputError&)
    {
        return xloperInputError();
    }
    catch (std::exception &e)
    {
        return ErrorHandler(e.what());
    }
    return xloperOutput(xlo);
}

XLOPER* ExcelService::UrlCacheSave(XLOPER* filename)
{
    XLOPER* xlo = NULL;

    try
    {
        Value output = spi::UrlCacheSave(
            m_service,
            xloperToValue(filename),
            getInputContext());

        xlo = xloperMakeFromValue(output);
    }
    catch (ExcelInputError&)
    {
        return xloperInputError();
    }
    catch (std::exception &e)
    {
        return ErrorHandler(e.what());
    }
    return xloperOutput(xlo);
}

XLOPER* ExcelService::UrlCacheLoad(XLOPER* filename)
{
    XLOPER* xlo = NULL;

    try
    {
        Value output = spi::UrlCacheLoad(
            m_service,
            xloperToValue(filename),
            getInputContext());

        xlo = xloperMakeFromValue(output);
    }
    catch (ExcelInputError&)
    {
        return xloperInputError();
    }
    catch (std::exception &e)
    {
        return ErrorHandler(e.what());
    }
    return xloperOutput(xlo);
}

XLOPER* ExcelService::UrlCacheInit()
{
    XLOPER* xlo = NULL;

    try
    {
        Value output = spi::UrlCacheInit(m_service);
        xlo = xloperMakeFromValue(output);
    }
    catch (ExcelInputError&)
    {
        return xloperInputError();
    }
    catch (std::exception &e)
    {
        return ErrorHandler(e.what());
    }
    return xloperOutput(xlo);
}

XLOPER* ExcelService::UrlCacheClear()
{
    XLOPER* xlo = NULL;

    try
    {
        Value output = spi::UrlCacheClear(m_service);
        xlo = xloperMakeFromValue(output);
    }
    catch (ExcelInputError&)
    {
        return xloperInputError();
    }
    catch (std::exception &e)
    {
        return ErrorHandler(e.what());
    }
    return xloperOutput(xlo);
}

spdoc::ServiceConstSP ExcelService::getDoc()
{
    if (m_doc)
        return m_doc;

    // will look in current directory and parent directory
    ServiceSP docService = spdoc::spdoc_exported_service();
    std::string basename = StringFormat(
        "%s.svo", m_service->get_name().c_str());

    try
    {
        std::string filename = spi_util::path::join(
            m_dirname.c_str(), basename.c_str(), 0);
        m_doc = spdoc::Service::Coerce(
            docService->object_from_file(filename));
        return m_doc;
    }
    catch (...)
    {
        // ignore errors and try one higher up
    }

    try
    {
        std::string filename = spi_util::path::join(
            spi_util::path::dirname(m_dirname).c_str(), basename.c_str(), 0);
        m_doc = spdoc::Service::Coerce(
            docService->object_from_file(filename));
        return m_doc;
    }
    catch (...)
    {
        // ignore errors - we will throw our own exception
    }

    throw RuntimeError("Could not find service documentation for %s",
                       m_service->get_name().c_str());
}

bool ExcelService::isLogging() const
{
    return m_service->is_logging();
}

bool ExcelService::isTiming() const
{
    return m_timing;
}

void ExcelService::addTiming(const std::string& name, bool failed, double time)
{
    ExcelTimings& timings = m_timings[name];
    timings.numCalls += 1;
    if (failed)
        timings.numFailures += 1;
    timings.totalTime += time;
}

bool ExcelService::mandatoryBaseName() const
{
    return !m_optionalBaseName;
}

const std::string& ExcelService::getNamespace() const
{
    return m_service->get_namespace();
}

bool ExcelService::errorPopups() const
{
    return g_errorPopups;
}

void ExcelService::logMessage(const std::string& msg)
{
    m_service->log_message(msg);
}

ExcelTimings::ExcelTimings()
    :
    numCalls(0),
    numFailures(0),
    totalTime(0.0)
{}

ExcelTimer::ExcelTimer(ExcelService* svc, const char* name)
    :
    m_svc(svc),
    m_name(name),
    m_failed(false),
    m_notCalled(false),
    m_clock()
{
    if (svc->isLogging())
        svc->logMessage(xlCellName() + " " + svc->getNamespace() + "." + name);

    if (svc->isTiming())
        m_clock.Start();
}

ExcelTimer::~ExcelTimer()
{
    if (!m_notCalled && m_svc->isTiming())
    {
        m_svc->addTiming(m_name, m_failed, m_clock.Time());
    }
}

void ExcelTimer::SetFailure()
{
    m_failed = true;
}

void ExcelTimer::SetNotCalled()
{
    m_notCalled = true;
}

const char* ExcelTimer::Name()
{
    return m_name;
}

SPI_END_NAMESPACE

