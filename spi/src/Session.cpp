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
#include "Session.hpp"
#include "RuntimeError.hpp"
#include "DateTime.hpp"

#if defined(_MSC_VER) && (_MSC_VER <= 1500)
#define NO_MUTEX
#endif

#ifndef NO_MUTEX
#include <mutex>
#endif

#include <spi_util/UserInfo.hpp>
#include <spi_util/FileUtil.hpp>
#include <spi_util/JSON.hpp>

SPI_BEGIN_NAMESPACE

namespace {

    static bool g_session_logging = false;

    // the function which fetches the singleton Session must be locked
    // any function which amends the singleton Session must be locked
    // any function which copies the singleton Session must be locked
    // however no methods which act on a specific Session need to be locked
#ifndef NO_MUTEX
    std::mutex global_session_lock;
#endif

    // singleton structure Session
    struct Session
    {
        Session();

        std::set<std::string> funcNames;
        std::set<std::string> fileNames;
        std::string computerName;
        std::string userName;
        unsigned int pid;
        DateTime startTime;
        DateTime endTime;

        void add_function_name(const std::string& funcName);
        void add_file_name(const std::string& fileName);
        void end_session();

        static Session* singleton();
        static Session copy_singleton();
    };

    Session::Session()
        :
        funcNames(),
        fileNames(),
        computerName(),
        userName(),
        pid(),
        startTime(),
        endTime()
    {
        try
        {
            computerName = spi_util::ComputerName();
        }
        catch (...)
        {
            computerName = "<<UNKNOWN>>";
        }
        try
        {
            userName = spi_util::UserName();
        }
        catch (...)
        {
            userName = "<<UNKNOWN>>";
        }
        try
        {
            pid = spi_util::ProcessID();
        }
        catch (...)
        {
            pid = 0;
        }
        try
        {
            startTime = DateTime::Now();
        }
        catch (...)
        {
            // ignore error - startTime will still be zero
        }
    }

    void Session::add_function_name(const std::string& funcName)
    {
        funcNames.insert(funcName);
    }

    void Session::add_file_name(const std::string& fileName)
    {
        fileNames.insert(fileName);
    }

    void Session::end_session()
    {
        try
        {
            endTime = DateTime::Now();
        }
        catch (...)
        {
            // ignore error - endTime will still be zero
        }
    }

    Session Session::copy_singleton()
    {
#ifndef NO_MUTEX
        std::lock_guard<std::mutex> guard(global_session_lock);
#endif
        Session* s = singleton();
        return Session(*s);
    }

    Session* Session::singleton()
    {
        static Session theSingleton;
        return &theSingleton;
    }
}

void session::start_session()
{
#ifndef NO_MUTEX
    std::lock_guard<std::mutex> guard(global_session_lock);
#endif
    Session::singleton();
    g_session_logging = true;
}

bool session::session_logging()
{
    return g_session_logging;
}

void session::add_function_name(const std::string& funcName)
{
    if (g_session_logging)
    {
#ifndef NO_MUTEX
        std::lock_guard<std::mutex> guard(global_session_lock);
#endif
        Session* s = Session::singleton();
        s->add_function_name(funcName);
    }
}

void session::add_file_name(const std::string& fileName)
{
    if (g_session_logging)
    {
        std::string pfn = spi_util::path::posix(fileName);

        // we don't report on local files
        if (pfn.length() > 2 && pfn[0] == '/' && pfn[1] == '/')
        {
#ifndef NO_MUTEX
            std::lock_guard<std::mutex> guard(global_session_lock);
#endif
            Session* s = Session::singleton();
            s->add_file_name(pfn);
        }
    }
}

void session::end_session()
{
#ifndef NO_MUTEX
    std::lock_guard<std::mutex> guard(global_session_lock);
#endif
    Session* s = Session::singleton();
    s->end_session();
    g_session_logging = false;
}

void session::to_stream(std::ostream& ostr)
{
    // declare as const& so that the copied Session is not itself copied unnecessarily
    const Session& s = Session::copy_singleton();

    // now we have a copy - we can stream it at our leisure
    spi_util::JSONArraySP jFuncNames(new spi_util::JSONArray());
    spi_util::JSONArraySP jFileNames(new spi_util::JSONArray());
    spi_util::JSONValue jComputerName(s.computerName);
    spi_util::JSONValue jUserName(s.userName);
    spi_util::JSONValue jPid((double)s.pid);
    spi_util::JSONValue jStartTime(s.startTime.ToString());

    spi_util::JSONMapSP jMap(new spi_util::JSONMap());

    for (std::set<std::string>::const_iterator iter = s.funcNames.begin(); iter != s.funcNames.end(); ++iter)
        jFuncNames->Append(spi_util::JSONValue(*iter));

    for (std::set<std::string>::const_iterator iter = s.fileNames.begin(); iter != s.fileNames.end(); ++iter)
        jFileNames->Append(spi_util::JSONValue(*iter));

    jMap->Insert("computerName", jComputerName);
    jMap->Insert("userName", jUserName);
    jMap->Insert("pid", jPid);
    jMap->Insert("startTime", jStartTime);

    if (s.endTime)
    {
        jMap->Insert("endTime", spi_util::JSONValue(s.endTime.ToString()));
    }

    jMap->Insert("functions", jFuncNames);
    jMap->Insert("files", jFileNames);

    spi_util::JSONValueToStream(ostr, spi_util::JSONValue(jMap));
}

SPI_END_NAMESPACE

