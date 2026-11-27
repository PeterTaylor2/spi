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
#include <chrono>
#include <ratio>
#include <stdexcept>

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
#include <process.h>        // _getpid
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
    std::string g_serverIP;
    std::string g_serverName;
    int g_serverPort = 0;
    std::string g_dnLocal;

    using Clock = std::chrono::steady_clock;
    const Clock::time_point NEVER = Clock::time_point::max();

    std::vector<double> g_intervalMinutes;   // validated schedule
    size_t g_scheduleIndex = 0;              // next gap to use
    Clock::time_point g_scheduled = NEVER;   // next point on the original grid

    std::unordered_map<std::string, size_t> g_records;

    void ValidateSchedule(const std::vector<double>& minutes)
    {
        if (minutes.empty())
            throw std::invalid_argument("StartRecording: interval schedule must not be empty");

        for (size_t i = 0; i < minutes.size(); ++i)
        {
            double m = minutes[i];
            bool last = (i + 1 == minutes.size());

            if (!(m >= 0.0))   // also catches NaN
                throw std::invalid_argument("StartRecording: intervals must not be negative");
            if (m == 0.0 && !last)
                throw std::invalid_argument("StartRecording: a zero interval is only allowed as the last element");
            if (m > 0.0 && std::chrono::duration_cast<Clock::duration>(
                std::chrono::duration<double, std::ratio<60>>(m)).count() <= 0)
                throw std::invalid_argument("StartRecording: interval is too small");
        }
    }

    // Gap that follows the n-th report (0-based).
    // The last element repeats forever, unless it is the terminating zero.
    // Returns false when there are no more interim reports.
    bool NextGap(size_t n, Clock::duration& gap)
    {
        const size_t last = g_intervalMinutes.size() - 1;
        double m = g_intervalMinutes[n < last ? n : last];
        if (m == 0.0)
            return false;
        gap = std::chrono::duration_cast<Clock::duration>(
            std::chrono::duration<double, std::ratio<60>>(m));
        return true;
    }

    // Move along the original grid (not from 'now') until the next grid point is in the future.
    void AdvanceSchedule(Clock::time_point now)
    {
        while (g_scheduled != NEVER && g_scheduled <= now)
        {
            Clock::duration gap;
            if (NextGap(g_scheduleIndex++, gap))
                g_scheduled += gap;
            else
                g_scheduled = NEVER;
        }
    }

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
                processId = spi_util::IntegerCast<size_t>(::_getpid());
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
            // then we add the name and the count for that record

            // the five items which are the same in every row are set once
            // Insert replaces the value of an existing key without changing its
            // position, so the key order is the same as before
            spi_util::JSONMapSP jm(new spi_util::JSONMap());

            jm->Insert("user", header.userName);
            jm->Insert("computer", header.computerName);
            jm->Insert("os", header.osName);
#ifdef RECORD_PID
            jm->Insert("pid", (double)header.processId);
#endif
            jm->Insert("context", header.context);

            // there was a case in testing where there was a crash seemingly due to the multi-threading
            SPI_UTIL_LOCAL_LOCK;

            std::unique_ptr<spi_util::UDPSender> sender;
            if (!header.serverIP.empty())
            {
                try
                {
                    sender.reset(new spi_util::UDPSender(header.serverIP, header.serverPort));
                }
                catch (std::exception& e)
                {
                    std::cerr << "WriteRecords: cannot send to " << header.serverIP << ": " << e.what() << std::endl;
                }
            }

            std::ofstream os;
            if (!header.dnLocal.empty())
            {
                std::string fn = "spi-recording-" + spi::Date::Today().ToString() + ".json";
                std::string ffn = spi_util::path::join(header.dnLocal.c_str(), fn.c_str(), 0);
                os.open(ffn, std::ios::app);
                if (!os.is_open())
                    std::cerr << "WriteRecords: cannot open " << ffn << std::endl;
            }

            // the value shares the map (it holds a reference, not a copy), so it always
            // reflects the latest Insert calls - this relies on JSONValue not copying the map
            spi_util::JSONValue jsonValue(jm);

            for (auto iter = records.begin(); iter != records.end(); ++iter)
            {
                jm->Insert("function", iter->first);
                jm->Insert("count", (double)iter->second);

                if (sender)
                    sender->SendJSON(jsonValue);

                if (os.is_open())
                {
                    spi_util::JSONValueToStream(os, jsonValue, true, 0);
                    os << '\n';
                }
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
    const std::vector<double>& intervalMinutes,
    const std::string& serverName,
    int serverPort,
    const std::string& dnLocal,
    const char* version)
{
    ValidateSchedule(intervalMinutes);      // before touching any state

    g_recording = true;
    g_intervalMinutes = intervalMinutes;
    g_serverName = serverName;
    g_serverPort = serverPort;
    g_dnLocal = dnLocal;

    Clock::time_point start = Clock::now();
    g_scheduleIndex = 0;
    g_scheduled = start;
    AdvanceSchedule(start);                 // first grid point = start + first gap, or NEVER for {0}

    g_records.clear();

    if (spi_util::IsIPv4Address(g_serverName))
        g_serverIP = g_serverName;
    else
        g_serverIP = spi_util::UDPGetHostByName(g_serverName);

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
        RecordHeader header;
        WriteRecords(header, g_records);
        g_records.clear();

        // thus RECORDING:STOP is always the final message
        std::ostringstream oss;
        oss << "RECORDING:STOP";
        if (version && *version)
        {
            oss << ":" << version;
        }
        AddRecord(oss.str());

        // there should be no need to recompute the header
        WriteRecords(header, g_records);
        g_records.clear();
        g_recording = false;
    }
}

void AddRecord(const std::string& name)
{
    if (g_recording)
    {
        // if we have passed a scheduled recording time then we
        // record everything we have so far
        //
        // the new record will be included in the next batch
        if (g_scheduled != NEVER)
        {
            Clock::time_point now = Clock::now();
            if (now >= g_scheduled)
            {
                // skip any grid points that we passed while idle
                AdvanceSchedule(now);

                if (!g_records.empty())
                {
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

        auto iter = g_records.find(name);
        if (iter != g_records.end())
        {
            ++(iter->second);
        }
        else
        {
            g_records.insert({ name,1 });
        }
    }
}

SPI_END_NAMESPACE

