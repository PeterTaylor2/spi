/**
***************************************************************************
** This code was published by Microsoft as a way of determining whether
** the function wizard is in use.
**
** As far as I can tell what it does is ask Windows to loop through all
** the open windows and see if any of them is an Excel function wizard
**
** Hence although you want to do this check for slow functions and it won't
** add much to the cost of a slow function while it runs, on the other
** hand with a fast function it can really destroy the performance of your
** spreadsheet.
**
** Hence it should be determined on a function-by-function basis on
** whether to detect the function wizard. Clearly with a slow function if
** you are in the function wizard it becomes painful - every key stroke
** invokes a call to the slow function.
***************************************************************************
*/

#include "xlFuncWizard.hpp"

#include "xlcall.h"
#include "framewrk.h"

#include <xlUtil.hpp>

/* This is needed for the code to detect the function wizard */
#define CLASS_NAME_BUFFER 50

typedef struct _EnumStruct
{
    BOOL  bFuncWiz;
    short hwndXLMain;
} EnumStruct, FAR * LPEnumStruct;


typedef struct _EnumStruct2
{
    bool wizard;
    DWORD pid;
} EnumStruct2, FAR* LPEnumStruct2;

#if SPI_XL_VERSION < 15
BOOL CALLBACK EnumProc(HWND hwnd, LPEnumStruct pEnum);
#endif

BOOL CALLBACK EnumProc2(HWND hwnd, LPEnumStruct2 pEnum);

/*
***************************************************************************
** FUNCTION: xlInFunctionWizard
**
** Used to detect function wizard.
***************************************************************************
*/
bool inExcelFunctionWizard()
{
#if SPI_XL_VERSION < 15
    if (spi::xlMajorVersion() < 15)
    {
        XLOPER xHwndMain;
        EnumStruct enm;

        if (Excel(xlGetHwnd, &xHwndMain, 0) == xlretSuccess)
        {
            enm.bFuncWiz = FALSE;
            enm.hwndXLMain = xHwndMain.val.w;
            EnumWindows((WNDENUMPROC)EnumProc,
                (LPARAM)((LPEnumStruct)&enm));

            return enm.bFuncWiz != FALSE;
        }
    }
    else
#endif
    {
        EnumStruct2 enm;
        enm.wizard = false;
        enm.pid = GetProcessId(GetCurrentProcess());
        EnumWindows((WNDENUMPROC)EnumProc2, (LPARAM)((LPEnumStruct)&enm));
        return enm.wizard;
    }
    return false;    /* safe case: Return false if not sure */
}

#if SPI_XL_VERSION < 15
/*
***************************************************************************
** FUNCTION: EnumProc
**
** Used by ixlInFunctionWizard (for Excel version 2010 and earlier).
***************************************************************************
*/
BOOL CALLBACK EnumProc(HWND hwnd, LPEnumStruct pEnum)
{
    /*
    ** First check the class of the window.  Will be szXLDialogClass
    ** if function wizard dialog is up in Excel
    */
    char rgsz[CLASS_NAME_BUFFER];
    GetClassName(hwnd, (LPSTR)rgsz, CLASS_NAME_BUFFER);
    if (2 == CompareString(MAKELCID(MAKELANGID(LANG_ENGLISH,
        SUBLANG_ENGLISH_US),SORT_DEFAULT), NORM_IGNORECASE,
        (LPSTR)rgsz,  (lstrlen((LPSTR)rgsz)>lstrlen("bosa_sdm_XL"))
        ? lstrlen("bosa_sdm_XL"):-1, "bosa_sdm_XL", -1))
    {
        if(LOWORD((DWORD) GetParent(hwnd)) == pEnum->hwndXLMain)
        {
            pEnum->bFuncWiz = TRUE;
            return FALSE;
        }
    }
    /* no luck - continue the enumeration */
    return TRUE;
}
#endif

/*
***************************************************************************
** FUNCTION: EnumProc2
**
** Used by ixlInFunctionWizard (for Excel version 2013 and later).
***************************************************************************
*/
BOOL CALLBACK EnumProc2(HWND hwnd, LPEnumStruct2 pEnum)
{
    static const char szFunctionWizardClass[] = "bosa_sdm_XL";
    static const char szFunctionWizardCaption[] = "Function Arguments";

    char szClass[sizeof(szFunctionWizardClass)];
    char szCaption[sizeof(szFunctionWizardCaption)];

    if (GetClassName(hwnd, (LPSTR)szClass, sizeof(szFunctionWizardClass))) 
    {
        if (CompareString(
                MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT),
                NORM_IGNORECASE,
                (LPSTR)szClass,
                (lstrlen((LPSTR)szClass) > lstrlen(szFunctionWizardClass)) ? 
                    lstrlen(szFunctionWizardClass) : -1,
                szFunctionWizardClass, -1) == CSTR_EQUAL)
        {
            // Do the process IDs match? (The former way of checking parent windows doesn't work in Excel2013).
            DWORD pid = NULL;
            GetWindowThreadProcessId(hwnd, &pid);
            if (pid == pEnum->pid) 
            {
                // Check the window caption
                if (::GetWindowText(hwnd, szCaption, sizeof(szFunctionWizardCaption))) 
                {
                    if (CompareString(MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT),
                        NORM_IGNORECASE, (LPSTR)szCaption,
                        (lstrlen((LPSTR)szCaption) > lstrlen(szFunctionWizardCaption)) ? lstrlen(szFunctionWizardCaption) : -1,
                        szFunctionWizardCaption, -1) == CSTR_EQUAL) 
                    {
                        pEnum->wizard = true;
                        return FALSE;
                    }
                }
            }
        }
    }
    // Continue the enumeration
    return TRUE;
}

