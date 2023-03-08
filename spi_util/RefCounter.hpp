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
/**
***************************************************************************
** RefCounter.hpp
***************************************************************************
** Defines a base class which will work with spi_boost::intrusive_ptr class
** for classes where the reference count is within the class itself.
***************************************************************************
*/

#ifndef SPI_UTIL_REFCOUNTER_HPP
#define SPI_UTIL_REFCOUNTER_HPP

#include "DeclSpec.h"
#include "Namespace.hpp"

#include <stdexcept>

#include <spi_boost/intrusive_ptr.hpp>

#undef SPI_UTIL_DECLARE_RC_CLASS
#define SPI_UTIL_DECLARE_RC_CLASS(T) \
class T;\
typedef spi_boost::intrusive_ptr<T>       T##SP;\
typedef spi_boost::intrusive_ptr<T const> T##ConstSP

#undef SPI_UTIL_DECLARE_RC_STRUCT
#define SPI_UTIL_DECLARE_RC_STRUCT(T) \
struct T;\
typedef spi_boost::intrusive_ptr<T>       T##SP;\
typedef spi_boost::intrusive_ptr<T const> T##ConstSP

SPI_UTIL_NAMESPACE

/**
 * Base class for reference counted classes.
 *
 * A reference counted class can be used (with care) as a raw pointer, and
 * this gives many benefits.
 */

class SPI_UTIL_IMPORT RefCounter
{
public:
    virtual ~RefCounter() {}

    long getRefCount() const { return m_refCount; }
    long incRefCount() const { return ++m_refCount; }
    long decRefCount() const { return --m_refCount; }

    friend void intrusive_ptr_add_ref(RefCounter *p)
    {
        if (p)
            p->incRefCount();
    }

    friend void intrusive_ptr_release(RefCounter *p)
    {
        if (p && p->decRefCount() == 0)
            delete p;
    }

    friend void intrusive_ptr_add_ref(const RefCounter *p)
    {
        if (p)
            p->incRefCount();
    }

    friend void intrusive_ptr_release(const RefCounter *p)
    {
        if (p && p->decRefCount() == 0)
            delete p;
    }


protected:
    /** On initialisation m_refCount=0. This is because we expect the first
        action to be to wrap the RefCounter inside an intrusive
        pointer. */
    RefCounter() : m_refCount(0) {}

    RefCounter(const RefCounter &src) { m_refCount = 0; }

private:
    // m_refCount is declared as mutable to allow const operations on
    // an instance of RefCounter
    //
    // incrementing and decrementing the reference count are not really
    // considered to be significant changes of an instance of RefCounter
    mutable long m_refCount;
};

/**
 * If you are inside a class and want to invoke a function which uses
 * an intrusive_ptr of that class, then you can use the share_this
 * template.
 *
 * In general this is not possible for a non-intrusive pointer.
 *
 * We need to check that the reference count is greater than zero.
 * Otherwise we are probably dealing with a pointer that was not created
 * on the heap.
 */
template<class T>
spi_boost::intrusive_ptr<T> share_this(T* ptr)
{
    if (ptr->getRefCount() > 0)
        return spi_boost::intrusive_ptr<T>(ptr);
    throw std::runtime_error("Attempt to share non-allocated pointer.");
}

template<class T>
spi_boost::intrusive_ptr<T const> share_this(T const* ptr)
{
    if (ptr->getRefCount() > 0)
        return spi_boost::intrusive_ptr<T const>(ptr);
    throw std::runtime_error("Attempt to share non-allocated pointer.");
}

/**
 * If you have a raw pointer for which you are maintaining the reference
 * counter independently, then you can use these template functions.
 */
template<class T>
T* incRefCount(T* ptr)
{
    if (ptr)
        ptr->incRefCount();
    return ptr;
}

template<class T>
T const* incRefCount (T const* ptr)
{
    if (ptr)
        ptr->incRefCount();
    return ptr;
}

template<class T>
T* decRefCount (T* ptr)
{
    if (ptr && ptr->decRefCount() == 0)
    {
        delete ptr;
        ptr = NULL;
    }
    return ptr;
}

template<class T>
T const* decRefCount (T const* ptr)
{
    if (ptr && ptr->decRefCount() == 0)
    {
        delete ptr;
        ptr = NULL;
    }
    return ptr;
}

SPI_UTIL_END_NAMESPACE

#endif


