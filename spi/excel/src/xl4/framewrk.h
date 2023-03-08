#ifndef FRAMEWRK_H
#define FRAMEWRK_H

/*
** Microsoft framework functions for interfacing with Excel.
*/

#include "xlcall.h"

#ifdef __cplusplus
extern "C"
{
#endif


/*
** Total amount of memory to allocate for all temporary XLOPERs
*/

#define MEMORYSIZE 10240


/*
** Function prototypes
*/
LPSTR    GetTempMemory(int cBytes);
void     FreeAllTempMemory(void);
int      Excel(int xlfn, LPXLOPER pxResult, int count, ...);
LPXLOPER TempNum(double d);
LPXLOPER TempStrConst(const char* str); /* raw string - no space at beginning */
LPXLOPER TempBool(int b);
LPXLOPER TempInt(short int i);
LPXLOPER TempErr(WORD i);
LPXLOPER TempMissing(void);
void     InitFramework(void);

#ifdef __cplusplus
}
#endif

#endif


