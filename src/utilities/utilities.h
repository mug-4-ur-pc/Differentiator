/*!
 * @file
 * @bruef A header file which includes different helpful functions.
 */

#ifndef UTILITIES_H_
#define UTILITIES_H_


#include <stdio.h>
#include <stdbool.h>
#include <string.h>




#ifdef _WIN32
#	define FILE_SEP '\\'
#else
#	define FILE_SEP '/'
#endif



/*!
 * @brief Get size of array which was created on stack.
 */
#define ARRAY_SIZE(ARR_) (sizeof (ARR_) / sizeof (*(ARR_)))

/*!
 * @brief Precision which used to compare double numbers.
 */
#define DOUBLE_PRECISION ((double) 0.000001)

/*!
 * @brief Macro which includes name of source file without path.
 */
#define __FILENAME__ \
	(strrchr(__FILE__, FILE_SEP) ? strrchr(__FILE__, FILE_SEP) + 1 : __FILE__)

/*!
 * @brief Say to compiler that variable or function may be unused.
 */
#define MAYBE_UNUSED(X_) ((void) (X_))

/*!
 * @brief Print character n times.
 */
void print_n_chars
(
	char   ch,    /*!< [in]     printed character.                           */
	size_t n,     /*!< [in]     amount of printed char.                      */
	FILE*  output /*!< [in,out] output stream.                               */
);

/*!
 * @brief Read string with allocating memory for it.
 *
 * @note Don't forget to free memory.
 *
 * @return String or NULL if an error occurred.
 */
char* read_string
(
	FILE*   input, /*!< [in]  input stream.                                  */
	size_t* size   /*!< [out] pointer to variable where the read string length
	                          will be placed.                                */
);

/*!
 * @brief Allocate memory for string array,
 * split string.by delimiter and write it to array.
 *
 * @note At the end of array functions adds NULL.
 *
 * @note Don't forget to free memory.
 *
 * @return String array.
 */
char** split_by_delim
(
	const char* str,  /*!< [in] input string.                                */
	char        delim /*!< [in] delimiter.                                   */
);

/*!
 * @brief Clear buffer by reading 
 * until '\n' will be encountered.
 */
void clear_buffer
(
	FILE* stream /*!< [in,out] input stream.                                 */
);

/*!
 * @brief Check two double numbers two equality.
 *
 * @retnrn Equality of two doubles.
 */
bool double_equal
(
	double a, /*!< [in] first number.                                        */
	double b  /*!< [in] second number.                                       */
);




#endif // Undefined UTILITIES_H_
