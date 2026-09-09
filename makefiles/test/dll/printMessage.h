#ifndef INC_PRINT_MESSAGE_H
#define INC_PRINT_MESSAGE_H

// this is the tedious stuff for import/export
// platform specific so in a normal case this is moved to a separate file

#ifdef _MSC_VER

#ifdef DLLEXPORT
#define DLLIMPORT __declspec(dllexport)
#else
#define DLLIMPORT __declspec(dllimport)
#endif

#elif defined(__GNUC__)

#ifdef DLLEXPORT
#define DLLIMPORT __attribute__((visibility("default")))
#else
#define DLLIMPORT
#endif

#else

/* neither microsoft compiler nor gcc/g++ */
#define DLLIMPORT

#endif 

DLLIMPORT void printMessage(char* msg);

#endif


