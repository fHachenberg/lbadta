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

#ifndef HQR_H_
#define HQR_H_

#include <stdio.h>

#include "errmem.h"

/**
 * Used to pass back information about an entry in a HQR file
 */
struct HQREntry
{
	int compressed_size;
	int uncompressed_size;
	int offset;
	int is_compressed; //0: uncompressed, else:compressed
};

/**
 * Decompressed a chunk of data
 * @todo compressed_size is not tested at all yet!
 * @param error
 * @param uncompressed_size
 * @param uncompressed
 * @param compressed_size
 * @param compressed
 * @return 0: okay, else: error
 */
int hqr_uncompressData(	struct ErrorMemory* error,
		int uncompressed_size, unsigned char* uncompressed,
		int compressed_size, unsigned char* compressed);

/**
 * Reads an item from a HQR file
 * @param file 	valid pointer to FILE object. If return value is 0, the file stream is guaranteed
 * 				to be set back to the position it was when passed into the routine
 * @param index index index of the item
 * @param compressed buffer to hold compressed item. Must be large enough
 * @param compr_bufsize maximum allowed size of the compressed dta
 * @param uncompressed buffer to hold uncompressed item. Must be large enough
 * @param uncompr_bufsize maximum allowed size of the uncompressed dta
 * @return
 */
int hqr_readItem(	struct ErrorMemory* error, FILE *file, short int index,
					unsigned char *compressed, int compr_bufsize,
					unsigned char *uncompressed, int uncompr_bufsize);

/**
 * returns the uncompressed dataSize of an entry
 * @param error[in,out] valid pointer to ErrorMemory object to hold the success state of the routine
 * @param file[in] 	valid pointer to FILE object. If return value is 0, the file stream is guaranteed
 * 					to be set back to the position it was when passed into the routine
 * @param index[in] index of the item
 * @param dta[out] valid pointer to struct to hold information about entry
 * @return 0: okay, else: error
 */
int hqr_getItemInformation(struct ErrorMemory* error, FILE *file, int index, struct HQREntry *dta);

#endif /* HQR_H_ */
