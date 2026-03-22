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

SPI_BEGIN_NAMESPACE

/// <summary>
/// Call this function at start-up to define the UDP server for receiving the records, and the number of minutes
/// which will trigger a record being published.
/// Also define the local directory which will be used for any local log file updates.
/// 
/// The log is always written at the end of the process, but can also be written after the size of the log exceeds the maxSize.
/// In that case (size of the log exceeds maxSize) then the log is cleared.
/// 
/// If this function is never called then all log entries are ignored.
/// </summary>
/// <param name="intervalMinutes"></param>
/// <param name="serverName"></param>
/// <param name="serverPort"></param>
/// <param name="dnLocal"></param>
SPI_IMPORT
void StartRecording(
    int intervalMinutes,
    const std::string& serverName,
    int serverPort,
    const std::string& dnLocal);

/// <summary>
/// Call this function at shut-down to write the remaining log entries.
/// </summary>
SPI_IMPORT
void StopRecording();

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
