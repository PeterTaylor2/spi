/*
***************************************************************************
* RootBrent.cpp
***************************************************************************
* Implements the Brent root solver in C++.
*
* This software was based on the ISDA CDS Standard Model
*
* ISDA CDS Standard Model
*
* Copyright (C) 2009 International Swaps and Derivatives Association, Inc.
* Developed and supported in collaboration with Markit
*
* This program is free software: you can redistribute it and/or modify it
* under the terms of the ISDA CDS Standard Model Public License.
*/

#include "../RootBrent.hpp"
#include "../RuntimeError.hpp"
#include <math.h>

SPI_UTIL_NAMESPACE

#define BEGIN_ANONYMOUS_NAMESPACE namespace {
#define END_ANONYMOUS_NAMESPACE }

BEGIN_ANONYMOUS_NAMESPACE

#define ONE_PERCENT 0.01

double CallFunction(const CFunction1& func, double x, double target)
{
    try
    {
        double y = func(x);
        return y - target;
    }
    catch (std::exception& e)
    {
        SPI_UTIL_THROW_RUNTIME_ERROR("Supplied function failed with x = " << x
            << "\nReported error: " << e.what());
    }
}


/*
***************************************************************************
** Finds the root using a combination of inverse quadratic method and
** bisection method.
***************************************************************************
*/
double brentMethod(
    const CFunction1& func,     // function to evaluate
    double      target,         // target
    int         numIterations,  // maximum number of iterations
    double      xAccuracy,      // x-accuracy tolerance
    double      fAccuracy,      // function accuracy tolerance
    double      *xPoints,       // array of three x-values
    double      *yPoints)       // array of three y-values
{
    int         j;                      /* Index */
    double      ratio;                  /* (x3-x1)/(x2-x1) */
    double      x31;                    /* x3-x1*/
    double      x21;                    /* x2-x1*/
    double      f21;                    /* f2-f1 */
    double      f31;                    /* f3-f1 */
    double      f32;                    /* f3-f2 */
    double      xm;                     /* New point found using Brent method*/
    double      fm;                     /* f(xm) */

    double      x1 = xPoints[0];        /* xN short hand for xPoints[n] */
    double      x2 = xPoints[1];
    double      x3 = xPoints[2];

    double      f1 = yPoints[0];
    double      f2 = yPoints[1];
    double      f3 = yPoints[2];

    for (j=1; j<=numIterations; j++)
    {
        /* Always want to be sure that f1 and f2 have opposite signs,
         * and f2 and f3 have the same sign.
         */
        if (f2*f1>0.0)
        {
            std::swap(x1,x3);
            std::swap(f1,f3);
        }
        f21 = f2-f1;
        f32 = f3-f2;
        f31 = f3-f1;
        x21 = x2-x1;
        x31 = x3-x1;
        /* Check whether this is suitable for interpolation. When checking
         * for f21,f31,f32 = 0, we don't use IS_ALMOST_ZERO for efficiency
         * reasons. If the objective function has been truncated to a
         * certain number of digits of accuracy, f21,f31,or f32 could be
         * (were in one case) zero. In this case we need to protect against
         * division by zero. So we use bisection instead of brent.
         */
        ratio = (x3-x1)/(x2-x1);
        if (f3*f31<ratio*f2*f21 || f21 == 0. || f31 == 0. || f32 == 0.)
        {
            /* This is not suitable, do bisection
             */
            x3 = x2;
            f3 = f2;

        }
        else
        {
            xm = x1 - (f1/f21)*x21 + ((f1*f2)/(f31*f32))*x31 -
                ((f1*f2)/(f21*f32))*x21;
            fm = CallFunction(func, xm, target);
            if (fm == 0.0 || (fabs(fm) <= fAccuracy && fabs(xm-x1) <= xAccuracy))
            {
                return xm;
            }
            /* If the new point and point1 bracket the root,
               replace point3 with the new point */
            if (fm*f1<0.0)
            {
                x3=xm;
                f3=fm;
            }
            /* If the root is not bracketed, replace point1 with new point,
               and point3 with point2 */
            else
            {
                x1=xm;
                f1=fm;
                x3=x2;
                f3=f2;
            }
        }
        x2 = 0.5*(x1+x3);
        f2 = CallFunction(func, x2, target);
        if (f2 == 0.0 || (fabs(f2) <= fAccuracy && fabs(x2 - x1) <= xAccuracy))
        {
            return x2;
        }
    }

    SPI_UTIL_THROW_RUNTIME_ERROR("Maximum number of iterations (" << numIterations << ") exceeded");
}

/*
***************************************************************************
** Uses the secant method until one of the following things happens. On the
** right, the setting of foundIt, bracketed, and the return status is given.
**
**                                      foundIt  bracketed
**                                      -------  ---------
**  1) the root is found                true     true
**  2) root is bracketed                false    true
**  3) max # steps reached              false    false
**  4) next x not in [boundLo, boundHi] false    false
**
** Note that the routine only throws an exception if the user-provided
** function throws an exception.
***************************************************************************
*/
double secantMethod(
    const CFunction1& func,     // function to evaluate
    double      target,         // target
    int         numIterations,  // maximum number of iterations
    double      xAccuracy,      // x-accuracy tolerance
    double      fAccuracy,      // function accuracy tolerance
    double      boundLo,        // lower bound on x
    double      boundHi,        // upper bound on x
    double      *xPoints,       // array of three x-values
    double      *yPoints,       // array of three y-values
    bool&       foundIt,        // If solution was found */
    bool&       bracketed)      // if root was bracketed*/
{
    int           j=numIterations;      /* Index */
    double        dx;                   /* Delta x used for secant */

    foundIt = false;           /* Until solution is found. */
    bracketed = false;         /* Until bracketed */

    while (j--)
    {
        /* Swap points so that yPoints[0] is smaller than yPoints[2] */
        if (fabs(yPoints[0])>fabs(yPoints[2]))
        {
            std::swap(xPoints[0],xPoints[2]);
            std::swap(yPoints[0],yPoints[2]);
        }

        /* Make sure that you do not divide by a very small value */
        if (fabs(yPoints[0]-yPoints[2]) <= fAccuracy)
        {
            if (yPoints[0] - yPoints[2] > 0)
            {
                dx = -yPoints[0]*(xPoints[0] - xPoints[2])/fAccuracy;
            }
            else
            {
                dx = yPoints[0]*(xPoints[0] - xPoints[2])/fAccuracy;
            }
        }
        else
        {
            dx= (xPoints[2]-xPoints[0])* yPoints[0]/(yPoints[0]-yPoints[2]);
        }
        xPoints[1] = xPoints[0] + dx;

        /* Make sure that the point is within bounds
         */
        if (xPoints[1] < boundLo || xPoints[1] > boundHi)
            return xPoints[1]; // not bracketed, not found

        yPoints[1] = CallFunction(func, xPoints[1], target);
        if (yPoints[1] == 0.0 ||
            (fabs(yPoints[1]) <= fAccuracy && fabs(xPoints[1] - xPoints[0]) <= xAccuracy))
        {
            foundIt   = true;
            bracketed = true;
            return xPoints[1];
        }

        if ((yPoints[0] < 0 && yPoints[1] < 0 && yPoints[2] < 0) ||
            (yPoints[0] > 0 && yPoints[1] > 0 && yPoints[2] > 0))
        {
            /* Swap points so that yPoints[0] is always smallest
             */
            if (fabs(yPoints[0]) > fabs(yPoints[1]))
            {
                xPoints[2] = xPoints[0];
                yPoints[2] = yPoints[0];
                xPoints[0] = xPoints[1];
                yPoints[0] = yPoints[1];
            }
            else
            {
                xPoints[2] = xPoints[1];
                yPoints[2] = yPoints[1];
            }
            continue;
        }
        else
        {
            /* Root was bracketed.
             * Swap points so that yPoints[0]*yPoints[2] < 0
             */
            if (yPoints[0]*yPoints[2] > 0)
            {
                /* Make sure that you swap so that
                 * xPoints[0]<xPoints[1]<xPoints[2]
                 */
                if (xPoints[1] < xPoints[0])
                {
                    std::swap(xPoints[0], xPoints[1]);
                    std::swap(yPoints[0], yPoints[1]);
                }
                else
                {
                    std::swap (xPoints[1], xPoints[2]);
                    std::swap (yPoints[1], yPoints[2]);
                }
            }
            /* Root was bracketed, but not found. */
            bracketed = true;
            return xPoints[1];
        }
    } /* while */

    /* Root not bracketed or found. */
    return (xPoints[0] + xPoints[2])/2.0;
}


END_ANONYMOUS_NAMESPACE

double RootFindBrent(
    const CFunction1& func,
    double target,
    double boundLo,
    double boundHi,
    int    numIterations,
    double guess,
    double xAccuracy,
    double fAccuracy,
    double initialXStep,
    double initialFDeriv)
{
  try
  {
    bool        bracketed;              /* If root was bracketed by secant */
    bool        foundIt;                /* If root was found by secant */
    double      xPoints[3];             /* Array of x values */
    double      yPoints[3];             /* Array of y values */

    xPoints[0] = guess;

    SPI_UTIL_PRE_CONDITION(boundLo < boundHi);
    SPI_UTIL_PRE_CONDITION(guess >= boundLo);
    SPI_UTIL_PRE_CONDITION(guess <= boundHi);

    yPoints[0] = CallFunction(func, xPoints[0], target);

    /* Check if guess is the root (use bounds for x-accuracy) */
    if (yPoints[0] == 0.0 ||
       (fabs(yPoints[0]) <= fAccuracy && (fabs(boundLo-xPoints[0]) <= xAccuracy ||
                                    fabs(boundHi-xPoints[0]) <= xAccuracy)))
    {
        return xPoints[0];
    }

    /*
     * The initial step will give us xPoints[2]
     *
     * The user can provide derivative at the guess - in which case our
     * initial step uses Newton approach for the first step only.
     *
     * Otherwise if the initial step is not provided we choose 1% of the
     * total range.
     *
     * We perform various obvious validations that xPoints[2] is within
     * bounds and if it isn't we adjust so that it is.
     */
    {
        double boundSpread  = boundHi - boundLo;
        if ( initialXStep == 0.0 )
        {
            initialXStep = ONE_PERCENT * boundSpread;
        }
        if (initialFDeriv == 0)
        {
           xPoints[2] = xPoints[0] + initialXStep;
        }
        else
        {
            xPoints[2] = xPoints[0] - (yPoints[0])/initialFDeriv;
        }

        if ( xPoints[2] < boundLo || xPoints[2] > boundHi )
        {
           xPoints[2] = xPoints[0] - initialXStep;
           if ( xPoints[2] < boundLo )
           {
               xPoints[2] = boundLo;
           }
           if ( xPoints[2] > boundHi )
           {
               xPoints[2] = boundHi;
           }
           if ( xPoints[2] == xPoints[0] )
           {
               if ( xPoints[2] == boundLo )
               {
                   xPoints[2] = boundLo + ONE_PERCENT * boundSpread;
               }
               else
               {
                   xPoints[2] = boundHi - ONE_PERCENT * boundSpread;
               }
           }
       }
    }

    /* Having obtained our first step evaluate the function and see if we have
     * a good solution already! */
    yPoints[2] = CallFunction(func, xPoints[2], target);
    if (yPoints[2] == 0.0 ||
       (fabs(yPoints[2]) <= fAccuracy  && fabs(xPoints[2]-xPoints[0]) <= xAccuracy))
    {
        return xPoints[2];
    }

    /*
     * OK - we did not strike lucky so we have to use the algorithm
     */

   /* Call secant method to find the root, or to get a
      third point, so that two of the three points bracket the root. */
    double solution = secantMethod (
        func, target, numIterations,
        xAccuracy, fAccuracy, boundLo, boundHi,
        xPoints, yPoints,
        foundIt, bracketed);

    if (foundIt)
        return solution;

    if (!bracketed)
    {
       /* Root was not bracketed, now try at the bounds */
       double fLo = CallFunction(func, boundLo, target);

       if (fLo == 0.0 || (fabs(fLo) <= fAccuracy && fabs(boundLo - xPoints[0]) <= xAccuracy))
       {
           return boundLo;
       }

       /* If these points bracket the root, assign points so that
          xPoints[0] < xPoints[2] */
       if (yPoints[0]*fLo < 0)
       {
           xPoints[2] = xPoints[0];
           xPoints[0] = boundLo;

           yPoints[2] = yPoints[0];
           yPoints[0] = fLo;
       }
       else
       {
           /* Root is still not bracketed, so try at the upper bound now. */
           double fHi = CallFunction(func, boundHi, target);
           if (fHi == 0.0 || (fabs(fHi) <= fAccuracy && fabs(boundHi-xPoints[0]) <= xAccuracy))
           {
               return boundHi;
           }
           if (yPoints[0]*fHi >= 0)
           {
               /* Root could not be bracketed at the bounds. */
               SPI_UTIL_THROW_RUNTIME_ERROR("Function values (" << fLo << ", " << fHi
                   << ") at bounds (" << boundLo << ", " << boundHi
                   << " imply no root exists.");
           }
            /* Points bracket the root, assign xPoints[2] to boundHi */
            xPoints[2] = boundHi;
            yPoints[2] = fHi;
       }

       /* xPoints[0] and xPoints[2] bracket the root, but we need third
          point to do Brent method. Take the midpoint. */
       xPoints[1] = 0.5*(xPoints[0]+xPoints[2]);
       yPoints[1] = CallFunction(func, xPoints[1], target);
       if (yPoints[1] == 0.0 ||
           (fabs(yPoints[1]) <= fAccuracy && fabs(xPoints[1] - xPoints[0]) <= xAccuracy))
       {
           return xPoints[1];
       }
    }

   /* xPoints is an array of three points, two of which bracket the root.
      Call brent Method now, to find the root */
    solution = brentMethod (func, target, numIterations, xAccuracy, fAccuracy,
                    xPoints, yPoints);

    return solution;
  }
  catch (std::exception& e)
  {
    SPI_UTIL_THROW_RUNTIME_ERROR("Failed.\n" << e.what());
  }
}

SPI_UTIL_END_NAMESPACE

