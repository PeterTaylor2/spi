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
#include "MemUtil.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include <new>

#ifdef TRACK_MEMORY_ALLOCATED
#include <stddef.h>  // Required for size_t and ptrdiff_t and NULL
#include <stdexcept> // Required for std::length_error
#include <iostream>  // For std::cout
#include <ostream>   // For std::endl
#include <map>
#endif

// these functions are used to allocate and free memory
// they do not override new/delete
//
// #define TRACK_MEMORY_ALLOCATED to track the total amount of memory allocated
// this comes with an overhead - an extra (sizeof(double)) bytes are allocated for each allocation

SPI_UTIL_NAMESPACE

static size_t NUM_ALLOCS = 0;
static size_t MEMORY_ALLOCATED = 0;

namespace {

#ifdef TRACK_MEMORY_ALLOCATED
// the attempt to track memory by putting something prior to the returned pointer did not seem to work too well
// relied on Malloc/Free being perfectly in synch which might be hard to obtain

// Mallocator can be used as an allocator for the std::map we use to track the memmory.
// We must use malloc/free for maintaining that particular map since we may elsewhere
// have overriden new/delete operators to call Malloc and Free

// NOTE: The implementation of Mallocator was provided on a blog by the Visual C++ team
// https://devblogs.microsoft.com/cppblog/the-mallocator/
// dated August 28th, 2008

template <typename T> class Mallocator
{
public:

    typedef T * pointer;
    typedef const T * const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T value_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    T * address(T& r) const { return &r; }
    const T * address(const T& s) const { return &s; }
    size_t max_size() const {
        // The following has been carefully written to be independent of
        // the definition of size_t and to avoid signed/unsigned warnings.
        return (static_cast<size_t>(0) - static_cast<size_t>(1)) / sizeof(T);
    }

    // The following must be the same for all allocators.
    template <typename U> struct rebind {
        typedef Mallocator<U> other;
    };

    bool operator!=(const Mallocator& other) const { return !(*this == other); }
    void construct(T * const p, const T& t) const {
        void * const pv = static_cast<void *>(p);
        new (pv) T(t);
    }
    void destroy(T * const p) const; // Defined below.
    // Returns true if and only if storage allocated from *this
    // can be deallocated from other, and vice versa.
    // Always returns true for stateless allocators.
    bool operator==(const Mallocator& other) const { return true; }

    // Default constructor, copy constructor, rebinding constructor, and destructor.
    // Empty for stateless allocators.
    Mallocator() { }
    Mallocator(const Mallocator&) { }
    template <typename U> Mallocator(const Mallocator<U>&) { }
    ~Mallocator() { }

    // The following will be different for each allocator.
    T * allocate(const size_t n) const {
        // The return value of allocate(0) is unspecified.
        // Mallocator returns NULL in order to avoid depending
        // on malloc(0)’s implementation-defined behavior
        if (n == 0) { return NULL; }

        // All allocators should contain an integer overflow check.
        // The Standardization Committee recommends that std::length_error
        // be thrown in the case of integer overflow.

        if (n > max_size()) {
            throw std::length_error("Mallocator<T>::allocate() – Integer overflow.");
        }
        // Mallocator wraps malloc().
        void * const pv = malloc(n * sizeof(T));
        // Allocators should throw std::bad_alloc in the case of memory allocation failure.
        if (pv == NULL) { throw std::bad_alloc(); }
        return static_cast<T *>(pv);
    }

    void deallocate(T * const p, const size_t n) const {
        // Mallocator wraps free().
        free(p);
    }

    // The following will be the same for all allocators that ignore hints.

    template <typename U> T * allocate(const size_t n, const U * /* const hint */) const {
        return allocate(n);
    }

    // Allocators are not required to be assignable, so
    // all allocators should have a private unimplemented
    // assignment operator. Note that this will trigger the
    // off-by-default (enabled under /Wall) warning C4626
    // "assignment operator could not be generated because a
    // base class assignment operator is inaccessible" within
    // the STL headers, but that warning is useless.

private:

    Mallocator& operator=(const Mallocator&);
};

// A compiler bug causes it to believe that p->~T() doesn’t reference p.

#ifdef _MSC_VER
    #pragma warning(push)
    #pragma warning(disable: 4100) // unreferenced formal parameter
#endif


// The definition of destroy() must be the same for all allocators.

template <typename T> void Mallocator<T>::destroy(T * const p) const {
    p->~T();
}

#ifdef _MSC_VER
    #pragma warning(pop)
#endif


#endif

} // end of anonymous namespace

#ifdef TRACK_MEMORY_ALLOCATED
typedef std::map<size_t, size_t, std::less<size_t>, Mallocator<std::pair<const size_t, size_t> > > INDEX_ALLOCATION_TYPE;
static INDEX_ALLOCATION_TYPE* pINDEX_ALLOCATION = NULL;
#endif

void* Malloc(size_t N)
{
#ifdef TRACK_MEMORY_ALLOCATED
    static INDEX_ALLOCATION_TYPE INDEX_ALLOCATION;
    if (pINDEX_ALLOCATION == NULL)
        pINDEX_ALLOCATION = &INDEX_ALLOCATION;
#endif

    if (N <= 0)
        return NULL;

    void* ptr;

    ptr = malloc(N);

    if (!ptr)
        throw std::bad_alloc();

#ifdef TRACK_MEMORY_ALLOCATED
    INDEX_ALLOCATION.insert(INDEX_ALLOCATION_TYPE::value_type((size_t)ptr, N));
    MEMORY_ALLOCATED += N;
#endif

    NUM_ALLOCS += 1;

    (void)memset((char *)ptr, (size_t)0, N);

#ifdef TRACE_MEM_UTIL
#ifdef TRACK_MEMORY_ALLOCATED
    printf("%s: %lu: %lu (%p)\n", __FUNCTION__, NUM_ALLOCS, MEMORY_ALLOCATED, ptr);
#else
    printf("%s: %lu (%p)\n", __FUNCTION__, NUM_ALLOCS, ptr);
#endif
#endif

    return ptr;
}

void Free(void* ptr)
{
    if (ptr)
    {
#ifdef TRACK_MEMORY_ALLOCATED
        bool found = false;
        if (pINDEX_ALLOCATION != NULL)
        {
            INDEX_ALLOCATION_TYPE::const_iterator iter = pINDEX_ALLOCATION->find((size_t)ptr);
            if (iter != pINDEX_ALLOCATION->end())
            {
                found = true;
                size_t N = iter->second;
                MEMORY_ALLOCATED -= N;
                pINDEX_ALLOCATION->erase(iter);
            }
        }
        if (!found && NUM_ALLOCS == 1)
        {
            // the problem seems to arise when deleting the last piece of memory
            // INDEX_ALLOCATOR is created by first use of Malloc but that will happen
            // after the first piece of global memory is created (and this piece of global
            // memory is now being free'd)
            //
            // to avoid this problem we would have to create our own map which has no destructor
            // but since it seems to be happening only when NUM_ALLOCS=1 we will make do with
            // this compromise
            MEMORY_ALLOCATED = 0;
            found = true;
        }

#ifdef TRACE_MEM_UTIL
        if (!found)
        {
           printf("%s: Free'ing memory (%p) that was not allocated by spi_util::Malloc\n",
                __FUNCTION__, ptr);
        }
#endif
#endif
       --NUM_ALLOCS;

#ifdef TRACE_MEM_UTIL
#ifdef TRACK_MEMORY_ALLOCATED
        printf("%s: %lu: %lu (%p)\n", __FUNCTION__, NUM_ALLOCS, MEMORY_ALLOCATED, ptr);
#else
        printf("%s: %lu (%p)\n", __FUNCTION__, NUM_ALLOCS, ptr);
#endif
#endif
       free(ptr);
   }
}

size_t AllocsNotFreed()
{
    return NUM_ALLOCS;
}

size_t MemoryNotFreed()
{
#ifdef TRACK_MEMORY_ALLOCATED
    return MEMORY_ALLOCATED;
#else
    return NUM_ALLOCS;
#endif
}

SPI_UTIL_END_NAMESPACE
