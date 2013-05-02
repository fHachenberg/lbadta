/*
 * test_lbadta.c
 *
 *  Created on: 10.03.2013
 *      Author: fabian
 */

#include <check.h>
#include <stdlib.h>
#include <stdint.h>

#include "errmem.h"
#include "hqr.h"
#include "ioconv.h"

char buffer[512];
struct ErrorMemory errmem;

unsigned char compbuf[512];
unsigned char uncompbuf[512];

// BEGIN TEST CASE ioconv

START_TEST (four)
{
	uint32_t lilendian_dword;
	uint16_t lilendian_word;
	char *byteacc = (char*)&lilendian_dword;
	byteacc[0] = 0x78;
	byteacc[1] = 0x56;
	byteacc[2] = 0x34;
	byteacc[3] = 0x12;
	byteacc = (char*)&lilendian_word;
	byteacc[0] = 0x34;
	byteacc[1] = 0x12;
	unsigned int usgnd_dword = fromLE_UDW_to_uint(lilendian_dword);
	unsigned int fromLE_UW_to_uint(uint16_t input);
	int fromLE_UDW_to_int(uint32_t input);
	int fromLE_UW_to_int(uint16_t input);
}
END_TEST


// BEGIN TEST CASE hqr
//Contains all tests for the HQR functions

void setup (void)
{
	initErrorMemory(&errmem, buffer, 512);
}

void teardown (void)
{
}

//FILE pointer NULL
START_TEST (no_file)
{
	FILE *nullfileptr = 0;
	struct HQREntry entry;
	hqr_getItemInformation(&errmem, nullfileptr, 0, &entry);
	fail_unless(isErrorPresent(&errmem));
	initErrorMemory(&errmem, buffer, 512);
	hqr_readItem(&errmem, nullfileptr, 0, compbuf, 512, uncompbuf, 512);
	fail_unless(isErrorPresent(&errmem));
}
END_TEST

START_TEST (empty_file)
{
	const char* filename="/tmp/check_lbadta_empty";
	FILE *fileptr = fopen(filename, "wb");
	fclose(fileptr);
	fileptr = fopen(filename, "rb");
	long int streampos = ftell(fileptr);
	struct HQREntry entry;
	hqr_getItemInformation(&errmem, fileptr, 0, &entry);
	fail_unless(isErrorPresent(&errmem));
	fail_unless(streampos == ftell(fileptr));

	initErrorMemory(&errmem, buffer, 512);
	hqr_readItem(&errmem, fileptr, 0, compbuf, 512, uncompbuf, 512);
	fail_unless(isErrorPresent(&errmem));
	fail_unless(streampos == ftell(fileptr));
	fclose(fileptr);
}
END_TEST

//File opened in WRITE mode
START_TEST (wrong_access_mode)
{
	const char* filename="/tmp/check_lbadta_wrong_access_mode";
	FILE *fileptr = fopen(filename, "wb");
	long int streampos = ftell(fileptr);
	struct HQREntry entry;
	hqr_getItemInformation(&errmem, fileptr, 0, &entry);
	fail_unless(isErrorPresent(&errmem));
	fail_unless(streampos == ftell(fileptr));

	initErrorMemory(&errmem, buffer, 512);
	hqr_readItem(&errmem, fileptr, 0, compbuf, 512, uncompbuf, 512);
	fail_unless(isErrorPresent(&errmem));
	fail_unless(streampos == ftell(fileptr));
	fclose(fileptr);
}
END_TEST

//file contains only the header
START_TEST (onlyheader)
{
	const char* filename="/tmp/check_lbadta_onlyheader";
	FILE *fileptr = fopen(filename, "wb");
	uint32_t headerentry = 4;
	fwrite(&headerentry, 4, 1, fileptr);
	fclose(fileptr);
	fileptr = fopen(filename, "rb");
	long int streampos = ftell(fileptr);
	struct HQREntry entry;
	hqr_getItemInformation(&errmem, fileptr, 0, &entry);
	fail_unless(isErrorPresent(&errmem));
	fail_unless(streampos == ftell(fileptr));

	initErrorMemory(&errmem, buffer, 512);
	hqr_readItem(&errmem, fileptr, 0, compbuf, 512, uncompbuf, 512);
	fail_unless(isErrorPresent(&errmem));
	fail_unless(streampos == ftell(fileptr));
	fclose(fileptr);
}
END_TEST

//Wrong entry index (-1)
START_TEST (indexwrong_minus1)
{
	const char* filename="/tmp/check_lbadta_indexwrong_minus1";
	FILE *fileptr = fopen(filename, "wb");
	uint32_t headerentry = 4;
	fwrite(&headerentry, 4, 1, fileptr);
	fclose(fileptr);
	fileptr = fopen(filename, "rb");
	long int streampos = ftell(fileptr);
	struct HQREntry entry;
	hqr_getItemInformation(&errmem, fileptr, -1, &entry);
	fail_unless(isErrorPresent(&errmem));
	fail_unless(streampos == ftell(fileptr));

	initErrorMemory(&errmem, buffer, 512);
	hqr_readItem(&errmem, fileptr, -1, compbuf, 512, uncompbuf, 512);
	fail_unless(isErrorPresent(&errmem));
	fail_unless(streampos == ftell(fileptr));
	fclose(fileptr);
}
END_TEST

//Wrong entry index (1)
START_TEST (indexwrong_1)
{
	const char* filename="/tmp/check_lbadta_indexwrong_1";
	FILE *fileptr = fopen(filename, "wb");
	uint32_t headerentry = 4;
	fwrite(&headerentry, 4, 1, fileptr);
	fclose(fileptr);
	fileptr = fopen(filename, "rb");
	long int streampos = ftell(fileptr);
	struct HQREntry entry;
	hqr_getItemInformation(&errmem, fileptr, 1, &entry);
	fail_unless(isErrorPresent(&errmem));
	fail_unless(streampos == ftell(fileptr));

	initErrorMemory(&errmem, buffer, 512);
	hqr_readItem(&errmem, fileptr, 1, compbuf, 512, uncompbuf, 512);
	fail_unless(isErrorPresent(&errmem));
	fail_unless(streampos == ftell(fileptr));
	fclose(fileptr);
}
END_TEST

//Header entry for HQR entry points before beginning of file
START_TEST (invalidoffset_minus1)
{
	const char* filename="/tmp/check_lbadta_invalidoffset_minus1";
	FILE *fileptr = fopen(filename, "wb");
	uint32_t headerentry = 0x04000000;
	fwrite(&headerentry, 4, 1, fileptr);
	uint32_t headerentry2 = 0xFFFFFFFF;
	fwrite(&headerentry2, 4, 1, fileptr);
	fclose(fileptr);
	fileptr = fopen(filename, "rb");
	long int streampos = ftell(fileptr);
	struct HQREntry entry;
	hqr_getItemInformation(&errmem, fileptr, 1, &entry);
	fail_unless(isErrorPresent(&errmem));
	fail_unless(streampos == ftell(fileptr));

	initErrorMemory(&errmem, buffer, 512);
	hqr_readItem(&errmem, fileptr, 1, compbuf, 512, uncompbuf, 512);
	fail_unless(isErrorPresent(&errmem));
	fail_unless(streampos == ftell(fileptr));
	fclose(fileptr);
}
END_TEST

//Header entry for HQR entry points beyong end of file
START_TEST (invalidoffset_beyondend)
{
	const char* filename="/tmp/check_lbadta_invalidoffset_beyondend";
	FILE *fileptr = fopen(filename, "wb");
	uint32_t headerentry = 0x04000000;
	fwrite(&headerentry, 4, 1, fileptr);
	uint32_t headerentry2 = 0x10000000; //this offset doesn't exist in the file
	fwrite(&headerentry2, 4, 1, fileptr);
	fclose(fileptr);
	fileptr = fopen(filename, "rb");
	long int streampos = ftell(fileptr);
	struct HQREntry entry;
	hqr_getItemInformation(&errmem, fileptr, 1, &entry);
	fail_unless(isErrorPresent(&errmem));
	fail_unless(streampos == ftell(fileptr));

	initErrorMemory(&errmem, buffer, 512);
	hqr_readItem(&errmem, fileptr, 1, compbuf, 512, uncompbuf, 512);
	fail_unless(isErrorPresent(&errmem));
	fail_unless(streampos == ftell(fileptr));
	fclose(fileptr);
}
END_TEST

//Nothing wrong, should work!
START_TEST (simple_uncompressed_dta)
{
	const char* filename="/tmp/check_lbadta_simple_uncompressed_dta";
	FILE *fileptr = fopen(filename, "wb");
	uint32_t headersize = 8;
	fwrite(&headersize, 4, 1, fileptr);
	uint32_t headerentry = 8;
	fwrite(&headerentry, 4, 1, fileptr);
	uint32_t uncompsize = 4;
	fwrite(&uncompsize, 4, 1, fileptr);
	uint32_t compsize = 4;
	fwrite(&compsize, 4, 1, fileptr);
	uint16_t mode = 0;
	fwrite(&mode, 2, 1, fileptr);
	uint32_t data = 0x12345678;
	fwrite(&data, 4, 1, fileptr);
	fclose(fileptr);
	fileptr = fopen(filename, "rb");
	long int streampos = ftell(fileptr);
	struct HQREntry entry;
	hqr_getItemInformation(&errmem, fileptr, 1, &entry);
	printf("%d\n", entry.offset);
	printf("%d\n", entry.uncompressed_size);
	fail_unless(entry.uncompressed_size == 4);
	fail_unless(entry.compressed_size == 4);
	fail_unless(entry.is_compressed == 0);
	fail_unless(isErrorPresent(&errmem) == 0, getLastErrorString(&errmem));
	fail_unless(streampos == ftell(fileptr));

	initErrorMemory(&errmem, buffer, 512);
	hqr_readItem(&errmem, fileptr, 1, compbuf, 512, uncompbuf, 512);
	fail_unless(isErrorPresent(&errmem) == 0);
	fail_unless(streampos == ftell(fileptr));
	fclose(fileptr);
}
END_TEST

// END TEST CASE hqr

Suite * lbadta_suite (void)
{
	Suite *s = suite_create ("LBADTA");

	/* Core test case */
	TCase *tc_core = tcase_create ("hqr");
	tcase_add_checked_fixture (tc_core, setup, teardown);
	tcase_add_test (tc_core, no_file);
	tcase_add_test (tc_core, empty_file);
	tcase_add_test (tc_core, wrong_access_mode);
	tcase_add_test (tc_core, onlyheader);
	tcase_add_test (tc_core, indexwrong_minus1);
	tcase_add_test (tc_core, indexwrong_1);
	tcase_add_test (tc_core, invalidoffset_minus1);
	tcase_add_test (tc_core, invalidoffset_beyondend);
	tcase_add_test (tc_core, simple_uncompressed_dta);

	suite_add_tcase (s, tc_core);

	return s;
}

int main (void)
{
	int number_failed;
	Suite *s = lbadta_suite ();
	SRunner *sr = srunner_create (s);
	srunner_run_all (sr, CK_NORMAL);
	number_failed = srunner_ntests_failed (sr);
	srunner_free (sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
 }

