/*
Copyright (C) 2002-2004 The TwinE team
Copyright (C) 2012 The LBADTA team

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

#include <stdio.h>
#include <limits.h>
#include <stdint.h>

#include "hqr.h"
#include "ioconv.h"

/**
 * internal routine to navigate through headers and offset tables in a HQR file
 * It returns the valid data structure for the entry
 * @param error pointer MUST be valid.
 * @param file pointer MUST be valid.
 * @param index
 * @param dta[out] dta pointer MUST be valid.
 * @return 0: okay, else: error
 */
int __fillItemDta(struct ErrorMemory *error, FILE *file, int index, struct HQREntry *dta)
{
	uint32_t dta_headerSize;
	if(fread(&dta_headerSize, 4, 1, file) != 1)
	{
		return returnError(error, 1, "cannot read header size");
	}
	int headerSize = fromLE_UDW_to_int(dta_headerSize);

	if(index >= headerSize/4 || index < 0 )
	{
		return returnError(error, 1, "index %d out of bounds %d", index, headerSize/4);
	}

	if(fseek(file, index*4, SEEK_SET) != 0)
	{
		return returnError(error, 1, "cannot reach header entry. File could be truncated");
	}

	uint32_t offToData;
	if(fread(&offToData, 4, 1, file) != 1)
	{
		return returnError(error, 1, "cannot read item offset. File could be truncated");
	}
	dta->offset = fromLE_UDW_to_int(offToData);

	if(fseek(file, offToData, SEEK_SET) != 0)
	{
		return returnError(error, 1, "cannot reach item. File could be truncated");
	}

	uint32_t dataSize;
	if(fread(&dataSize, 4, 1, file) != 1)
	{
		return returnError(error, 1, "cannot read item size. File could be truncated");
	}
	dta->uncompressed_size = fromLE_UDW_to_int(dataSize);

	uint32_t compressedSize;
	if(fread(&compressedSize, 4, 1, file) != 1)
	{
		return returnError(error, 1, "cannot read compressed item size. File could be truncated");
	}
	dta->compressed_size = fromLE_UDW_to_int(compressedSize);

	const int DATA_COMPRESSED = 1;
	uint16_t mode;
	if(fread(&mode, 2, 1, file) != 1)
	{
		return returnError(error, 1, "cannot read item mode. File could be truncated");
	}
	dta->is_compressed = (mode== DATA_COMPRESSED ? 1 : 0);

	return 0;
}

int hqr_uncompressData(	struct ErrorMemory* error,
						int uncompressed_size, unsigned char* uncompressed,
						int compressed_size, unsigned char* compressed)
{
	char loop;
	unsigned char indic;
	unsigned char *readback;
	int size;
	uint16_t temp;
	int i;

	do
	{
		loop = 8;
		indic = *(compressed++); ///Read in one byte
		do
		{
			if (!(indic & 1)) /// if (first bit is not set)
			{
				temp = (uint16_t)((*(compressed+1)<<8)+(*compressed));//*((short*)compressed);
				compressed += 2;

				size = temp & 0x0F; ///transfer lowest 8 bit to size

				size += 2;
				uncompressed_size -= size;
				readback = (uncompressed - (temp >> 4)) - 1; ///Now write back from output to output

				for (i = 0; i < size; i++)
				{
					*(uncompressed++) = *(readback++);
				}
				if (uncompressed_size <= 0)
					return returnError(error, 1, "decompression ran out of output buffer");
				loop--;
			}
			else
			{
				*(uncompressed++) = *(compressed++);

				loop--;
				uncompressed_size--;
				if (uncompressed_size <= 0)
					return returnError(error, 1, "decompression ran out of output buffer");
			}
			indic >>= 1;
		}while (loop);
	}while (uncompressed_size); ///work through the whole buffer!

	return 0;
}

int hqr_readItem(	struct ErrorMemory* error, FILE *file, short int index,
					unsigned char *compressed, int compr_bufsize,
					unsigned char *uncompressed, int uncompr_bufsize)
{
	//Argument Checks
	checkErrorMemory(error);
	if(file == 0)
	{
		return returnError(error, 1, "file pointer is NULL");
	}

	//Remember stream position
	long streampos = ftell(file);

	struct HQREntry dta;
	int rgw = __fillItemDta(error, file, index, &dta);
	if(rgw != 0)
	{
		//Reset stream position
		fseek(file, streampos, SEEK_SET); //if fseek fails, we don't report it, because the primary error is more important
		return rgw;
	}

	if(uncompr_bufsize < dta.uncompressed_size)
	{
		//Reset stream position
		fseek(file, streampos, SEEK_SET); //if fseek fails, we don't report it, because the primary error is more important
		return returnError(error, 1, "require %d bytes to unpack data, but buffer has only %d bytes", dta.uncompressed_size, uncompr_bufsize);
	}

	if(dta.is_compressed)
	{
		if(compr_bufsize < dta.compressed_size)
		{
			//Reset stream position
			fseek(file, streampos, SEEK_SET); //if fseek fails, we don't report it, because the primary error is more important
			return returnError(error, 1, "buffer for compressed data requires size >= %d but only provides %d", dta.compressed_size, compr_bufsize);
		}

		if(fread(compressed, 1, (size_t)dta.compressed_size, file) != dta.compressed_size)
		{
			//Reset stream position
			fseek(file, streampos, SEEK_SET); //if fseek fails, we don't report it, because the primary error is more important
			return returnError(error, 1, "could not read %d bytes of compressed data. File could be truncated", dta.compressed_size);
		}

		int rgw = hqr_uncompressData(error, dta.uncompressed_size, uncompressed, dta.compressed_size, compressed);
		if(rgw != 0)
		{
			//Reset stream position
			fseek(file, streampos, SEEK_SET); //if fseek fails, we don't report it, because the primary error is more important
			return rgw;
		}
	}
	else //uncompressed
	{
		if(fread(uncompressed, 1, (size_t)dta.uncompressed_size, file) != dta.uncompressed_size)
		{
			//Reset stream position
			fseek(file, streampos, SEEK_SET); //if fseek fails, we don't report it, because the primary error is more important
			return returnError(error, 1, "could not read %d bytes of uncompressed data. File could be truncated", dta.uncompressed_size);
		}
	}

	//Reset stream position
	if(fseek(file, streampos, SEEK_SET) != 0)
	{
		return returnError(error, 1, "could not reset stream position");
	}

	return(0);
}

int hqr_getItemInformation(struct ErrorMemory* error, FILE *file, int index, struct HQREntry *dta)
{
	//Argument Checks
	checkErrorMemory(error);
	if(dta == 0)
	{
		return returnError(error, 1, "dta pointer is NULL");
	}
	if(file == 0)
	{
		return returnError(error, 1, "file pointer is NULL");
	}

	//Remember stream position
	long streampos = ftell(file);

	int rgw = __fillItemDta(error, file, index, dta);
	if(rgw != 0)
	{
		//Reset stream position
		fseek(file, streampos, SEEK_SET); //if fseek fails, we don't report it, because the primary error is more important
		return rgw;
	}

	//Reset stream position
	if(fseek(file, streampos, SEEK_SET) != 0)
	{
		return returnError(error, 1, "could not reset stream position");
	}

	return (0);
}
