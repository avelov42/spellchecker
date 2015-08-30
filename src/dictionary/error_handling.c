/** @file
 * Implementation of error_handling library.
 * @ingroup error_handling
 * @author Piotr Rybicki <pr360957@mimuw.edu.pl>
 * @date 2015-08
 */

#include <stdlib.h>
#include <stdio.h>
#include "error_handling.h"

/**
 * @brief type_to_string Converts Error_Type to string.
 * @param type Type of the error.
 * @return Pointer to const string which describes the error.
 */
static const char* type_to_string(Error_Type type)
{
    switch(type)
    {
    case MEMORY:
        return "memory";
    case FILE_READ:
        return "file-reading";
    case FILE_SAVE:
        return "file-saving";
    default:
        return "unknown";
    }
    return "42";
}

void _report_error(Error_Type type, int line, const char* func, const char* file)
{
    fprintf(stderr, "Sadly.. %s error has occured:\nat line: %d\nin function: %s\nin module: %s\nAborting program!\n",
            type_to_string(type), line, func, file);
    exit(EXIT_FAILURE); //and that causes massive mem-lack, but we dont care
}
