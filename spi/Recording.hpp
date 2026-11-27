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
#ifndef SPI_RECORDING_HPP
#define SPI_RECORDING_HPP

/**
***************************************************************************
** Records which functions were called and which files where accessed.
***************************************************************************
*/

#include "Declare.hpp"
#include <string>
#include <vector>

SPI_BEGIN_NAMESPACE

/// <summary>
/// Call this function at start-up to define the UDP server for receiving the records,
/// and the schedule on which records are published.
/// Also define the local directory which will be used for any local log file updates.
///
/// intervalMinutes is a list of gaps (in minutes) between reports. The first report is sent
/// after the first gap, the second after the second gap, and so on. For example
/// {0.5, 1, 3, 5, 20, 30.5, 60} gives reports at 0.5, 1.5, 4.5, 9.5, 29.5 and 60 minutes
/// and then every 60 minutes after that.
///
/// All gaps must be positive, except that the last gap may be zero. A final zero means no
/// further interim reports are sent after the preceding gaps, and only the final report is
/// written at shutdown. Without a final zero, the last gap repeats indefinitely.
/// A single 0 gives shutdown-only reporting.
///
/// Scheduled reports are only sent when AddRecord is called. The new record defined by
/// AddRecord will not be included in the scheduled report since it was sent after the
/// scheduled time - as a result it will only sent out on or after the next scheduled
/// time. Hence if there has been a period of inactivity you will not get just one
/// record from the next burst of activity.
///
/// The log is always written at the end of the process.
///
/// If this function is never called then all log entries are ignored.
/// </summary>
/// <param name="intervalMinutes">gaps between reports in minutes; non-empty, positive, optionally ending with a zero</param>
/// <param name="serverName"></param>
/// <param name="serverPort"></param>
/// <param name="dnLocal"></param>
SPI_IMPORT
void StartRecording(
    const std::vector<double>& intervalMinutes,
    const std::string& serverName,
    int serverPort,
    const std::string& dnLocal,
    const char* version = nullptr);

/// <summary>
/// Single-interval form: equivalent to a one-element schedule.
/// Zero (or negative) means shutdown-only reporting, as before.
/// </summary>
inline void StartRecording(
    int intervalMinutes,
    const std::string& serverName,
    int serverPort,
    const std::string& dnLocal,
    const char* version = nullptr)
{
    StartRecording(
        std::vector<double>(1, static_cast<double>(intervalMinutes > 0 ? intervalMinutes : 0)),
        serverName, serverPort, dnLocal, version);
}

/// <summary>
/// Call this function at shut-down to write the remaining log entries.
/// </summary>
SPI_IMPORT
void StopRecording(const char* version = nullptr);

/// <summary>
/// Adds a log entry of the given type and name.
///
/// If StartLogging was never called then this function does nothing.
/// </summary>
/// <param name="name"></param>
SPI_IMPORT
void AddRecord(const std::string& name);

SPI_END_NAMESPACE

#endif
