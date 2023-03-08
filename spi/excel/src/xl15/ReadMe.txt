Changes to XLCALL.CPP in the Microsoft Excel 2013 XLL Software Development Kit (SDK)
====================================================================================

There are no changes to XLCALL.CPP.

What is XLCALL.CPP?
===================

XLCALL.CPP was introduced in the Excel 2007 XLL SDK.
XLCALL.CPP contains the definitions of two Excel entry points -
Excel12 and Excel12v.

Any XLL built starting with the Excel 2007 XLL SDK that uses the Excel12 or Excel12v
entry points must include the code from XLCALL.CPP for successful compilation.

The code from XLCALL.CPP can be included in the following three ways:

1. If you are using Microsoft Visual Studio for development, add XLCALL.CPP to the project.

2. If you are using makefiles to build the XLL, add XLCALL.CPP to the list of .CPP files
to compile.

3. Include XLCALL.CPP in a C or C++ file that contains code for your XLL.
XLCALL.CPP can be included using the following statement:

#include "XLCALL.CPP"

Make sure that XLCALL.CPP is present in your include path.

The first two methods are the preferred way of including the code from XLCALL.CPP.
The third method is illustrated by the framework sample included in the Excel 2010 XLL SDK.

NOTE - It is not necessary to explicitly include the code from XLCALL.CPP if
the XLL links to frmwrk32.lib. frmwrk32.lib already contains this code.


Changes to XLCALL.H in the Microsoft Excel 2013 XLL Software Development Kit (SDK)
==================================================================================

1. Added support for new Microsoft Excel 2013 worksheet functions.


Notes for International Applications
====================================

The Excel 2013 XLL SDK does not include localized versions of XLCALL.H.
Instead, the INTLMAP.XLSX workbook which shows you how the localized
command and function names map to the English-language constants used
in calls to the Excel12, Excel12v, Excel4 and Excel4v functions.

If you have code written for early versions of Microsoft Excel using a
localized version of XLCALL.H, you can use one of the following strategies
to update the code for Microsoft Excel 2013:

1. If you do not need any of the added functionality exposed in the Excel 2013 XLL SDK,
you can continue to use the localized version of XLCALL.H that shipped with the
Excel 4.0 XLL SDK.

2. You can use the constants in the new Excel 2013 XLL SDK XLCALL.H by using the
international-mapping workbook to change localized constants in your application.
Once you have changed the constants, your code will work with future versions of XLCALL.H,
and you can use the commands and functions exposed in the Excel 2013 XLL SDK.

3. You can continue to use a localized version of XLCALL.H and use the
international-mapping workbook to determine which new constant definitions
your code requires. You can add these new definitions to your application
and use the new features. However, for increased maintainability it is recommended
that you update your code to use the Excel 2013 XLL SDK constants.
