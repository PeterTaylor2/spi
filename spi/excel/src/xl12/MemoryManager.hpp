#ifndef MEMORY_MANAGER_HPP
#define MEMORY_MANAGER_HPP

///***************************************************************************
// File:        MemoryManager.hpp
//
// Purpose:     Class definition for the memory manager used in the framework
//              library.
// 
// Platform:    Microsoft Windows
//
///***************************************************************************

#include "MemoryPool.hpp"

//
// Total number of memory allocation pools to manage
//

#define MEMORYPOOLS 4

class MemoryManager
{
public:
    MemoryManager(void);
    ~MemoryManager(void);

    static MemoryManager* GetManager();

    LPSTR CPP_GetTempMemory(int cByte);
    void CPP_FreeAllTempMemory();

private:
    MemoryPool* CreateNewPool(DWORD dwThreadID);
    MemoryPool* GetMemoryPool(DWORD dwThreadID);
    void GrowPools();

    int m_impCur;        // Current number of pools
    int m_impMax;        // Max number of mem pools
    MemoryPool* m_rgmp;    // Storage for the memory pools
};

#endif


