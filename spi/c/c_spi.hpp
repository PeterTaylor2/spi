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

#ifndef SPI_C_C_SPI_HPP
#define SPI_C_C_SPI_HPP

/**
****************************************************************************
* Header file: c_spi.hpp
*
* Private C++ classes and functions for spi-c DLL
****************************************************************************
*/

#include <spi/spi.hpp>
#include <spi_util/Utils.hpp>
#include "DeclSpec.h"

#ifdef _MSC_VER

// for the reason for this mysterious macro see
// https://stackoverflow.com/questions/78598141/first-stdmutexlock-crashes-in-application-built-with-latest-visual-studio

#if _MSC_VER < 1942
#define _DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR
#endif

#endif

#include <mutex>

#ifndef SPI_C_MULTI_THREAD

#define SPI_C_LOCK_GUARD std::lock_guard<std::recursive_mutex> _lock_guard(spi::g_lock_guard)

#else

#define SPI_C_LOCK_GUARD

#endif

SPI_BEGIN_NAMESPACE

SPI_C_IMPORT
extern std::recursive_mutex g_lock_guard;

// returns the raw pointer from an intrusive shared pointer with the reference count
// incremented by one - hence the consumer of the raw pointer is responsible for
// deleting it
//
// we always need to increment before returning since the shared pointer in the C++
// code might be about to go out of scope (although this is not always true)
template<class T>
T* RawPointer(const spi_boost::intrusive_ptr<T>& o)
{
    SPI_C_LOCK_GUARD;
    T* p = o.get();
    intrusive_ptr_add_ref(p);
    return p;
}

template<class T>
const T* RawPointer(const spi::ObjectSmartPtr<T>& o)
{
    SPI_C_LOCK_GUARD;
    const T* p = o.get();
    intrusive_ptr_add_ref(p);
    return p;
}

template<class T, typename U>
typename T::outer_type convert_in(const U* p)
{
    SPI_C_LOCK_GUARD;
    if (!p)
        return typename T::outer_type();
    return typename T::outer_type((T*)p);
}

template<typename U, class T>
U* convert_out(const T& o)
{
    SPI_C_LOCK_GUARD;
    return (U*)(spi::RawPointer(o));
}


// PROBLEM: avoid multiplicity of access methods for vectors of shared pointers
//
// We undoubtedly need Vector_delete and Vector_new for each individual class
// however we wish to avoid having all the access methods (item, set_item, size)
// for each class in the C-interface
//
// So the idea is that we declare the function in C#:
// 
// spi_Instance_Vector_item(IntPtr v, int i, out IntPtr item);
// 
// which in reality in C is spi_Instance_Vector_item(void* v, int i, void** item);
// 
// where we might have some arbitrary type spi_Instance instead of void
// 
// So we cast spi_Instance_Vector* to (std::vector<shared_ptr<spi::Instance>>)*
// 
// This is OK as long as shared_ptr<Instance> is the same size as
// shared_ptr<T> for arbitrary type and unknown type T. Since the shared pointer
// template class just contains a single instance of T* that should be OK.
// 
// Then the tricky bit is that we need to increment the reference count.
// Now what we need is that the reference count field appears in the same place
// for any type T and Instance. In which case we can increment the
// reference count simply by using Instance.
// 
// Now we have returned a pointer back through the interface by using the RawPointer
// function above. As long as the C# code then puts this pointer back inside the
// correct class in C# (at which point all it knows is that has a pointer) then this
// pointer can be successfully handed back via the C-interface and all should work.
// 
// What about the other way around? We have an Array of T in C# and we want to create
// a vector of T in C++? We call the named constructor function (T_Vector_new) which
// creates a vector of empty shared pointers. Then we repeatedly call
// spi_Instance_Vector_set_item(void* v, int i, void* item) declared in C# as
// spi_Instance_Vector_set_item(IntPtr v, int i, IntPtr item)
// 
// The claim again is that we can safely cast v to as pointer to vector of shared pointers
// to Instance and we can cast item to pointer to Instance, put that
// inside a pointer to Instance (thus incrementing the reference pointer)
// and then into the vector.

SPI_DECLARE_OBJECT_CLASS(Instance);

class Instance : public RefCounter
{};

struct Enum
{
    Enum(int value) : value(value) {}
    int value;
};


/*
* Functions templates for the Vector functions for an object class (OC).
*
* OC is a class typically derived from spi::Object which defines its shared pointer type
* as OC::outer_type.
*
* VT is the artificial name of the vector in the C-interface.
* MT is the artificial name of the matrix in the C-interface.
* ST is the artificial name of the scalar in the C-interface.
*
* signs are checked by use of spi_util::IntegerCast
* bounds are checked by use of the std::vector at method
*/
template<class OC, typename VT>
void Vector_delete(VT* v)
{
    SPI_C_LOCK_GUARD;
    if (v)
    {
        delete ((std::vector < typename OC::outer_type>*)(v));
    }
}

template<class OC, typename VT>
VT* Vector_new(int N)
{
    SPI_C_LOCK_GUARD;
    try
    {
        return (VT*)(new std::vector<typename OC::outer_type>(spi_util::IntegerCast<size_t>(N)));
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return nullptr;
    }
}

template<class OC, typename VT, typename ST>
int Vector_item(VT* v, int i, ST** item)
{
    SPI_C_LOCK_GUARD;
    if (!v || !item)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }
    try
    {
        typename OC::outer_type o = ((std::vector<typename OC::outer_type>*)v)->at(spi_util::IntegerCast<size_t>(i));
        *item = (ST*)(RawPointer(o));
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

template<class OC, typename VT, typename ST>
int Vector_set_item(VT* v, int i, ST* item)
{
    SPI_C_LOCK_GUARD;
    if (!v)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }
    try
    {
        ((std::vector<typename OC::outer_type>*)v)->at(spi_util::IntegerCast<size_t>(i)) =
            typename OC::outer_type((OC*)item);
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

template<class OC, typename VT>
int Vector_size(VT* v, int* size)
{
    SPI_C_LOCK_GUARD;
    if (!v || !size)
    {
        spi_Error_set_function(__FUNCTION__, "NULL inputs");
        return -1;
    }
    *size = spi_util::IntegerCast<int>(((std::vector<typename OC::outer_type>*)v)->size());
    return 0;
}

template<class OC, typename MT>
void Matrix_delete(MT* m)
{
    SPI_C_LOCK_GUARD;
    if (m)
    {
        delete (spi::MatrixData<typename OC::outer_type>*)(m);
    }
}

template<class OC, typename MT>
MT* Matrix_new(int nr, int nc)
{
    SPI_C_LOCK_GUARD;
    try
    {
        return (MT*)(new spi::MatrixData<typename OC::outer_type>(
            spi_util::IntegerCast<size_t>(nr),
            spi_util::IntegerCast<size_t>(nc)));
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return nullptr;
    }
}

template<class OC, typename MT, typename ST>
int Matrix_item(MT* m, int i, int j, ST** item)
{
    SPI_C_LOCK_GUARD;
    if (!m || !item)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }

    try
    {
        typename OC::outer_type o = ((spi::MatrixData<typename OC::outer_type>*)(m))->at(
            spi_util::IntegerCast<size_t>(i),
            spi_util::IntegerCast<size_t>(j));
        *item = (ST*)(RawPointer(o));
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

template<class OC, typename MT, typename ST>
int Matrix_set_item(MT* m, int i, int j, ST* item)
{
    SPI_C_LOCK_GUARD;
    if (!m)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }

    try
    {
        ((spi::MatrixData<typename OC::outer_type>*)m)->at(
            spi_util::IntegerCast<size_t>(i),
            spi_util::IntegerCast<size_t>(j)) = typename OC::outer_type((OC*)item);
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

template<class OC, typename MT>
int Matrix_size(MT* m, int* nr, int* nc)
{
    SPI_C_LOCK_GUARD;
    if (!nr || !nc)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }

    if (!m)
    {
        *nr = 0;
        *nc = 0;
        return 0;
    }

    try
    {
        *nr = spi_util::IntegerCast<int>(((spi::MatrixData<typename OC::outer_type>*)m)->Rows());
        *nc = spi_util::IntegerCast<int>(((spi::MatrixData<typename OC::outer_type>*)m)->Cols());
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

SPI_END_NAMESPACE

#endif


