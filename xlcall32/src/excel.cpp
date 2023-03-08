#include "xlcall.h"

#include <stdarg.h>
#include "excel.h"

#include <map>
#include <iostream>
#include <string>
#include <vector>

namespace {
    std::string xllName;

    typedef short (xlAutoOpenFunc)();
    typedef short (xlAutoCloseFunc)();

    // is there a better way than enumerating all the possible function calls?
    typedef XLOPER* (xlCall0)();
    typedef XLOPER* (xlCall1)(XLOPER*);

    struct LibraryInfo
    {
        HMODULE hXLL;
        xlAutoOpenFunc* xlAutoOpen;
        xlAutoCloseFunc* xlAutoClose;
        xlAutoFreeFunc* xlAutoFree;
    };

    std::map<std::string, LibraryInfo> indexNameXLL;
    std::map<std::string, std::vector<std::string> > indexXLLFunctions;
    struct FunctionInfo
    {
        FARPROC func;
        xlAutoFreeFunc* freeFunc;
        int numArgs;

        FunctionInfo() : func(NULL), freeFunc(NULL), numArgs(0)
        {}
    };

    const LibraryInfo* getLibraryInfo(const std::string& sXLL)
    {
        std::map<std::string, LibraryInfo>::const_iterator iter = indexNameXLL.find(sXLL);
        if (iter == indexNameXLL.end())
            return NULL;

        return &(iter->second);
    }

    std::map<std::string, FunctionInfo> indexNameFunction;

    void DeregisterXLL(const std::string& sXLL)
    {
        if (indexXLLFunctions.count(sXLL) == 0)
            return;

        std::vector<std::string> functions = indexXLLFunctions[sXLL];
        indexXLLFunctions.erase(sXLL);

        for (std::vector<std::string>::const_iterator iter = functions.begin();
            iter != functions.end(); ++iter)
        {
            if (indexNameFunction.count(*iter) != 0)
            {
                indexNameFunction.erase(*iter);
            }
        }
    }

    XLOPER xloperError(int errorType)
    {
        XLOPER oper;
        oper.val.err = errorType;
        oper.xltype = xltypeErr;

        return oper;
    }

    XLOPER xloperNoValue(int xltype)
    {
        XLOPER oper;
        oper.xltype = xltypeMissing;
        return oper;
    }

    void xloperSetString(XLOPER* oper, const std::string& str)
    {
        oper->xltype = xltypeStr;
        oper->val.str = (char*)malloc(256);
        
        size_t len = str.length();
        if (len > 255)
            len = 255;

        oper->val.str[0] = (CHAR)len;
        strncpy(&oper->val.str[1], str.c_str(), len);
    }

    XLOPER ErrName = xloperError(xlerrName);
    XLOPER ErrValue = xloperError(xlerrValue);
    XLOPER ArgMissing = xloperNoValue(xltypeMissing);

    // converts an XLOPER to string but only if it is a string
    // this returns a pointer to static - you must populate std::string immediately
    const char* xloperToCString(LPXLOPER oper)
    {
        static char buf[256];
        if (oper->xltype & xltypeStr)
        {
            char* str = oper->val.str;
            size_t len = (unsigned char)str[0];

            if (len > 255)
                len = 255;

            strncpy(buf, &str[1], len);
            buf[len] = '\0';

            return &buf[0];
        }
        return NULL;
    }

    int xloperToInt(LPXLOPER oper)
    {
        if (oper->xltype & xltypeInt)
            return oper->val.w;

        throw std::runtime_error("XLOPER is not an integer");
    }

    int RegisterFunction(
        const std::string& xll,
        const std::string& cFuncName,
        const std::string& argTypes,
        const std::string& xlFuncName)
    {
        const LibraryInfo* info = getLibraryInfo(xll);
        if (!info)
            return xlretFailed;

        if (argTypes.length() == 0)
            return xlretFailed;

        FunctionInfo funcInfo;

        funcInfo.freeFunc = info->xlAutoFree;
        funcInfo.numArgs = (int)argTypes.length() - 1;
        if (argTypes[argTypes.length() - 1] == '!')
            funcInfo.numArgs -= 1;

        if (funcInfo.numArgs < 0)
            return xlretFailed;

        funcInfo.func = GetProcAddress(info->hXLL, cFuncName.c_str());
        if (funcInfo.func == NULL)
            return xlretFailed;

        indexNameFunction[xlFuncName] = funcInfo;
        indexXLLFunctions[xll].push_back(xlFuncName);

        return xlretSuccess;
    }

    int Excel4Free(LPXLOPER oper)
    {
        WORD xltype = oper->xltype & 0x0FFF;
        switch (xltype)
        {
        case xltypeStr:
            free(oper->val.str);
            oper->val.str = NULL;
            oper->xltype = xltypeNil;
            break;
        default:
            break;
        }
        return xlretSuccess;
    }

    int Excel4Alert(LPXLOPER xMsg, LPXLOPER xFlags=NULL)
    {
        std::string msg(xloperToCString(xMsg));
        int flags = xFlags ? xloperToInt(xFlags) : 2;

        switch (flags)
        {
        case 2:
            std::cout << "WARNING: ";
            break;
        default:
            std::cout << "flags=" << flags << ": ";
            break;
        }
        std::cout << msg << std::endl;
        std::cout << "Enter to continue:";
        char buf[128];
        std::cin >> buf;

        return xlretSuccess;
    }

    int Excel4GetWorkspace(LPXLOPER oper, LPXLOPER xRequest)
    {
        int request = xloperToInt(xRequest);
        switch (request)
        {
        case 2:
            xloperSetString(oper, "1.0");
            break;
        default:
            throw std::runtime_error("Unknown request");
        }
        return xlretSuccess;
    }

    int Excel4Register(
        LPXLOPER operRes,
        LPXLOPER xllName,
        LPXLOPER cFuncName,
        LPXLOPER argTypes,
        LPXLOPER xlFuncName)
    {
        const char* tmp;

        tmp = xloperToCString(xllName);
        if (!tmp)
            return xlretInvXloper;
        std::string sXLL(tmp);

        tmp = xloperToCString(cFuncName);
        if (!tmp)
            return xlretInvXloper;
        std::string sCFuncName(tmp);

        tmp = xloperToCString(argTypes);
        if (!tmp)
            return xlretInvXloper;
        std::string sArgTypes(tmp);

        tmp = xloperToCString(xlFuncName);
        if (!tmp)
            return xlretInvXloper;
        std::string sXLFuncName(tmp);

        return RegisterFunction(sXLL, sCFuncName, sArgTypes, sXLFuncName);
    }
} // end of anonymous namespace

int xllOpen(const char *xll)
{
    std::string sXLL(xll);

    if (indexNameXLL.count(sXLL) != 0)
        return 0; // already open

    HMODULE hXLL = LoadLibrary(xll);
    if (hXLL == NULL)
        return -1;

    int status = -1;

    LibraryInfo info;
    info.hXLL = hXLL;
    info.xlAutoOpen = (xlAutoOpenFunc*)GetProcAddress(hXLL, "xlAutoOpen");
    info.xlAutoClose = (xlAutoCloseFunc*)GetProcAddress(hXLL, "xlAutoClose");
    info.xlAutoFree = (xlAutoFreeFunc*)GetProcAddress(hXLL, "xlAutoFree");

    if (!info.xlAutoOpen || !info.xlAutoClose || !info.xlAutoFree)
        goto done; /* failure */

    xllName = sXLL;
    indexNameXLL[sXLL] = info;

    if (info.xlAutoOpen() != 1)
        goto done; /* failure */

    status = 0;

done:

    if (status != 0)
    {
        DeregisterXLL(sXLL);
        if (hXLL != NULL)
            FreeLibrary(hXLL);
        return status;
    }

    return 0;
}

int xllClose(const char * xll)
{
    std::string sXLL(xll);

    if (indexNameXLL.count(sXLL) == 0)
        return -1; // not open

    const LibraryInfo& info = indexNameXLL[sXLL];

    info.xlAutoClose();
    FreeLibrary(info.hXLL);

    indexNameXLL.erase(sXLL);
    DeregisterXLL(sXLL);
    return 0;
}

int _cdecl Excel4(int xlfn, LPXLOPER operRes, int count, ...)
{
    LPXLOPER opers[64];
    int i;

    if (count < 0 || count > 64)
        return xlretInvCount;

    va_list args;
    va_start(args, count);

    for (i = 0; i < count; ++i)
    {
        opers[i] = va_arg(args, LPXLOPER);
    }

    va_end(args);

    return Excel4v(xlfn, operRes, count, opers);
}

int pascal Excel4v(int xlfn, LPXLOPER operRes, int count, LPXLOPER opers[])
{
    try
    {
        switch (xlfn)
        {
        case xlfRegister:
            if (count < 4)
                return xlretInvCount;
            return Excel4Register(operRes, opers[0], opers[1], opers[2], opers[3]);
        case xlFree:
            if (count != 1)
                return xlretInvCount;
            return Excel4Free(opers[0]);
        case xlGetName:
            if (count != 0)
                return xlretInvCount;
            xloperSetString(operRes, xllName);
            return xlretSuccess;
        case xlcAlert:
            if (count == 1)
                return Excel4Alert(opers[0]);
            if (count == 2)
                return Excel4Alert(opers[0], opers[1]);
            return xlretInvCount;
        case xlfGetWorkspace:
            if (count == 1)
                return Excel4GetWorkspace(operRes, opers[0]);
            return xlretInvCount;
        default:
            break;
        }

        return xlretInvXlfn;
    }
    catch (...)
    {
        return xlretFailed;
    }
}

#if 0

// we don't call this function so don't bother to implement it

int pascal XLCallVer(void)
{
    return 0;
}

#endif

void FreeXLOutput(XL_OUTPUT* out)
{
    if (out && out->value)
    {
        if ((out->value->xltype & xlbitDLLFree) && out->freeFunc != NULL)
        {
            out->freeFunc(out->value);
            out->value = 0;
        }
    }
}

XL_OUTPUT CallXLFunction(const char* name, int count, LPXLOPER* opers)
{
    std::vector<LPXLOPER> vArgs;
    vArgs.reserve(count);

    for (int i = 0; i < count; ++i)
    {
        LPXLOPER arg = opers[i];
        vArgs.push_back(arg);
    }

    XL_OUTPUT output;
    output.value = NULL;
    output.freeFunc = NULL;

    std::string sName(name);
    std::map<std::string, FunctionInfo>::const_iterator iter = indexNameFunction.find(sName);
    if (iter == indexNameFunction.end())
    {
        output.value = &ErrName;
        return output;
    }

    if (iter->second.numArgs < count)
    {
        output.value = &ErrValue;
        return output;
    }

    for (int i = count; i < iter->second.numArgs; ++i)
        vArgs.push_back(&ArgMissing);

    switch (iter->second.numArgs)
    {
    case 0:
        output.value = ((xlCall0*)(iter->second.func))();
        break;
    case 1:
        output.value = ((xlCall1*)(iter->second.func))(vArgs[0]);
        break;
    default:
        output.value = &ErrValue;
        return output;
    }
    output.freeFunc = iter->second.freeFunc;

    return output;
}


#if SPI_XL_VERSION == 12

namespace {

    // converts an XLOPER12 to string but only if it is a string
    // this returns a pointer to static - you must populate std::string immediately
    const char* xloper12ToCString(LPXLOPER12 oper)
    {
        static char buf[256];
        if (oper->xltype & xltypeStr)
        {
            XCHAR* str = oper->val.str;
            size_t len = (unsigned char)str[0];

            if (len > 255)
                len = 255;

            WideCharToMultiByte(CP_ACP, 0, str + 1, len, &buf[0], len, NULL, NULL);
            buf[len] = '\0';
            return &buf[0];
        }
        return NULL;
    }

    int Excel12Register(
        LPXLOPER12 operRes,
        LPXLOPER12 xllName,
        LPXLOPER12 cFuncName,
        LPXLOPER12 argTypes,
        LPXLOPER12 xlFuncName)
    {
        const char* tmp;

        tmp = xloper12ToCString(xllName);
        if (!tmp)
            return xlretInvXloper;
        std::string sXLL(tmp);

        tmp = xloper12ToCString(cFuncName);
        if (!tmp)
            return xlretInvXloper;
        std::string sCFuncName(tmp);

        tmp = xloper12ToCString(argTypes);
        if (!tmp)
            return xlretInvXloper;
        std::string sArgTypes(tmp);

        tmp = xloper12ToCString(xlFuncName);
        if (!tmp)
            return xlretInvXloper;
        std::string sXLFuncName(tmp);

        return RegisterFunction(sXLL, sCFuncName, sArgTypes, sXLFuncName);
    }



} // end of anonymous namespace

#if 0
int _cdecl Excel12(int xlfn, LPXLOPER12 operRes, int count, ...)
{
    LPXLOPER12 opers[64];
    int i;

    if (count < 0 || count > 64)
        return xlretInvCount;

    va_list args;
    va_start(args, count);

    for (i = 0; i < count; ++i)
    {
        opers[i] = va_arg(args, LPXLOPER12);
    }

    va_end(args);

    return Excel12v(xlfn, operRes, count, opers);
}
#endif

int CallExcel12(int xlfn, int count, LPXLOPER12 opers[], LPXLOPER12 operRes)
{
    switch (xlfn)
    {
    case xlfRegister:
        if (count < 4)
            return xlretInvCount;
        return Excel12Register(operRes, opers[0], opers[1], opers[2], opers[3]);
    default:
        break;
    }

    return xlretInvXlfn;

}

#endif
