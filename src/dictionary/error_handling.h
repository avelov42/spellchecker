/** @defgroup error_handling Module error_handling
 * Provides macro to fast error handling.
 */

/** @file
  Header of error handling module.

  @ingroup error_handling
  @author Piotr Rybicki <pr360957@students.mimuw.edu.pl>
  @date 2015-08
 */

#ifndef ERROR_HANDLING_H
#define ERROR_HANDLING_H


#define report_error(x) _report_error(x, __LINE__, __func__, __FILE__) ///<Macro used to report error in program.

///Predefined types of errors that may occur.
typedef enum
{
    MEMORY,
    FILE_READ,
    FILE_SAVE
} Error_Type;

/**
 * @brief _report_error Actuall function which is called when macro is used.
 * @param type Type of the error.
 * @param line Line of the error, provided automatically by compiler.
 * @param func Name of function which caused error, provided by compiler.
 * @param file Name of file where error occured, provided by compiler.
 * Informs about type of error, line, function and file where error occured.
 */
void _report_error(Error_Type type, int line, const char* func, const char* file);

#endif // ERROR_HANDLING_H

