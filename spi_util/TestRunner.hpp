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
#ifndef SPI_UTIL_TESTRUNNER_HPP
#define SPI_UTIL_TESTRUNNER_HPP

#include "DeclSpec.h"
#include "Namespace.hpp"

SPI_UTIL_NAMESPACE

typedef void (TestRoutine)(void);

/**
 * Runs a number of tests with controls from the command line.
 * You define the tests in pairs with name (const char*) followed by
 * test routine (function with no inputs and no outputs).
 *
 * The options are as follows:
 *
 * -w:  Wait for input before proceeding with each test.
 *      This gives you a chance to attach a debugger.
 * -rN: Repeat tests N times (for timing purposes).
 * -t:  Run timings (future enhancement).
 *
 * If you provide no command line arguments, then all tests will be run.
 * Otherwise any command line arguments indicate the tests that you want
 * to run specifically.
 *
 * Returns the number of errors detected which can be used as the
 * return value from the test routine.
 *
 * The end of the list of arguments should be indicated by passing (char*)0
 * (not simply 0 since that will be confusing in 64-bit environment).
 */
SPI_UTIL_IMPORT
int TestRunner(int argc, char* argv[], ...);

SPI_UTIL_END_NAMESPACE

#endif // SPI_UTIL_TESTRUNNER_HPP
