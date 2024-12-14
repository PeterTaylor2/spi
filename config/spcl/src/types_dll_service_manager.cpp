/*

    Sartorial Programming Interface (SPI) code generators
    Copyright (C) 2012-2023 Sartorial Programming Ltd.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

/**
****************************************************************************
* Source file: types_dll_service_manager.cpp
****************************************************************************
*/

/*
****************************************************************************
* Generated code - do not edit
****************************************************************************
*/

#include "types_dll_service_manager.hpp"
#include "types_dll_time_out.hpp"

#include "types_types_helper.hpp"

TYPES_BEGIN_NAMESPACE

static char g_startup_directory[2048] = "";
static spi::ServiceSP g_service;
static const bool* g_is_logging = NULL;
static spi::Date g_time_out;
static bool g_timed_out = false;
static std::string g_time_out_error;

static spi::ServiceSP MakeService()
{
    spi::ServiceSP svc = spi::Service::Make("types", "types", "0.0.0.1");
    g_is_logging = svc->is_logging_flag();

    types_register_object_types(svc);

    return svc;
}

static void types_init()
{
    if (!g_service)
    {
        g_service = MakeService();
    }
}

spi::ServiceSP types_start_service()
{
    types_init();
    g_service->start_up();
    return g_service;
}

void types_stop_service()
{
    if (g_service)
        g_service->shut_down();
}

spi::ServiceSP types_exported_service()
{
    types_init();
    return g_service;
}

spi::Service* types_service()
{
    types_init();
    return g_service.get();
}

const char* types_version()
{
    return "0.0.0.1";
}

bool types_begin_function(bool noLogging)
{
    types_check_permission();
    bool isLogging(false);
    if (g_is_logging && *g_is_logging)
    {
        int log_level = spi::IncrementLogLevel();
        isLogging = !noLogging && log_level == 0;
    }
    return isLogging;
}

void types_end_function()
{
    if (g_is_logging && *g_is_logging)
        spi::DecrementLogLevel();
}


std::runtime_error types_catch_exception(bool isLogging, const char* name, std::exception& e)
{
    if (g_is_logging && *g_is_logging)
        spi::DecrementLogLevel();
    if (isLogging)
        g_service->log_error(e);
    return spi::RuntimeError(e, name);
}

std::runtime_error types_catch_all(bool isLogging, const char* name)
{
    if (g_is_logging && *g_is_logging)
        spi::DecrementLogLevel();
    std::runtime_error e("Unknown exception");
    if (isLogging)
        g_service->log_error(e);
    return spi::RuntimeError(e, name);
}

void types_start_logging(const char* filename, const char* options)
{
    g_service->start_logging(filename, options);
}

void types_stop_logging()
{
    g_service->stop_logging();
}

void types_set_time_out(spi::Date timeout, const char* msg)
{
    g_time_out = timeout;
    g_timed_out = timeout && (spi::Date::Today() > timeout);
    if (g_timed_out)
    {
        std::ostringstream oss;
        oss << "types timed out after " << g_time_out.ToString();
        if (msg)
            oss << "\n" << msg;
        g_time_out_error = oss.str();
    }
}

spi::Date types_get_time_out()
{
    return g_time_out;
}

void types_check_time_out()
{
    if (g_timed_out)
    {
        throw std::runtime_error(g_time_out_error);
    }
}

void types_check_permission()
{
    static bool g_has_permission = false;
    if (g_has_permission)
        return; // permission granted not revoked in current session
    types_check_time_out();
    g_has_permission = true;
}

TYPES_END_NAMESPACE

#ifdef SPI_EXPORT
#include <spi_util/FileUtil.hpp>
#ifdef _MSC_VER
#include <windows.h>

extern "C"
BOOL WINAPI DllMain(
    HINSTANCE hDLL,
    DWORD callReason,
    LPVOID reserved)
{
    switch(callReason)
    {
    case DLL_PROCESS_ATTACH:
        try
        {
            char buf[2048];
            DWORD size = 2048;
            if (GetModuleFileName(hDLL, buf, size) == size)
                throw std::runtime_error("Buffer overflow");
            std::string dirname = spi_util::path::dirname(buf);
            strcpy(&types::g_startup_directory[0], dirname.c_str());
        }
        catch (...)
        {
            return FALSE;
        }
        return TRUE;
    case DLL_PROCESS_DETACH:
        try
        {
            types::types_stop_service();
        }
        catch (...)
        {
            return FALSE;
        }
        return TRUE;
    }
    return TRUE;
}

#endif

#ifdef __GNUC__

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#ifdef __CYGWIN__
#undef __GNU_VISIBLE
#define __GNU_VISIBLE 1
#endif

#include <dlfcn.h>

extern "C"
void __attribute__((constructor))
types_types_on_attach(void)
{
    try
    {
        Dl_info dl_info;
        if (dladdr((const void*)types_types_on_attach, &dl_info) != 0)
        {
            std::string dirname = spi_util::path::abspath(spi_util::path::dirname(dl_info.dli_fname));
            strcpy(&types::g_startup_directory[0], dirname.c_str());
        }
    }
    catch (std::exception& e)
    {
        fprintf(stderr, "Error on attaching shared library: %s\n", e.what());
    }
}

extern "C"
void __attribute__((destructor))
types_types_on_detach(void)
{
    try
    {
        types::types_stop_service();
    }
    catch (std::exception& e)
    {
        fprintf(stderr, "Error on detaching shared library: %s\n", e.what());
    }
}

#endif

#endif /* SPI_EXPORT */


