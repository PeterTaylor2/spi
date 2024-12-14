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
* Source file: replay_dll_service_manager.cpp
****************************************************************************
*/

#include "replay_dll_service_manager.hpp"
#include "replay_dll_time_out.hpp"

#include "spi_replay_map_helper.hpp"
#include "spi_replay_replay_helper.hpp"

SPI_REPLAY_BEGIN_NAMESPACE

static char g_startup_directory[2048] = "";
static spi::ServiceSP g_service;
static const bool* g_is_logging = NULL;
static spi::Date g_time_out;
static bool g_timed_out = false;
static std::string g_time_out_error;

static spi::ServiceSP MakeService()
{
    spi::ServiceSP svc = spi::Service::Make("replay", "spi_replay", "1.0.0.0");
    g_is_logging = svc->is_logging_flag();

    map_register_object_types(svc);
    replay_register_object_types(svc);

    return svc;
}

static void replay_init()
{
    if (!g_service)
    {
        g_service = MakeService();
    }
}

spi::ServiceSP replay_start_service()
{
    replay_init();
    g_service->start_up();
    return g_service;
}

void replay_stop_service()
{
    if (g_service)
        g_service->shut_down();
}

spi::ServiceSP replay_exported_service()
{
    replay_init();
    return g_service;
}

spi::Service* replay_service()
{
    replay_init();
    return g_service.get();
}

const char* replay_version()
{
    return "1.0.0.0";
}

bool replay_begin_function(bool noLogging)
{
    replay_check_permission();
    bool isLogging(false);
    if (g_is_logging && *g_is_logging)
    {
        int log_level = spi::IncrementLogLevel();
        isLogging = !noLogging && log_level == 0;
    }
    return isLogging;
}

void replay_end_function()
{
    if (g_is_logging && *g_is_logging)
        spi::DecrementLogLevel();
}


std::runtime_error replay_catch_exception(bool isLogging, const char* name, std::exception& e)
{
    if (g_is_logging && *g_is_logging)
        spi::DecrementLogLevel();
    if (isLogging)
        g_service->log_error(e);
    return spi::RuntimeError(e, name);
}

std::runtime_error replay_catch_all(bool isLogging, const char* name)
{
    if (g_is_logging && *g_is_logging)
        spi::DecrementLogLevel();
    std::runtime_error e("Unknown exception");
    if (isLogging)
        g_service->log_error(e);
    return spi::RuntimeError(e, name);
}

void replay_start_logging(const char* filename, const char* options)
{
    g_service->start_logging(filename, options);
}

void replay_stop_logging()
{
    g_service->stop_logging();
}

void replay_set_time_out(spi::Date timeout, const char* msg)
{
    g_time_out = timeout;
    g_timed_out = timeout && (spi::Date::Today() > timeout);
    if (g_timed_out)
    {
        std::ostringstream oss;
        oss << "replay timed out after " << g_time_out.ToString();
        if (msg)
            oss << "\n" << msg;
        g_time_out_error = oss.str();
    }
}

spi::Date replay_get_time_out()
{
    return g_time_out;
}

void replay_check_time_out()
{
    if (g_timed_out)
    {
        throw std::runtime_error(g_time_out_error);
    }
}

void replay_check_permission()
{
    static bool g_has_permission = false;
    if (g_has_permission)
        return; // permission granted not revoked in current session
    replay_check_time_out();
    g_has_permission = true;
}

SPI_REPLAY_END_NAMESPACE

#ifdef REPLAY_EXPORT
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
            strcpy(&spi_replay::g_startup_directory[0], dirname.c_str());
        }
        catch (...)
        {
            return FALSE;
        }
        return TRUE;
    case DLL_PROCESS_DETACH:
        try
        {
            spi_replay::replay_stop_service();
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
spi_replay_replay_on_attach(void)
{
    try
    {
        Dl_info dl_info;
        if (dladdr((const void*)spi_replay_replay_on_attach, &dl_info) != 0)
        {
            std::string dirname = spi_util::path::abspath(spi_util::path::dirname(dl_info.dli_fname));
            strcpy(&spi_replay::g_startup_directory[0], dirname.c_str());
        }
    }
    catch (std::exception& e)
    {
        fprintf(stderr, "Error on attaching shared library: %s\n", e.what());
    }
}

extern "C"
void __attribute__((destructor))
spi_replay_replay_on_detach(void)
{
    try
    {
        spi_replay::replay_stop_service();
    }
    catch (std::exception& e)
    {
        fprintf(stderr, "Error on detaching shared library: %s\n", e.what());
    }
}

#endif

#endif /* REPLAY_EXPORT */


