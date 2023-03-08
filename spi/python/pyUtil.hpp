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
/*
***************************************************************************
** pyUtil.hpp
**
** General purpose Python utility functions.
**
** Note that all functions are in the spi namespace, but also use the
** prefix py (lower case). This is to distinguish them from the Python
** C-interface which uses the prefix Py (mixed case). Otherwise there
** might be name clashes which would require us to use :: before functions
** from the Python C-interface.
***************************************************************************
*/

#ifndef SPI_PYUTIL_HPP
#define SPI_PYUTIL_HPP

#include "DeclSpec.h"
#include "Python.h"

#ifdef _MSC_VER
namespace spi_boost {
#endif
    void intrusive_ptr_add_ref(PyObject* p);
    void intrusive_ptr_release(PyObject* p);
#ifdef _MSC_VER
}
#endif


#include <spi/RefCounter.hpp>
#include <spi/Date.hpp>
#include <spi/Map.hpp>
#include <spi/Object.hpp>

#include <exception>
#include <string>
#include <vector>


typedef spi_boost::intrusive_ptr<PyObject> PyObjectSP;

#ifdef _MSC_VER
namespace spi_boost {
#endif

    inline void intrusive_ptr_add_ref(PyObject* p)
    {
        Py_XINCREF(p);
    }

    inline void intrusive_ptr_release(PyObject* p)
    {
        Py_XDECREF(p);
    }
#ifdef _MSC_VER
} 
#endif

SPI_BEGIN_NAMESPACE

/**
 * Shares a newly allocated PyObject but does not increase the reference
 * count.
 *
 * The idea is that you call this function after you have created PyObject,
 * and if you need to return it from the function as PyObject* then you
 * call the related function PyObjectRelease.
 *
 * add_ref should be false when you use pyoShare to wrap a newly created
 * PyObject - this is our most common use case hence the default value
 * is add_ref = false (contrary to normal behaviour for a shared pointer).
 *
 * add_ref should be true when you use pyoShare to wrap an existing
 * PyObject.
 */
SPI_PY_IMPORT
PyObjectSP pyoShare(PyObject* pyo, bool add_ref = false);

/**
 * Returns the underlying PyObject from the shared pointer, and increases the
 * reference count of the PyObject.
 *
 * The PyObject remains inside the PyObjectSP, and when that goes out of
 * scope the PyObject is therefore not automatically deleted.
 *
 * See also PyObjectShare.
 */
PyObject* pyoRelease(const PyObjectSP& pyo);

/**
 * When a C++ exception is detected, call this function to exit the
 * function at the top-level of the Python/C++ interface. For usage see
 * also PyException class.
 */
SPI_PY_IMPORT
PyObject* pyExceptionHandler(const char* msg);

/**
 * When a Python exception is detected, call this function to raise
 * a C++ exception.
 */
SPI_PY_IMPORT
void pyPythonExceptionHandler();

/**
 * C++ exception class that we can throw at a low-level after we have set
 * a Python error. You should only catch this at the highest level.
 * Note that this exception class is not a sub-class of std::exception.
 *
 * Thus the standard block for the highest level Python/C++ functions is
 * as follows:
 *
 * try
 * {
 *     code;
 *     return pyo;
 * }
 * catch (spi::PyException)  { return NULL; }
 * catch (std::exception &e) { return spi::pyExceptionHandler(e.what()); }
 */
class PyException
{};

/**
 * Converts a tuple object to a vector of objects.
 *
 * The outputs remain owned by the tuple, and hence you should not increment
 * or decrement the reference count of the outputs.
 *
 * If the input is not a tuple, then returns a vector of size one containing
 * the input value.
 */
std::vector<PyObject*> pyTupleToVector(PyObject* in);

/**
 * Class which obtains the interpreter lock and releases it when you go out of
 * scope.
 *
 * Needed in particular in the context of Python delegate classes where the Python
 * code is invoked by C++ code which might not be in the main thread of a Python
 * process.
 */
class SPI_PY_IMPORT PyInterpreterLock
{
public:
    PyInterpreterLock();
    ~PyInterpreterLock();

private:
    PyGILState_STATE gstate;
};

SPI_END_NAMESPACE

#endif
