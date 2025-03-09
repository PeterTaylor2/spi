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

double Clock::g_scaling = 1.0;

Clock::Clock()
:
m_start(0)
{}

static double posix_accurate_clock_time()
{
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);

    double clock_time = (double)spec.tv_sec + (double)spec.tv_nsec/1e9;
    return clock_time;
}

void Clock::Start()
{
    m_start = posix_accurate_clock_time();
}

double Clock::Time()
{
    double elapsed = posix_accurate_clock_time() - m_start;
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

    static ClockFunction* g_parent = NULL;

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
double ClockEventsLog(const char* msg, double extraTime)
{
    return g_clockEvents.Log(msg, extraTime);
}

/*
 * Call this to write all the events to the given file.
 * If ClockStart has not been called then this function does nothing.
 */
void ClockEventsWrite(const char* filename)
{
    g_clockEvents.Write(filename);
}

ClockEvents::ClockEventCount::ClockEventCount()
    :
    count(0),
    time(0.0),
    totalTime(0.0)
{}

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

double ClockEvents::Log(const char* msg, double extraTime)
{
    if (!m_started)
        return 0.0;

    double time = m_clock.Time();
    double elapsedTime = time - m_time + m_unallocatedTime;
    double totalTime = elapsedTime + extraTime;
    m_unallocatedTime = 0.0;
    AddClockEvent(msg, elapsedTime, totalTime);

    // re-calculate the time to avoid the overhead of adding to the index of clock events
    m_time = m_clock.Time();

    return totalTime;
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
    totalTimes.clear();

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
            sprintf(buf, "%-50s : %10.6f : %5.2f%% : %10.6f : %5.2f%% (%d)",
                names[i].c_str(), times[i], 100 * fractionalTimes[i],
                totalTimes[i], 100 * fractionalTotalTimes[i], numCalls[i]);
            ostr << buf << std::endl;
        }
        sprintf(buf, "%50s   ==========", "");
        ostr << buf << std::endl;
        sprintf(buf, "%-50s : %10.6f", "Total", totalTime);
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
    std::map<std::string, double> indexTotalTime;
    double grandTotalTime = 0.0;
        
    size_t numEvents = m_indexEvents.size();

    for (IndexClockEventCount::const_iterator iter = m_indexEvents.begin();
        iter != m_indexEvents.end(); ++iter)
    {
        indexTime[iter->first] += iter->second.time;
        indexCount[iter->first] += iter->second.count;
        indexTotalTime[iter->first] += iter->second.totalTime;
        grandTotalTime += iter->second.time;
    }

    for (std::map<std::string, double>::const_iterator iter = indexTime.begin();
        iter != indexTime.end(); ++iter)
    {
        // We cannot filter out the anonymous-namespace at the
        // ClockFunction level since the string might have been static
        std::string noAnonymous = StringReplace(
            iter->first, "`anonymous-namespace'", "");
        double timeFraction = grandTotalTime > 0.0 ? iter->second / grandTotalTime : 0.0;

        double totalTime = indexTotalTime[iter->first];
        double totalTimeFraction = totalTime > 0.0 ? totalTime / grandTotalTime : 0.0;

        profile.names.push_back(noAnonymous);
        profile.times.push_back(iter->second);
        profile.fractionalTimes.push_back(timeFraction);
        profile.numCalls.push_back(indexCount[iter->first]);
        profile.totalTimes.push_back(totalTime);
        profile.fractionalTotalTimes.push_back(totalTimeFraction);
    }
    profile.totalTime = grandTotalTime;
    profile.count = profile.names.size();

    SPI_UTIL_POST_CONDITION(profile.count == profile.times.size());
    SPI_UTIL_POST_CONDITION(profile.count == profile.fractionalTimes.size());
    SPI_UTIL_POST_CONDITION(profile.count == profile.numCalls.size());
    SPI_UTIL_POST_CONDITION(profile.count == profile.totalTimes.size());
    SPI_UTIL_POST_CONDITION(profile.count == profile.fractionalTotalTimes.size());
}

void ClockEvents::Clear()
{
    m_started = false;
    m_indexEvents.clear();
}

void ClockEvents::AddClockEvent(const char* msg, double time, double totalTime)
{
    // the time spent in this function is not counted by the event clock
    std::string name(msg);
    IndexClockEventCount::iterator iter = m_indexEvents.find(name);
    if (iter == m_indexEvents.end())
    {
        ClockEventCount total;
        total.count = 1;
        total.time = time;
        total.totalTime = totalTime;

        m_indexEvents.insert(IndexClockEventCount::value_type(name, total));
    }
    else
    {
        iter->second.count += 1;
        iter->second.time += time;
        iter->second.totalTime += totalTime;
    }
}

ClockFunction::ClockFunction(const char* func, ClockEvents* events)
    :
    m_func(func),
    m_unallocatedTime(),
    m_events(events),
    m_parent(g_parent),
    m_extraTime()
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

        g_parent = this;
    }
}

ClockFunction::~ClockFunction()
{
    if (m_events->m_started)
    {
        double elapsedTime = m_events->Log(m_func, m_extraTime);
        m_events->m_unallocatedTime = m_unallocatedTime;
        g_parent = m_parent;
        if (g_parent)
            g_parent->m_extraTime += elapsedTime;
    }
}

SPI_UTIL_END_NAMESPACE
