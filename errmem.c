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

#include "errmem.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

int isErrorPresent(struct ErrorMemory *error)
{
	checkErrorMemory(error);

	return error->last_error_code != 0;
}

void initErrorMemory(struct ErrorMemory *error, char* err_str_buf, size_t err_str_bufsize)
{
	if(error == 0)
	{
		printf("pointer on error object is NULL");
		exit (EXIT_FAILURE);
	}

	error->error_str_buf = err_str_buf;
	error->error_str_bufsize = err_str_bufsize;
	err_str_buf[error->error_str_bufsize-1] = 0; //enforce the last byte to be zero!

	error->last_error_code = 0; //0 == no error
	error->last_error_str = 0;
}

const char *getLastErrorString(struct ErrorMemory *error)
{
	checkErrorMemory(error);

	return error->last_error_str;
}

int getLastErrorCode(struct ErrorMemory *error)
{
	checkErrorMemory(error);

	return error->last_error_code;
}

void checkErrorMemory(struct ErrorMemory *error)
{
	if(error == 0)
	{
		printf("pointer on error object is NULL");
		exit (EXIT_FAILURE);
	}
}

int returnError(struct ErrorMemory *error, int error_code, const char *formatstr, ...)
{
	checkErrorMemory(error);

	/* Declare a va_list type variable */
	va_list myargs;

	/* Initialise the va_list variable with the ... after fmt */
	va_start(myargs, formatstr);

	/* Forward the '...' to vprintf */
	int ret = vsnprintf(error->error_str_buf, error->error_str_bufsize, formatstr, myargs);
	if(ret < 0) //something went wrong
	{
		printf("Error adding an error");
		vprintf(formatstr, myargs);
		/* Clean up the va_list */
		va_end(myargs);
		exit (EXIT_FAILURE);
	}

	error->last_error_code = error_code;
	error->last_error_str = error->error_str_buf;

	/* Clean up the va_list */
	va_end(myargs);

	return error_code;
}
