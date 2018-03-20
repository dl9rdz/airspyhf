/*
 * Copyright 2012 Jared Boone <jared@sharebrained.com>
 * Copyright 2013 Michael Ossmann <mike@ossmann.com>
 * Copyright 2013/2014 Benjamin Vernoux <bvernoux@airspy.com>
 *
 * This file is part of AirSpy (based on HackRF project).
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include "airspyhf.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#ifndef bool
typedef int bool;
#define true 1
#define false 0
#endif


#define airspy_error_name(x) ""

#define AIRSPY_MAX_DEVICE (32)
char version[255 + 1];
airspyhf_read_partid_serialno_t read_partid_serialno;
struct airspyhf_device* devices[AIRSPY_MAX_DEVICE+1] = { NULL };

int parse_u64(char* s, uint64_t* const value) {
	uint_fast8_t base = 10;
	char* s_end;
	uint64_t u64_value;

	if( strlen(s) > 2 ) {
		if( s[0] == '0' ) {
			if( (s[1] == 'x') || (s[1] == 'X') ) {
				base = 16;
				s += 2;
			} else if( (s[1] == 'b') || (s[1] == 'B') ) {
				base = 2;
				s += 2;
			}
		}
	}

	s_end = s;
	u64_value = strtoull(s, &s_end, base);
	if( (s != s_end) && (*s_end == 0) ) {
		*value = u64_value;
		return AIRSPYHF_SUCCESS;
	} else {
		return AIRSPYHF_ERROR;
	}
}

static void usage(void)
{
	printf("Usage:\n");
	printf("\t[-s serial_number_64bits]: Open board with specified 64bits serial number.\n");
}

bool serial_number = false;
uint64_t serial_number_val;

int main(int argc, char** argv)
{
	int i;
	uint32_t j;
	int result;
	int opt;
	uint32_t count;
	uint32_t *samplerates;
	uint32_t serial_number_msb_val;
	uint32_t serial_number_lsb_val;
	airspyhf_lib_version_t lib_version;
	uint8_t board_id = AIRSPYHF_BOARD_ID_INVALID;

	while( (opt = getopt(argc, argv, "s:")) != EOF )
	{
		result = AIRSPYHF_SUCCESS;
		switch( opt ) 
		{
		case 's':
			serial_number = true;
			result = parse_u64(optarg, &serial_number_val);
			serial_number_msb_val = (uint32_t)(serial_number_val >> 32);
			serial_number_lsb_val = (uint32_t)(serial_number_val & 0xFFFFFFFF);
			printf("Board serial number to open: 0x%08X%08X\n", serial_number_msb_val, serial_number_lsb_val);
			break;

		default:
			printf("unknown argument '-%c %s'\n", opt, optarg);
			usage();
			return EXIT_FAILURE;
		}
		
		if( result != AIRSPYHF_SUCCESS ) {
			printf("argument error: '-%c %s' (%d)\n", opt, optarg, result);
			usage();
			return EXIT_FAILURE;
		}		
	}
#if 0
	result = airspy_init();
	if (result != AIRSPY_SUCCESS) {
		fprintf(stderr, "airspy_init() failed: %s (%d)\n",
				airspy_error_name(result), result);
		return EXIT_FAILURE;
	}
#endif

	airspyhf_lib_version(&lib_version);
	printf("airspyhf_lib_version: %d.%d.%d\n", 
					lib_version.major_version, lib_version.minor_version, lib_version.revision); 
// TODO FIXME rdz
// should use airspyhf_list_devices
	for (i = 0; i < AIRSPY_MAX_DEVICE; i++)
	{
		if(serial_number == true)
		{
			result = airspyhf_open_sn(&devices[i], serial_number_val);
		}else
		{
			result = airspyhf_open(&devices[i]);
		}
		if (result != AIRSPYHF_SUCCESS)
		{
			if(i == 0)
			{
				fprintf(stderr, "airspyhf_open() board %d failed: (%d)\n",
						i+1, result);
			}
			break;
		}
	}

	for(i = 0; i < AIRSPY_MAX_DEVICE; i++)
	{
		if(devices[i] != NULL)
		{
			printf("\nFound AirSpy board %d\n", i + 1);
			fflush(stdout);
			//result = airspyhf_board_id_read(devices[i], &board_id);
			//if (result != AIRSPYHF_SUCCESS) {
			//	fprintf(stderr, "airspy_board_id_read() failed: (%d)\n",
			//		result);
			//	continue;
			//}
			//printf("Board ID Number: %d (%s)\n", board_id,
			//	airspyhf_board_id_name(board_id));

			result = airspyhf_version_string_read(devices[i], &version[0], 255);
			if (result != AIRSPYHF_SUCCESS) {
				fprintf(stderr, "airspyhf_version_string_read() failed: %s (%d)\n",
					airspy_error_name(result), result);
				continue;
			}
			printf("Firmware Version: %s\n", version);

			result = airspyhf_board_partid_serialno_read(devices[i], &read_partid_serialno);
			if (result != AIRSPYHF_SUCCESS) {
				fprintf(stderr, "airspyhf_board_partid_serialno_read() failed: %s (%d)\n",
					airspy_error_name(result), result);
				continue;
			}
			printf("Part ID Number: 0x%08X\n",
				read_partid_serialno.part_id);
			printf("Serial Number: 0x%08X%08X\n",
				read_partid_serialno.serial_no[2],
				read_partid_serialno.serial_no[3]);

			printf("Supported sample rates:\n");
			airspyhf_get_samplerates(devices[i], &count, 0);
			samplerates = (uint32_t *) malloc(count * sizeof(uint32_t));
			airspyhf_get_samplerates(devices[i], samplerates, count);
			for (j = 0; j < count; j++)
			{
				printf("\t%f MSPS\n", samplerates[j] * 0.000001f);
			}
			free(samplerates);

			printf("Close board %d\n", i+1);
			result = airspyhf_close(devices[i]);
			if (result != AIRSPYHF_SUCCESS) {
				fprintf(stderr, "airspyhf_close() board %d failed: %s (%d)\n",
						i+1, airspy_error_name(result), result);
				continue;
			}
		}
	}

	//airspy_exit();
	return EXIT_SUCCESS;
}
