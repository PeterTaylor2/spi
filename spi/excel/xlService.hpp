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

#if SPI_XL_VERSION >= 12
typedef struct xloper12 XLOPER12;
#endif

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
    ExcelService(const ServiceSP& service, ExcelService* parent, const std::string& xllName,
        const char* sep = ".",
        bool errorPopups = false,
        bool upperCase = false,
        bool optionalBaseName = false,
        bool errNA = false);

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
        const std::string& objectToFile = std::string("object_to_file"),
        const std::string& objectFromFile = std::string("object_from_file"),
        const std::string& objectCount = std::string("object_count"),
        const std::string& objectFree = std::string("object_free"),
        const std::string& objectFreeAll = std::string("object_free_all"),
        const std::string& objectList = std::string("object_list"),
        const std::string& objectClassName = std::string("object_class_name"),
        const std::string& objectSHA= std::string("object_sha"));

    /**
     * Register a function with Excel and keep track of its name so that
     * we can unregister the function later.
     *
     * @param pxDll
     *    XLOPER containing some sort of handle to the XLL.
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

    /**
     * These methods need the service. You should create functions
     * that can call these methods using get_service() to get the
     * service object.
     */
    XLOPER* StartLogging(XLOPER* filename, XLOPER* options);
    XLOPER* StopLogging(void);
    XLOPER* IsLogging(void);
    XLOPER* SetErrorPopups(XLOPER* errorPopups);
    XLOPER* HelpFunc(XLOPER* name);
    XLOPER* HelpEnum(XLOPER* name);
    XLOPER* StartTiming();
    XLOPER* StopTiming();
    XLOPER* ClearTimings();
    XLOPER* GetTimings();

    XLOPER* ObjectToString(XLOPER* handle, XLOPER* format, XLOPER* options, XLOPER* hMetaData);
    XLOPER* ObjectFromString(XLOPER* baseName, XLOPER* strings);
#if SPI_XL_VERSION >= 12
    XLOPER* ObjectFromString(XLOPER* baseName, XLOPER12* str);
#endif

    XLOPER* ObjectToFile(XLOPER* handle, XLOPER* fileName, XLOPER* format,
        XLOPER* options, XLOPER* hMetaData);
    XLOPER* ObjectFromFile(XLOPER* baseName, XLOPER* fileName);
    XLOPER* ObjectFromURL(XLOPER* baseName, XLOPER* url, XLOPER* timeout,
        XLOPER* names, 
        XLOPER* v1, XLOPER* v2, XLOPER* v3, XLOPER* v4, XLOPER* v5,
        XLOPER* v6, XLOPER* v7, XLOPER* v8, XLOPER* v9, XLOPER* v10,
        XLOPER* v11, XLOPER* v12, XLOPER* v13, XLOPER* v14, XLOPER* v15,
        XLOPER* v16, XLOPER* v17, XLOPER* v18, XLOPER* v19, XLOPER* v20);

    XLOPER* ObjectGet(XLOPER* handle, XLOPER* name);
    XLOPER* ObjectPut(XLOPER* baseName, XLOPER* handle, XLOPER* names,
        XLOPER* v1, XLOPER* v2, XLOPER* v3, XLOPER* v4, XLOPER* v5,
        XLOPER* v6, XLOPER* v7, XLOPER* v8, XLOPER* v9, XLOPER* v10,
        XLOPER* v11, XLOPER* v12, XLOPER* v13, XLOPER* v14, XLOPER* v15,
        XLOPER* v16, XLOPER* v17, XLOPER* v18, XLOPER* v19, XLOPER* v20,
        XLOPER* v21, XLOPER* v22, XLOPER* v23, XLOPER* v24, XLOPER* v25);
    XLOPER* ObjectToMap(XLOPER* baseName, XLOPER* handle);

    XLOPER* ObjectCount(XLOPER* className);
    XLOPER* ObjectFree(XLOPER* handle);
    XLOPER* ObjectFreeAll();
    XLOPER* ObjectList(XLOPER* prefix, XLOPER* className);
    XLOPER* ObjectClassName(XLOPER* handle);
    XLOPER* ObjectCoerce(XLOPER* baseName, XLOPER* className, XLOPER* value);
    XLOPER* ObjectSHA(XLOPER* handle, XLOPER* version);

    XLOPER* UrlCacheSize();
    XLOPER* UrlCacheSave(XLOPER* filename);
    XLOPER* UrlCacheLoad(XLOPER* filename);
    XLOPER* UrlCacheInit();
    XLOPER* UrlCacheClear();

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
    ExcelService* m_parent;
    std::vector<std::string> m_registeredFunctions;
    bool m_errorPopups;
    std::string m_dirname;
    spdoc::ServiceConstSP m_doc;
    InputContext* m_inputContext;
    const char* m_sep;
    bool m_upperCase;
    bool m_optionalBaseName;
    bool m_errNA;
    bool m_timing;
    std::map<std::string, ExcelTimings> m_timings;
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
