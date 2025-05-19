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
****************************************************************************
* Source file: spdoc_dll_service_manager.cpp
****************************************************************************
*/

#include "spdoc_dll_service_manager.hpp"
#include "spdoc_dll_time_out.hpp"
#include <spi/spdoc_dll_service.hpp>
#include <spi_util/FileUtil.hpp>

#include "spdoc_publicType_helper.hpp"
#include "spdoc_constant_helper.hpp"
#include "spdoc_configTypes_helper.hpp"

SPDOC_BEGIN_NAMESPACE

static char g_startup_directory[2048] = "";
static spi::ServiceSP g_service;
static const bool* g_is_logging = NULL;
static spi::Date g_time_out;
static bool g_timed_out = false;
static std::string g_time_out_error;

static spi::ServiceSP MakeService()
{
    spi::ServiceSP svc = spi::Service::Make("spdoc", "spdoc", "1.0.0.0");
    g_is_logging = svc->is_logging_flag();

    publicType_register_object_types(svc);
    constant_register_object_types(svc);
    configTypes_register_object_types(svc);

    return svc;
}

static void spdoc_init()
{
    if (!g_service)
    {
        g_service = MakeService();
    }
}

spi::ServiceSP spdoc_start_service()
{
    spdoc_init();
    g_service->start_up();
    return g_service;
}

void spdoc_stop_service()
{
    if (g_service)
        g_service->shut_down();
}

spi::ServiceSP spdoc_exported_service()
{
    spdoc_init();
    return g_service;
}

spi::Service* spdoc_service()
{
    spdoc_init();
    return g_service.get();
}

const char* spdoc_version()
{
    return "1.0.0.0";
}

bool spdoc_begin_function(bool noLogging)
{
    spdoc_check_permission();
    bool isLogging(false);
    if (g_is_logging && *g_is_logging)
    {
        int log_level = spi::IncrementLogLevel();
        isLogging = !noLogging && log_level == 0;
    }
    return isLogging;
}

void spdoc_end_function()
{
    if (g_is_logging && *g_is_logging)
        spi::DecrementLogLevel();
}


std::runtime_error spdoc_catch_exception(bool isLogging, const char* name, std::exception& e)
{
    if (g_is_logging && *g_is_logging)
        spi::DecrementLogLevel();
    if (isLogging)
        g_service->log_error(e);
    return spi::RuntimeError(e, name);
}

std::runtime_error spdoc_catch_all(bool isLogging, const char* name)
{
    if (g_is_logging && *g_is_logging)
        spi::DecrementLogLevel();
    std::runtime_error e("Unknown exception");
    if (isLogging)
        g_service->log_error(e);
    return spi::RuntimeError(e, name);
}

void spdoc_start_logging(const char* filename, const char* options)
{
    g_service->start_logging(filename, options);
}

void spdoc_stop_logging()
{
    g_service->stop_logging();
}

void spdoc_set_time_out(spi::Date timeout, const char* msg)
{
    g_time_out = timeout;
    g_timed_out = timeout && (spi::Date::Today() > timeout);
    if (g_timed_out)
    {
        std::ostringstream oss;
        oss << "spdoc timed out after " << g_time_out.ToString();
        if (msg)
            oss << "\n" << msg;
        g_time_out_error = oss.str();
    }
}

spi::Date spdoc_get_time_out()
{
    return g_time_out;
}

void spdoc_check_time_out()
{
    if (g_timed_out)
    {
        throw std::runtime_error(g_time_out_error);
    }
}

void spdoc_check_permission()
{
    static bool g_has_permission = false;
    if (g_has_permission)
        return; // permission granted not revoked in current session
    spdoc_check_time_out();
    g_has_permission = true;
}

const char* spdoc_startup_directory()
{
    return &g_startup_directory[0];
}

spdoc::ServiceConstSP spdoc_service_doc()
{
    spdoc::spdoc_start_service();
    std::string fn = spi_util::path::join(&g_startup_directory[0],
        "spdoc.svo", 0);
    auto service_doc = spdoc::Service::from_file(fn);
    const std::vector<std::string>& satellites = spdoc_service()->satellites();
    if (satellites.size() > 0)
    {
        std::vector<spdoc::ServiceConstSP> shared_services;
        for (const auto& satellite : satellites)
        {
            fn = spi_util::path::join(&g_startup_directory[0],
                (satellite + ".svo").c_str(), 0);
            shared_services.push_back(spdoc::Service::from_file(fn));
        }
        service_doc = service_doc->CombineSharedServices(shared_services);
    }
    return service_doc;
}

SPDOC_END_NAMESPACE

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
            strcpy(&spdoc::g_startup_directory[0], dirname.c_str());
        }
        catch (...)
        {
            return FALSE;
        }
        return TRUE;
    case DLL_PROCESS_DETACH:
        try
        {
            spdoc::spdoc_stop_service();
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
spdoc_spdoc_on_attach(void)
{
    try
    {
        Dl_info dl_info;
        if (dladdr((const void*)spdoc_spdoc_on_attach, &dl_info) != 0)
        {
            std::string dirname = spi_util::path::abspath(spi_util::path::dirname(dl_info.dli_fname));
            strcpy(&spdoc::g_startup_directory[0], dirname.c_str());
        }
    }
    catch (std::exception& e)
    {
        fprintf(stderr, "Error on attaching shared library: %s\n", e.what());
    }
}

extern "C"
void __attribute__((destructor))
spdoc_spdoc_on_detach(void)
{
    try
    {
        spdoc::spdoc_stop_service();
    }
    catch (std::exception& e)
    {
        fprintf(stderr, "Error on detaching shared library: %s\n", e.what());
    }
}

#endif

#endif /* SPI_EXPORT */


