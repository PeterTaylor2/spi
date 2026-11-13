/*

    Sartorial Programming Interface (SPI) runtime libraries

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

#include "../Recording.hpp"
#include "../CommonRuntime.hpp"
#include "../Date.hpp"
#include "../Map.hpp"
#include "../DateTime.hpp"

#include <spi_util/JSON.hpp>

#include <unordered_map>

#undef DEBUG_LOGGING
#ifdef DEBUG_LOGGING
#include <iostream>
#endif

#include <fstream>
#include <sstream>

#undef RECORD_IN_NEW_THREAD

#ifdef RECORD_IN_NEW_THREAD
#include <thread>
#include <spi_util/MutexLock.hpp>
#endif

#define RECORD_PID

#ifdef RECORD_PID
// for process id
#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <limits.h>
#endif
#endif

#include <ctype.h>

#include <spi_util/Utils.hpp>
#include <spi_util/StringUtil.hpp>
#include <spi_util/UserInfo.hpp>
#include <spi_util/CSV.hpp>
#include <spi_util/FileUtil.hpp>
#include <spi_util/UdpUpload.hpp>
#include <spi_util/MutexLock.hpp>

SPI_BEGIN_NAMESPACE

namespace
{
    bool g_recording = false;
    int g_intervalMinutes = 0;
    std::string g_serverIP;
    std::string g_serverName;
    int g_serverPort = 0;
    std::string g_dnLocal;
    DateTime g_nextRecordTime;

    std::unordered_map<std::string, size_t> g_records;

    struct RecordHeader
    {
        std::string serverIP;
        std::string serverName;
        int serverPort;
        std::string dnLocal;
        std::string context;
        std::string userName;
        std::string computerName;
        std::string osName;
        size_t processId;

        RecordHeader()
            :
            serverIP(g_serverIP),
            serverName(g_serverName),
            serverPort(g_serverPort),
            dnLocal(g_dnLocal),
            context(),
            userName(),
            computerName(),
            osName(),
            processId(0)
        {
            try
            {
                context = spi_util::StringJoin("|", CommonRuntime::GetContextNames());
            }
            catch (...) {}

            try
            {
                userName = spi_util::UserName();
            }
            catch (...) {}

            try
            {
                computerName = spi_util::ComputerName();
            }
            catch (...) {}

#if defined(_WIN32)
            osName = "windows";
#elif defined(__APPLE__)
            osName = "macos";
#elif defined(__linux__)
            osName = "linux";
#else
            osName = "unknown";
#endif

#ifdef RECORD_PID

#ifdef _MSC_VER
            try
            {
                processId = spi_util::IntegerCast<size_t>(::GetCurrentProcessId());
            }
            catch (...) {}
#else
            try
            {
                processId = spi_util::IntegerCast<size_t>(::getpid());
            }
            catch (...) {}
#endif

#endif
        }
    };

    void WriteRecords(
        const RecordHeader& header,
        const std::unordered_map<std::string, size_t>& records)
    {
        // note we need to be careful not to throw any exceptions since it is likely called from an atexit handler
        // this is for individual data requests - we put an overarching try...catch around the whole function to avoid any exceptions escaping from this function

        // we don't use any global variables in this function
        // they have been copied into RecordHeader header structure

        try
        {
            if (header.serverName.empty() && header.dnLocal.empty())
                return;

            if (records.empty())
                return;

#ifdef DEBUG_LOGGING
            std::cout << "Writing " << records.size() << " records to "
                << header.serverName << "(" << header.serverIP << ")" << ":" << header.serverPort << std::endl;
#endif

            // each row starts with userName, computerName, osName, context
            // then we add the type, name and the count for that record

            std::vector<spi_util::JSONValue> jsonValues;
            jsonValues.reserve(records.size());

            for (auto iter = records.begin(); iter != records.end(); ++iter)
            {
                const auto& key = iter->first;
                size_t count = iter->second;

                spi_util::JSONMapSP jm(new spi_util::JSONMap());

                jm->Insert("user", header.userName);
                jm->Insert("computer", header.computerName);
                jm->Insert("os", header.osName);
#ifdef RECORD_PID
                jm->Insert("pid", (double)header.processId);
#endif
                jm->Insert("context", header.context);
                jm->Insert("function", key);
                jm->Insert("count", (double)count);

                jsonValues.push_back(spi_util::JSONValue(jm));
            }

            // there was a case in testing where there was a crash seemingly due to the multi-threading
            SPI_UTIL_LOCAL_LOCK;

#ifndef SPI_STATIC
            if (!header.serverIP.empty())
            {
                spi_util::UDPUploadJSON(header.serverIP, header.serverPort, jsonValues);
            }
#endif

            if (!header.dnLocal.empty())
            {
                std::string fn = "spi-recording-" + spi::Date::Today().ToString() + ".json";
                std::string ffn = spi_util::path::join(header.dnLocal.c_str(), fn.c_str(), 0);

                std::ofstream os(ffn, std::ios::app);

                for (const auto& jsonValue : jsonValues)
                {
                    spi_util::JSONValueToStream(os, jsonValue, true, 0);
                    os << std::endl;
                }
                os.close();
            }
        }
        catch (std::exception& e)
        {
            std::cerr << "Exception in WriteRecords: " << e.what() << std::endl;
            return;
        }
        catch (...)
        {
            std::cerr << "Unknown exception in WriteRecords" << std::endl;
            return;
        }
    }

    void WriteAndClearRecords()
    {
        RecordHeader header;
        WriteRecords(header, g_records);
        g_records.clear();
    }

} // end of anonymous namespace

void StartRecording(
    int intervalMinutes,
    const std::string& serverName,
    int serverPort,
    const std::string& dnLocal,
    const char* version)
{
    g_recording = true;
    g_intervalMinutes = intervalMinutes;
    g_serverName = serverName;
    g_serverPort = serverPort;
    g_dnLocal = dnLocal;
    g_nextRecordTime = DateTime::Now(true);
    if (intervalMinutes > 0)
        g_nextRecordTime = g_nextRecordTime.Add(TimeDelta(0, 60 * intervalMinutes));

    g_records.clear();

#ifndef SPI_STATIC
    if (isdigit(g_serverName[0]))
        g_serverIP = g_serverName;
    else
        g_serverIP = spi_util::UDPGetHostByName(g_serverName);
#else
    g_serverIP = g_serverName;
#endif

    std::ostringstream oss;
    oss << "RECORDING:START";
    if (version && *version)
    {
        oss << ":" << version;
    }
    AddRecord(oss.str());
    WriteAndClearRecords();
}

void StopRecording(const char* version)
{
    if (g_recording)
    {
        std::ostringstream oss;
        oss << "RECORDING:STOP";
        if (version && *version)
        {
            oss << ":" << version;
        }
        AddRecord(oss.str());
        g_recording = false;
    }
    WriteAndClearRecords();
}

void AddRecord(const std::string& name)
{
    if (g_recording)
    {
        auto iter = g_records.find(name);
        if (iter != g_records.end())
        {
            ++(iter->second);
        }
        else
        {
            g_records.insert({ name,1 });
        }

        if (g_intervalMinutes > 0)
        {
            DateTime now = DateTime::Now(true);
            if (now.Before(g_nextRecordTime))
                return;

            g_nextRecordTime = now.Add(TimeDelta(0, 60 * g_intervalMinutes));

            std::unordered_map<std::string, size_t> records;
            records.swap(g_records);

            RecordHeader header;

#ifdef RECORD_IN_NEW_THREAD
            std::thread t(WriteRecords, header, std::move(records));
            t.detach();
#else
            WriteRecords(header, records);
#endif
        }
    }
}

SPI_END_NAMESPACE

