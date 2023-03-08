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
#include "ClockUtil.hpp"
#include "StringUtil.hpp"

#ifdef _MSC_VER

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// Windows implementation

SPI_UTIL_NAMESPACE

void Sleep(int ms)
{
    ::Sleep(ms);
}

double Clock::g_scaling = 0.0;

Clock::Clock()
:
m_start(0)
{}

void Clock::Start()
{
    if (g_scaling == 0.0)
    {
        LARGE_INTEGER frequency;
        QueryPerformanceFrequency(&frequency);
        g_scaling = (double)(frequency.QuadPart);
    }
    LARGE_INTEGER startTime;
    QueryPerformanceCounter(&startTime);
    m_start = (double)(startTime.QuadPart);
}

double Clock::Time()
{
    LARGE_INTEGER endTime;
    QueryPerformanceCounter(&endTime);
    double elapsed = (double)(endTime.QuadPart) - m_start;
    return elapsed / g_scaling;
}

SPI_UTIL_END_NAMESPACE

#else

#include <time.h>
#include <unistd.h>

SPI_UTIL_NAMESPACE

void Sleep(int ms)
{
    usleep(ms * 1000);
}

double Clock::g_scaling = CLOCKS_PER_SEC;

Clock::Clock()
:
m_start(0)
{}

void Clock::Start()
{
    m_start = (double)clock();
}

double Clock::Time()
{
    double elapsed = (double)clock() - m_start;
    return elapsed / g_scaling;
}

SPI_UTIL_END_NAMESPACE

#endif


// non-platform specific code
#include "Utils.hpp"

#include <fstream>
#include <map>
#include <string>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

SPI_UTIL_NAMESPACE

namespace {

    static ClockEvents g_clockEvents;

} // end of anonymous namespace

/*
 * For low-level logging purposes call ClockStart to start keeping track
 * of time.
 *
 * When you have an event that you want to log then call ClockLog.
 *
 * When you are done call ClockWrite which will flush the entries to file.
 */

void ClockEventsStart()
{
    g_clockEvents.Start();
}

/*
 * Call this to log an event after you have called ClockStart.
 * If ClockStart has not been called then this function does nothing.
 * Note that msg must be static data as it is only consumed later.
 */
void ClockEventsLog(const char* msg)
{
    g_clockEvents.Log(msg);
}

/*
 * Call this to write all the events to the given file.
 * If ClockStart has not been called then this function does nothing.
 */
void ClockEventsWrite(const char* filename)
{
    g_clockEvents.Write(filename);
}

ClockEvents::ClockEvents()
    :
    m_indexEvents(),
    m_clock(),
    m_started(false),
    m_time(),
    m_unallocatedTime()
{}

void ClockEvents::Start()
{
    m_started = true;
    m_indexEvents.clear();
    m_clock.Start();
    m_unallocatedTime = 0.0;
    m_time = m_clock.Time();
}

void ClockEvents::Log(const char* msg)
{
    if (!m_started)
        return;

    double time = m_clock.Time();
    double elapsedTime = time - m_time + m_unallocatedTime;
    m_unallocatedTime = 0.0;
    AddClockEvent(msg, elapsedTime);

    // re-calculate the time to avoid the overhead of adding to the index of clock events
    m_time = m_clock.Time();
}

void ClockEvents::Write(const char* filename)
{
    if (!m_started)
        return;

    try
    {
        Profile profile;
        GetProfile(profile);

        char buf[256];

#ifdef _MSC_VER
        sprintf(buf, "C:/temp/%s", filename);
#else
        sprintf(buf, "%s/%s", getenv("HOME"), filename);
#endif

        profile.Write(buf);
    }
    catch (...)
    {
        // ignore exceptions
    }

    Clear();
}

Profile::Profile()
{
    Clear();
}

void Profile::Clear()
{
    names.clear();
    times.clear();
    fractionalTimes.clear();
    numCalls.clear();

    totalTime = 0.0;
    count = 0;
}

void Profile::Write(const char* filename) const
{
    char buf[128];

    std::ofstream ostr(filename);
    if (totalTime > 0.0)
    {
        ostr << std::endl;
        for (size_t i = 0; i < count; ++i)
        {
            sprintf(buf, "%-45s : %12.8f : %5.2f%% (%d)",
                names[i].c_str(), times[i], 100 * fractionalTimes[i],
                numCalls[i]);
            ostr << buf << std::endl;
        }
        sprintf(buf, "%45s   ============", "");
        ostr << buf << std::endl;
        sprintf(buf, "%-45s : %12.8f", "Total", totalTime);
        ostr << buf << std::endl;
    }
    ostr << std::endl;
    ostr << Timestamp() << std::endl;
    ostr.close();
}

void ClockEvents::GetProfile(Profile& profile) const
{
    profile.Clear();

    std::map<std::string, double> indexTime;
    std::map<std::string, int> indexCount;
    double totalTime = 0.0;
        
    size_t numEvents = m_indexEvents.size();

    for (IndexClockEventCount::const_iterator iter = m_indexEvents.begin();
        iter != m_indexEvents.end(); ++iter)
    {
        indexTime[iter->first] += iter->second.time;
        indexCount[iter->first] += iter->second.count;
        totalTime += iter->second.time;
    }

    for (std::map<std::string, double>::const_iterator iter = indexTime.begin();
        iter != indexTime.end(); ++iter)
    {
        // We cannot filter out the anonymous-namespace at the
        // ClockFunction level since the string might have been static
        std::string noAnonymous = StringReplace(
            iter->first, "`anonymous-namespace'", "");
        double timeFraction = totalTime > 0.0 ? iter->second / totalTime : 0.0;

        profile.names.push_back(noAnonymous);
        profile.times.push_back(iter->second);
        profile.fractionalTimes.push_back(timeFraction);
        profile.numCalls.push_back(indexCount[iter->first]);
    }
    profile.totalTime = totalTime;
    profile.count = profile.names.size();

    SPI_UTIL_POST_CONDITION(profile.count == profile.times.size());
    SPI_UTIL_POST_CONDITION(profile.count == profile.fractionalTimes.size());
    SPI_UTIL_POST_CONDITION(profile.count == profile.numCalls.size());
}

void ClockEvents::Clear()
{
    m_started = false;
    m_indexEvents.clear();
}

void ClockEvents::AddClockEvent(const char* msg, double time)
{
    // the time spent in this function is not counted by the event clock
    std::string name(msg);
    IndexClockEventCount::iterator iter = m_indexEvents.find(name);
    if (iter == m_indexEvents.end())
    {
        ClockEventCount total;
        total.count = 1;
        total.time = time;

        m_indexEvents.insert(IndexClockEventCount::value_type(name, total));
    }
    else
    {
        iter->second.count += 1;
        iter->second.time += time;
    }
}

ClockFunction::ClockFunction(const char* func, ClockEvents* events)
    :
    m_func(func),
    m_unallocatedTime(),
    m_events(events)
{
    if (!m_events)
        m_events = &g_clockEvents;

    if (m_events->m_started)
    {
        double time = m_events->m_clock.Time();
        double unallocated = time - m_events->m_time;

        m_unallocatedTime = m_events->m_unallocatedTime + unallocated;
        m_events->m_unallocatedTime = 0.0;

        m_events->m_time = m_events->m_clock.Time();
    }
}

ClockFunction::~ClockFunction()
{
    if (m_events->m_started)
    {
        m_events->Log(m_func);
        m_events->m_unallocatedTime = m_unallocatedTime;
    }
}

SPI_UTIL_END_NAMESPACE
