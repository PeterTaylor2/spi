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
#ifndef SPI_CLOCK_UTIL_HPP
#define SPI_CLOCK_UTIL_HPP

/**
***************************************************************************
** ClockUtil.hpp
***************************************************************************
** Low level functions for examining the system clock for the purpose
** of accurate timing of functions.
***************************************************************************
*/

#include "DeclSpec.h"
#include "Namespace.hpp"

#include <map>
#include <string>
#include <vector>

SPI_UTIL_NAMESPACE

SPI_UTIL_IMPORT
void Sleep(int ms);

class SPI_UTIL_IMPORT Clock
{
public:
    Clock();
    void Start();
    double Time();
private:
    double m_start;
    static double g_scaling;
};

/*
 * For low-level logging purposes call ClockEventsStart to start keeping track
 * of time.
 *
 * When you have an event that you want to log then call ClockEventsLog.
 *
 * When you are done call ClockEventsWrite which will flush the entries to file.
 */
SPI_UTIL_IMPORT
void ClockEventsStart();

/*
 * Call this to log an event after you have called ClockEventsStart.
 * If ClockEventsStart has not been called then this function does nothing.
 * Note that msg must be static data as it is only consumed later.
 */
SPI_UTIL_IMPORT
double ClockEventsLog(const char* msg, double extraTime);

/*
 * Call this to write all the events to the given file.
 * If ClockEventsStart has not been called then this function does nothing.
 */
SPI_UTIL_IMPORT
void ClockEventsWrite(const char* filename);

struct SPI_UTIL_IMPORT Profile
{
    Profile();

    std::vector<std::string> names;
    std::vector<double> times;
    std::vector<double> fractionalTimes;
    std::vector<int> numCalls;
    std::vector<double> totalTimes;
    std::vector<double> fractionalTotalTimes;

    double totalTime;
    size_t count;

    void Clear();
    void Write(const char* filename) const;
};

class SPI_UTIL_IMPORT ClockEvents
{
public:
    ClockEvents();

    void Start();
    double Log(const char* msg, double extraTime);
    void Write(const char* filename);
    void AddClockEvent(const char* msg, double time, double totalTime);

    void GetProfile(Profile& profile) const;
    void Clear();

private:

    friend class ClockFunction;

    struct ClockEventCount
    {
        int count = 0;
        double time = 0.0;
        double totalTime = 0.0;
    };

    typedef std::map<std::string, ClockEventCount> IndexClockEventCount;
    IndexClockEventCount m_indexEvents;
    Clock m_clock;
    bool m_started;
    double m_time;
    double m_unallocatedTime;

};

/*
 * If you want to clock a function then create a ClockFunction object at the
 * start of the function.
 *
 * When the function exits the ClockFunction will go out of scope and it will
 * log the time spent in the function.
 */
class SPI_UTIL_IMPORT ClockFunction
{
public:
    ClockFunction(const char* func, ClockEvents* events = NULL);
    ~ClockFunction();
private:
    const char* m_func;
    double m_unallocatedTime;
    ClockEvents* m_events;
    ClockFunction* m_parent;
    double m_extraTime;
};

SPI_UTIL_END_NAMESPACE

#ifdef SPI_UTIL_CLOCK_EVENTS

#define SPI_UTIL_CLOCK_EVENTS_START() spi_util::ClockEventsStart()
#define SPI_UTIL_CLOCK_EVENTS_LOG(msg) spi_util::ClockEventsLog(msg)
#define SPI_UTIL_CLOCK_EVENTS_WRITE(filename) spi_util::ClockEventsWrite(filename)
#define SPI_UTIL_CLOCK_FUNCTION() spi_util::ClockFunction clock_function_(__FUNCTION__)
#define SPI_UTIL_CLOCK_BLOCK(name) spi_util::ClockFunction clock_block_(name)

#else

#define SPI_UTIL_CLOCK_EVENTS_START()
#define SPI_UTIL_CLOCK_EVENTS_LOG(msg)
#define SPI_UTIL_CLOCK_EVENTS_WRITE(filename)
#define SPI_UTIL_CLOCK_FUNCTION()
#define SPI_UTIL_CLOCK_BLOCK(name)

#endif

#endif
