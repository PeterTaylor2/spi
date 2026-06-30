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
#ifndef SPI_XL_SERVICE_HPP
#define SPI_XL_SERVICE_HPP

/*
***************************************************************************
** xlService.hpp
**
** Defines the Excel service class. This is linked to the Service class
** for the underlying C++ outer library.
***************************************************************************
*/

#include "DeclSpec.h"
#include <spi/Service.hpp>
#include <spi/spdoc_configTypes.hpp>
#include <spi_util/ClockUtil.hpp>
#include <vector>
#include <string>

typedef struct xloper XLOPER;
typedef struct xloper12 XLOPER12;

#include "xlValue.hpp"

SPI_BEGIN_NAMESPACE

SPI_XL_IMPORT
std::string GetXLLName();

SPI_XL_IMPORT
void StartupError(const std::string& xllName, const char* errmsg);

struct ExcelTimings
{
    ExcelTimings();

    int numCalls;
    int numFailures;
    double totalTime;
};

class SPI_XL_IMPORT ExcelService
{
public:
    ExcelService(const ServiceSP& service, const std::string& xllName,
        const char* sep = ".",
        bool upperCase = false,
        bool optionalBaseName = false,
        bool errNA = false,
        bool nsUpperCase = false);

    /**
     * Registers the standard functions that are provided for all libraries.
     */
    void RegisterStandardFunctions(const std::string& xllName,
        const std::string& helpFunc = std::string("help_func"),
        const std::string& helpFuncList = std::string(),
        const std::string& helpEnum = std::string("help_enum"),
        const std::string& objectCoerce = std::string("object_coerce"),
        const std::string& startLogging = std::string("start_logging"),
        const std::string& stopLogging = std::string("stop_logging"),
        const std::string& startTiming = std::string("start_timing"),
        const std::string& stopTiming = std::string("stop_timing"),
        const std::string& clearTimings = std::string("clear_timings"),
        const std::string& getTimings = std::string("get_timings"),
        const std::string& setErrorPopups = std::string("set_error_popups"),
        const std::string& objectToString = std::string("object_to_string"),
        const std::string& objectFromString = std::string("object_from_string"),
        const std::string& objectGet = std::string("object_get"),
        const std::string& objectPut = std::string("object_put"),
        const std::string& objectPutMetaData = std::string("object_put_meta_data"),
        const std::string& objectToFile = std::string("object_to_file"),
        const std::string& objectFromFile = std::string("object_from_file"),
        const std::string& objectCount = std::string("object_count"),
        const std::string& objectFree = std::string("object_free"),
        const std::string& objectFreeAll = std::string("object_free_all"),
        const std::string& objectList = std::string("object_list"),
        const std::string& objectClassName = std::string("object_class_name"),
        const std::string& objectSHA= std::string("object_sha"),
        bool noPrefixObjectFuncs = false);

    /**
     * Register a function with Excel and keep track of its name so that
     * we can unregister the function later.
     *
     * @param xllName
     *    The name of the XLL.
     * @param cFuncName
     *    The name of the c-function that Excel will call when the user
     *    requests xlFuncName.
     * @param xlFuncName
     *    The name of the function that will appear in Excel.
     * @param xlCatName
     *    The Excel category name where we will register all the functions.
     * @param args
     *    Vector of argument names.
     * @param funcHelp
     *    Function help for the function as a whole.
     * @param argsHelp
     *    Short help for each parameter.
     * @param hiddenFromWizard
     *    If defined then this function does not appear in Excel function wizard.
     * @param volatileInExcel
     *    If defined then this function will be declared as volatile in Excel.
     * @param xlArgTypes
     *    If not defined then the function is registered with a string of 'P'
     *    (of size args.size() + 1). If defined and of the right length then
     *    xlArgTypes is used instead for the registration. Note that XLOPER12
     *    needs 'Q' instead of 'P' and we might selectively use XLOPER12 instead
     *    of XLOPER.
     */
    void RegisterFunction(
        const std::string&              xllName,
        const std::string&              cFuncName,
        const std::string&              xlFuncName,
        const std::string&              xlCatName,
        const std::vector<std::string>& args,
        const std::string&              funcHelp,
        const std::vector<std::string>& argsHelp,
        bool hiddenFromWizard = false,
        bool volatileInExcel = false,
        const char* xlArgTypes = nullptr);

    /**
     * Goes through the list of registered functions and unregisters
     * them all.
     */
    void UnregisterAllFunctions();

    /**
     * Make an XLOPER error object. If we have error popups, this will also
     * show up in a pop-up window.
     */
    XLOPER* ErrorHandler(const char* err);
    XLOPER12* ErrorHandler12(const char* err);

    /**
     * These methods need the service. You should create functions
     * that can call these methods using get_service() to get the
     * service object.
     */
    XLOPER12* StartLogging(XLOPER12* filename, XLOPER12* options, XLOPER12* minimal);
    XLOPER12* StopLogging(void);
    XLOPER12* IsLogging(void);
    XLOPER12* SetErrorPopups(XLOPER12* errorPopups);
    XLOPER12* HelpFunc(XLOPER12* name);
    XLOPER12* HelpEnum(XLOPER12* name);
    XLOPER12* StartTiming();
    XLOPER12* StopTiming();
    XLOPER12* ClearTimings();
    XLOPER12* GetTimings();

    XLOPER12* ObjectToString(XLOPER12* handle, XLOPER12* format, XLOPER12* options,
        XLOPER12* hMetaData, XLOPER12* mergeMetaData);
    XLOPER12* ObjectFromString(XLOPER12* baseName, XLOPER12* str);

    XLOPER12* ObjectToFile(XLOPER12* handle, XLOPER12* fileName, XLOPER12* format,
        XLOPER12* options, XLOPER12* hMetaData, XLOPER12* mergeMetaData);
    XLOPER12* ObjectFromFile(XLOPER12* baseName, XLOPER12* fileName);
    XLOPER12* ObjectFromURL(XLOPER12* baseName, XLOPER12* url, XLOPER12* timeout,
        XLOPER12* names, 
        XLOPER12* v1, XLOPER12* v2, XLOPER12* v3, XLOPER12* v4, XLOPER12* v5,
        XLOPER12* v6, XLOPER12* v7, XLOPER12* v8, XLOPER12* v9, XLOPER12* v10,
        XLOPER12* v11, XLOPER12* v12, XLOPER12* v13, XLOPER12* v14, XLOPER12* v15,
        XLOPER12* v16, XLOPER12* v17, XLOPER12* v18, XLOPER12* v19, XLOPER12* v20);

    XLOPER12* ObjectGet(XLOPER12* handle, XLOPER12* name);
    XLOPER12* ObjectPut(XLOPER12* baseName, XLOPER12* handle, XLOPER12* names,
        XLOPER12* v1, XLOPER12* v2, XLOPER12* v3, XLOPER12* v4, XLOPER12* v5,
        XLOPER12* v6, XLOPER12* v7, XLOPER12* v8, XLOPER12* v9, XLOPER12* v10,
        XLOPER12* v11, XLOPER12* v12, XLOPER12* v13, XLOPER12* v14, XLOPER12* v15,
        XLOPER12* v16, XLOPER12* v17, XLOPER12* v18, XLOPER12* v19, XLOPER12* v20,
        XLOPER12* v21, XLOPER12* v22, XLOPER12* v23, XLOPER12* v24, XLOPER12* v25);
    XLOPER12* ObjectPutMetaData(XLOPER12* baseName, XLOPER12* handle, XLOPER12* names,
        XLOPER12* v1, XLOPER12* v2, XLOPER12* v3, XLOPER12* v4, XLOPER12* v5,
        XLOPER12* v6, XLOPER12* v7, XLOPER12* v8, XLOPER12* v9, XLOPER12* v10,
        XLOPER12* v11, XLOPER12* v12, XLOPER12* v13, XLOPER12* v14, XLOPER12* v15,
        XLOPER12* v16, XLOPER12* v17, XLOPER12* v18, XLOPER12* v19, XLOPER12* v20,
        XLOPER12* v21, XLOPER12* v22, XLOPER12* v23, XLOPER12* v24, XLOPER12* v25);
    XLOPER12* ObjectToMap(XLOPER12* baseName, XLOPER12* handle);

    XLOPER12* ObjectCount(XLOPER12* className);
    XLOPER12* ObjectFree(XLOPER12* handle);
    XLOPER12* ObjectFreeAll();
    XLOPER12* ObjectList(XLOPER12* prefix, XLOPER12* className);
    XLOPER12* ObjectClassName(XLOPER12* handle);
    XLOPER12* ObjectCoerce(XLOPER12* baseName, XLOPER12* className, XLOPER12* value);
    XLOPER12* ObjectSHA(XLOPER12* handle, XLOPER12* version);

    XLOPER12* UrlCacheSize();
    XLOPER12* UrlCacheSave(XLOPER12* filename);
    XLOPER12* UrlCacheLoad(XLOPER12* filename);
    XLOPER12* UrlCacheInit();
    XLOPER12* UrlCacheClear();

    FunctionCaller* getFunctionCaller(const char* name);
    InputContext* getInputContext();
    spdoc::ServiceConstSP getDoc();
    bool isLogging() const;
    bool isTiming() const;
    void addTiming(const std::string& name, bool failed, double time);
    bool mandatoryBaseName() const;
    void logMessage(const std::string& msg);
    const std::string& getNamespace() const;
    bool errorPopups() const;

private:
    ServiceSP m_service;
    std::vector<std::string> m_registeredFunctions;
    std::string m_dirname;
    spdoc::ServiceConstSP m_doc;
    InputContext* m_inputContext;
    const char* m_sep;
    bool m_upperCase;
    bool m_optionalBaseName;
    bool m_errNA;
    bool m_nsUpperCase;
    std::string m_functionPrefix;
};

class SPI_XL_IMPORT ExcelTimer
{
public:
    ExcelTimer(ExcelService* svc, const char* name);
    ~ExcelTimer();

    void SetNotCalled();
    void SetFailure();
    const char* Name();

private:
    ExcelService* m_svc;
    const char* m_name;
    bool m_failed;
    bool m_notCalled;
    spi_util::Clock m_clock;
};

SPI_END_NAMESPACE

#endif /* SPI_XL_SERVICE_HPP */
