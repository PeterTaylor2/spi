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
** pyObjectMap.cpp
**
** Defines mapping between a python dictionary and an IObjectMap interface.
***************************************************************************
*/

#include <spi/python/pyObjectMap.hpp>
#include <spi/python/pyObject.h>
#include <spi/python/pyObject.hpp>
#include <spi/python/pyInput.hpp>

#include <spi/python/pyValue.hpp>
#include <spi/python/pyUtil.hpp>
#include <spi/python/pyOutput.hpp>
#include <spi/python/pyService.hpp>
#include <spi/Value.hpp>
#include <spi/Object.hpp>
#include <spi/ObjectPut.hpp>
#include <spi/ObjectHelper.hpp>
#include <spi/InputContext.hpp>

SPI_BEGIN_NAMESPACE

void pyDictToObjectMap(
    PyObject* dict,
    IObjectMap* objectMap)
{
    if (!PyDict_Check(dict))
        SPI_THROW_RUNTIME_ERROR("dict is not a Python dictionary");

    PyObject *key, *value;
    Py_ssize_t pos = 0;

    spi::Map m(objectMap->ClassName().c_str());

    while (PyDict_Next(dict, &pos, &key, &value))
    {
        std::string name = pyoToString(key);
        Value v = pyoToValue(value);
        m.SetValue(name, v);
    }

    objectMap->ImportMap(&m);
}

PyObjectSP pyObjectMapToDict(
    IObjectMap* objectMap,
    ValueToObject& valueToObject)
{
    spi::MapSP m = objectMap->ExportMap();

    PyObject* dict = PyDict_New();
    PyObjectSP output(dict);

    const std::vector<std::string>& fieldNames = m->FieldNames();
    for (auto iter = fieldNames.begin(); iter != fieldNames.end(); ++iter)
    {
        const std::string& name = *iter;
        Value value = valueToObject.ResolveObject(m->GetValue(name));

        PyObject* pyo = pyoFromValue(value);

        PyDict_SetItemString(dict, name.c_str(), pyo);
        Py_DECREF(pyo);
    }

    return output;
}

void pyObjectToMap(PyObject* self, IObjectMap* objectMap, bool publicOnly)
{
    // for this routine to work we must not only provide to_dict method in the class
    // but also to have registered the Python class with SPI

    ObjectTypeSP objectType = PythonService::FindRegisteredPythonClass(self->ob_type);

    PyInterpreterLock lock;
    PyObjectSP the_method = spi::pyoShare(PyObject_GetAttrString(self, "to_dict"));
    if (!the_method)
        spi::pyPythonExceptionHandler();

    std::vector<spi::Value> input_values;
    input_values.push_back(spi::Value(publicOnly));
    PyObjectSP method_args = pyTupleFromValueVector(input_values);

    PyObject* pyo_output = PyObject_CallObject(the_method.get(), method_args.get());
    if (!pyo_output)
        spi::pyPythonExceptionHandler();
    PyObjectSP pyo_output_sp = spi::pyoShare(pyo_output);
    objectMap->SetClassName(objectType->get_class_name());
    spi::pyDictToObjectMap(pyo_output, objectMap);
}

SPI_END_NAMESPACE
