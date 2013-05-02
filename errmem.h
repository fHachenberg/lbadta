/*
Copyright (C) 2002-2004 The TwinE team
Copyright (C) 2013 The LBADTA team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/


#ifndef H_
#define H_

#include <stddef.h>

/**
 * Describes errors
 *
 * This structure should be used as a black box and alle access should be done via the
 * responsible functions. In that way it is possible to expand the functionality of
 * this interface over time to - e.g. - a list of errors
 */
struct ErrorMemory
{
	const char* last_error_str;  	//0 == no error
	int last_error_code;  			//0 == no error

	char* error_str_buf;
	size_t error_str_bufsize;			//INCLUDING the terminating zero!
};

/**
 * Checks whether an error occured
 * @param error valid pointer to error object
 * @return no error present: 0, error present: 1
 */
int isErrorPresent(struct ErrorMemory *error);

/**
 * Initializes an error object as "no error present"
 * IF THIS OPERATION FAILS THE PROGRAM IS STOPPED
 * @param error valid pointer to error object
 * @param err_str_buf pointer to static buffer to be used by the error memory object
 * @param err_str_bufsize size of the buffer INCLUDING the terminating zero (!!!!!!)
 */
void initErrorMemory(struct ErrorMemory *error, char* err_str_buf, size_t err_str_bufsize);

/**
 * Returns the string of the last error occured
 * @param error valid pointer to error object
 * @return pointer to zero-terminated string
 */
const char *getLastErrorString(struct ErrorMemory *error);

/**
 * Returns the code of the last error occured
 * @param error valid pointer to error object
 * @return the error code
 */
int getLastErrorCode(struct ErrorMemory *error);

/**
 * Checks whether this is a valid error memory object
 * IF NO THE PROGRAM IS STOPPED
 * @param error
 */
void checkErrorMemory(struct ErrorMemory *error);

/**
 * Fills the error memory object with the new error and returns the passed in error_code
 * This is a variadic function (like printf)
 * @param error valid pointer to error object
 * @param error_code mustn't be 0
 * @param formatstr a printf-like format string
 * @return error_code is simply forwarded as return value
 */
int returnError(struct ErrorMemory *error, int error_code, const char *formatstr, ...);

#endif /* H_ */
