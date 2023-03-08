#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#ifdef __cplusplus
extern "C"
{
#endif

    char* MGetTempMemory(int cByte);
    void MFreeAllTempMemory();

#ifdef __cplusplus
}
#endif

#endif


