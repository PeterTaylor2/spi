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
class SPI_UTIL_IMPORT FunctionOfX
{
public:
    virtual ~FunctionOfX() {}
    virtual double operator()(double x) const = 0;
};

/**
 * Finds the root of f(x) = target using a combination of secant, bisection
 * and an inverse quadratic interpolation method.
 */
SPI_UTIL_IMPORT
double RootFindBrent(
    const FunctionOfX& function,
    double target, // we seek x such that function(x) = target
    double guess, // initial guess of solution
    double boundLo, // lower bound of solution
    double boundHi, // upper bound of solution
    int    numIterations,
    double xAccuracy, // accuracy of x
    double fAccuracy, // accuracy of f(x)
    double initialXStep = 0.0, // optional - initial step
    double initialFDeriv = 0.0); // optional - derivative at the guess

SPI_UTIL_END_NAMESPACE

#endif


