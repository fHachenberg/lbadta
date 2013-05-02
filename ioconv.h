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

#include <endian.h>

#ifndef IOCONV_H_
#define IOCONV_H_

#include "stdint.h"

/**
 * Routines to read data from Little Endian formatting
 * @param input
 * @return
 */

unsigned int fromLE_UDW_to_uint(uint32_t input);
unsigned int fromLE_UW_to_uint(uint16_t input);
int fromLE_UDW_to_int(uint32_t input);
int fromLE_UW_to_int(uint16_t input);

#endif /* IOCONV_H_ */
