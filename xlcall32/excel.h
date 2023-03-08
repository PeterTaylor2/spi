#ifndef EXCEL_H
#define EXCEL_H

/* extra functions which we will export from our copy of xlcall32.dll */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

typedef struct xloper* LPXLOPER;
typedef struct xloper12* LPXLOPER12;
typedef void (xlAutoFreeFunc)(LPXLOPER);

typedef struct xlOutput
{
    LPXLOPER value;
    xlAutoFreeFunc* freeFunc;
} XL_OUTPUT;

__declspec(dllexport)
int xllOpen(const char* xll);

__declspec(dllexport)
int xllClose(const char* xll);

__declspec(dllexport)
void FreeXLOutput(XL_OUTPUT*);

__declspec(dllexport)
XL_OUTPUT CallXLFunction(const char* name, int count, LPXLOPER* opers);

__declspec(dllexport)
int CallExcel12(int xlfn, int count, LPXLOPER12 inputs[], LPXLOPER12 output);

#ifdef __cplusplus
} /* End of extern "C" { */
#endif /* __cplusplus */

#endif /* EXCEL_H */
