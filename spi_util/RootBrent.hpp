#ifndef SPI_UTIL_ROOT_BRENT_HPP
#define SPI_UTIL_ROOT_BRENT_HPP

#include "Namespace.hpp"
#include "DeclSpec.h"

/**
***************************************************************************
** RootBrent.hpp
***************************************************************************
** Implements the Brent root solver in C++.
***************************************************************************
*/

SPI_UTIL_NAMESPACE

/**
 * Defines a function of one variable
 */
class SPI_UTIL_IMPORT CFunction1
{
public:
    virtual ~CFunction1() {}
    virtual double operator()(double x) const = 0;
};

/**
 * Finds the root of f(x) = target using a combination of secant, bisection
 * and an inverse quadratic interpolation method.
 */
SPI_UTIL_IMPORT
double RootFindBrent(
    const CFunction1& function,
    double target,
    double boundLo,
    double boundHi,
    int    numIterations,
    double guess,
    double xAccuracy,
    double fAccuracy,
    double initialXStep = 0.0,
    double initialFDeriv = 0.0);

SPI_UTIL_END_NAMESPACE

#endif


