/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      Steve Sherman <stevees@us.ibm.com>
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <SaHpi.h>
#include <oh_utils.h>

#define UNDEFINED_MANUFACTURER  -1
#define BAD_TYPE -1

int main(int argc, char **argv) 
{
	const char *expected_str;
	const char *str;
	SaErrorT   expected_err, err;
	SaHpiTextBufferT buffer, bad_buffer;

	/************************************ 
	 * oh_decode_manufacturerid testcases
         ************************************/

	{
		/* oh_decode_manufacturerid: SAHPI_MANUFACTURER_ID_UNSPECIFIED testcase */
		SaHpiManufacturerIdT mid;	

		expected_str = "UNSPECIFIED Manufacturer";
		mid = SAHPI_MANUFACTURER_ID_UNSPECIFIED;

		oh_init_textbuffer(&buffer);
		err = oh_decode_manufacturerid(mid, &buffer); 
		
                if (strcmp(expected_str, buffer.Data) || err) {
                        printf("Error! oh_decode_manufacturerid: SAHPI_MANUFACTURER_ID_UNSPECIFIED testcase failed\n");
			printf("Received string=%s; Error=%d\n", buffer.Data, err);
                        return -1;
                }
		
		/* oh_decode_manufacturerid: IBM testcase */
		expected_str = "IBM";
		mid = 2;

		oh_init_textbuffer(&buffer);
		err = oh_decode_manufacturerid(mid, &buffer); 
		
                if (strcmp(expected_str, buffer.Data) || err) {
                        printf("Error! oh_decode_manufacturerid: IBM testcase failed\n");
			printf("Received string=%s; Error=%d\n", buffer.Data, err);
                        return -1;
                }
		
		/* oh_encode_manufacturerid: Undefined manufacturer testcase */
		expected_str = "Unknown Manufacturer";
		mid = UNDEFINED_MANUFACTURER;

		oh_init_textbuffer(&buffer);
		err = oh_decode_manufacturerid(mid, &buffer);
		
                if (strcmp(expected_str, buffer.Data) || err) {
                        printf("Error! oh_decode_manufacturerid: Undefined manufacturer testcase failed\n");
			printf("Received string=%s; Error=%d\n", buffer.Data, err);
                        return -1;
                }

		/* oh_encode_manufacturerid: NULL buffer testcase */
		expected_err = SA_ERR_HPI_INVALID_PARAMS;
		mid = UNDEFINED_MANUFACTURER;

		oh_init_textbuffer(&buffer);
		err = oh_decode_manufacturerid(mid, 0);
		
                if (err != expected_err) {
                        printf("Error! oh_decode_manufacturerid: NULL buffer testcase failed\n");
			printf("Error received=%d\n", err);
                        return -1;
                }
	}

	/**************************** 
	 * oh_sensorreading testcases
         ****************************/
	{
		SaHpiSensorDataFormatT format_default, format_test;
		SaHpiSensorReadingT reading_default, reading_test;
		
		reading_default.IsSupported = SAHPI_TRUE;
		reading_default.Type = SAHPI_SENSOR_READING_TYPE_INT64;
		reading_default.Value.SensorInt64 = 20;
		
		format_default.IsSupported = SAHPI_TRUE;
		format_default.ReadingType = SAHPI_SENSOR_READING_TYPE_INT64;
		format_default.BaseUnits = SAHPI_SU_VOLTS;
		format_default.ModifierUnits = SAHPI_SU_UNSPECIFIED;
		format_default.ModifierUse = SAHPI_SMUU_NONE;
		format_default.Percentage = SAHPI_FALSE;

		/* oh_sensorreading: NULL buffer testcase */
		expected_err = SA_ERR_HPI_INVALID_PARAMS;
		
		oh_init_textbuffer(&buffer);
		err = oh_decode_sensorreading(reading_default, format_default, 0);
		if (err != expected_err) {
			printf("Error! oh_sensorreading: NULL buffer testcase failed\n");
			printf("Received error=%d\n", err);
			return -1;
		}
		
		/* oh_sensorreading: IsSupported == FALSE testcase */
		expected_err = SA_ERR_HPI_INVALID_CMD;
		reading_test = reading_default;
		format_test = format_default;
		format_test.IsSupported = SAHPI_FALSE;
	
		oh_init_textbuffer(&buffer);
		err = oh_decode_sensorreading(reading_test, format_test, &buffer);
		if (err != expected_err) {
			printf("Error! oh_sensorreading: IsSupported == FALSE testcase failed\n");
			printf("Received error=%d\n", err);
			return -1;
		}

		/* oh_sensorreading: Bad SaHpiSensorModifierUseT testcase */
		expected_err = SA_ERR_HPI_INVALID_PARAMS;
		reading_test = reading_default;
		format_test = format_default;
		format_test.ModifierUnits = SAHPI_SU_WEEK;
		format_test.ModifierUse = BAD_TYPE;
		
		oh_init_textbuffer(&buffer);
		err = oh_decode_sensorreading(reading_test, format_test, &buffer);
		if (err != expected_err) {
			printf("Error! oh_sensorreading: Bad SaHpiSensorModifierUseT testcase failed\n");
			printf("Received error=%d\n", err);
			return -1;
		}

		/* oh_sensorreading: Bad SaHpiSensorReadingT testcase */
		expected_err = SA_ERR_HPI_INVALID_PARAMS;
		reading_test = reading_default;
		reading_test.Type = BAD_TYPE;
		format_test = format_default;
		format_test.ReadingType = BAD_TYPE;
		
		oh_init_textbuffer(&buffer);
		err = oh_decode_sensorreading(reading_test, format_test, &buffer);
		if (err != expected_err) {
			printf("Error! oh_sensorreading: Bad SaHpiSensorReadingT testcase failed\n");
			printf("Received error=%d\n", err);
			return -1;
		}
		
		/* oh_sensorreading: Reading Types not equal testcase */
		expected_err = SA_ERR_HPI_INVALID_DATA;
		reading_test = reading_default;
		format_test = format_default;
		format_test.ReadingType = format_default.ReadingType + 1;
		
		oh_init_textbuffer(&buffer);
		err = oh_decode_sensorreading(reading_test, format_test, &buffer);
		if (err != expected_err) {
			printf("Error! oh_sensorreading: Reading Types not equal testcase failed\n");
			printf("Received error=%d\n", err);
			return -1;
		}

		/* oh_sensorreading: SAHPI_SENSOR_READING_TYPE_INT64 testcase */
		expected_str = "20 VOLTS";
		reading_test = reading_default;
		format_test = format_default;
		
		oh_init_textbuffer(&buffer);
		err = oh_decode_sensorreading(reading_test, format_test, &buffer);
		if (err != SA_OK) {
			printf("Error! oh_sensorreading: SAHPI_SENSOR_READING_TYPE_INT64 testcase failed\n");
			printf("Received error=%d\n", err);
			return -1;
		}
		
                if (strcmp(expected_str, buffer.Data)) {
                        printf("Error! Testcase SAHPI_SENSOR_READING_TYPE_INT64 failed\n");
			printf("Received string=%s\n", buffer.Data);
                        return -1;             
                }
		
		/* oh_sensorreading: SAHPI_SMUU_BASIC_OVER_MODIFIER testcase */
		expected_str = "20 VOLTS / WEEK";
		reading_test = reading_default;
		format_test = format_default;
		format_test.ModifierUnits = SAHPI_SU_WEEK;
		format_test.ModifierUse = SAHPI_SMUU_BASIC_OVER_MODIFIER;
		
		oh_init_textbuffer(&buffer);
		err = oh_decode_sensorreading(reading_test, format_test, &buffer);
		if (err != SA_OK) {
			printf("Error! oh_sensorreading: testcase SAHPI_SMUU_BASIC_OVER_MODIFIER failed\n");
			printf("Received error=%d\n", err);
			return -1;
		}
		
                if (strcmp(expected_str, buffer.Data)) {
                        printf("Error! Testcase SAHPI_SMUU_BASIC_OVER_MODIFIER failed\n");
			printf("Received string=%s\n", buffer.Data);
                        return -1;             
                }

		/* oh_sensorreading: SAHPI_SMUU_BASIC_TIMES_MODIFIER testcase */
		expected_str = "20 VOLTS * WEEK";
		reading_test = reading_default;
		format_test = format_default;
		format_test.ModifierUnits = SAHPI_SU_WEEK;
		format_test.ModifierUse = SAHPI_SMUU_BASIC_TIMES_MODIFIER;
		
		oh_init_textbuffer(&buffer);
		err = oh_decode_sensorreading(reading_test, format_test, &buffer);
		if (err != SA_OK) {
			printf("Error! oh_sensorreading: testcase SAHPI_SMUU_BASIC_TIMES_MODIFIER failed\n");
			printf("Received error=%d\n", err);
			return -1;
		}
		
                if (strcmp(expected_str, buffer.Data)) {
                        printf("Error! Testcase SAHPI_SMUU_BASIC_TIMES_MODIFIER failed\n");
			printf("Received string=%s\n", buffer.Data);
                        return -1;             
                }

		/* oh_sensorreading: Percentage testcase */
		expected_str = "20%";
		reading_test = reading_default;
		format_test = format_default;
		format_test.Percentage = SAHPI_TRUE;
		format_test.ModifierUnits = SAHPI_SU_WEEK;
		format_test.ModifierUse = SAHPI_SMUU_BASIC_TIMES_MODIFIER;
		
		oh_init_textbuffer(&buffer);
		err = oh_decode_sensorreading(reading_test, format_test, &buffer);
		if (err != SA_OK) {
			printf("Error! oh_sensorreading: testcase Percentage failed\n");
			printf("Received error=%d\n", err);
			return -1;
		}
		
                if (strcmp(expected_str, buffer.Data)) {
                        printf("Error! Testcase Percentage failed\n");
			printf("Received string=%s\n", buffer.Data);
                        return -1;             
                }

		/* oh_sensorreading: SAHPI_SENSOR_READING_TYPE_UINT64 testcase */
		expected_str = "20 VOLTS";
		reading_test = reading_default;
		reading_test.Type = SAHPI_SENSOR_READING_TYPE_UINT64;
		reading_test.Value.SensorUint64 = 20;
		format_test = format_default;
		format_test.ReadingType = SAHPI_SENSOR_READING_TYPE_UINT64;
		
		oh_init_textbuffer(&buffer);
		err = oh_decode_sensorreading(reading_test, format_test, &buffer);
		if (err != SA_OK) {
			printf("Error! oh_sensorreading: SAHPI_SENSOR_READING_TYPE_UINT64 testcase failed\n");
			printf("Received error=%d\n", err);
			return -1;
		}
		
                if (strcmp(expected_str, buffer.Data)) {
                        printf("Error! Testcase SAHPI_SENSOR_READING_TYPE_UINT64 failed\n");
			printf("Received string=%s\n", buffer.Data);
                        return -1;             
                }

		/* oh_sensorreading: SAHPI_SENSOR_READING_TYPE_FLOAT64 testcase */
		expected_str = "2.020000e+01 VOLTS";
		reading_test = reading_default;
		reading_test.Type = SAHPI_SENSOR_READING_TYPE_FLOAT64;
		reading_test.Value.SensorFloat64 = 20.2;
		format_test = format_default;
		format_test.ReadingType = SAHPI_SENSOR_READING_TYPE_FLOAT64;
		
		oh_init_textbuffer(&buffer);
		err = oh_decode_sensorreading(reading_test, format_test, &buffer);
		if (err != SA_OK) {
			printf("Error! oh_sensorreading: SAHPI_SENSOR_READING_TYPE_FLOAT64 testcase failed\n");
			printf("Received error=%d\n", err);
			return -1;
		}
		
                if (strcmp(expected_str, buffer.Data)) {
                        printf("Error! Testcase SAHPI_SENSOR_READING_TYPE_FLOAT64 failed\n");
			printf("Received string=%s\n", buffer.Data);
                        return -1;             
                }

		/* oh_sensorreading: SAHPI_SENSOR_READING_TYPE_BUFFER testcase */
		expected_str = "22222222222222222222222222222222 VOLTS";
		reading_test = reading_default;
		reading_test.Type = SAHPI_SENSOR_READING_TYPE_BUFFER;
		memset(reading_test.Value.SensorBuffer, 0x32, SAHPI_SENSOR_BUFFER_LENGTH);
		format_test = format_default;
		format_test.ReadingType = SAHPI_SENSOR_READING_TYPE_BUFFER;
		
		oh_init_textbuffer(&buffer);
		err = oh_decode_sensorreading(reading_test, format_test, &buffer);
		if (err != SA_OK) {
			printf("Error! oh_sensorreading: SAHPI_SENSOR_READING_TYPE_BUFFER testcase failed\n");
			printf("Received error=%d\n", err);
			return -1;
		}
		
                if (strcmp(expected_str, buffer.Data)) {
                        printf("Error! Testcase SAHPI_SENSOR_READING_TYPE_BUFFER failed\n");
			printf("Received string=%s\n", buffer.Data);
                        return -1;             
                }
	}

	/***************************** 
	 * oh_xxx_textbuffer testcases
         *****************************/
	{

		char str[4] = "1234";

		/* oh_init_textbuffer: NULL buffer testcase */
		expected_err = SA_ERR_HPI_INVALID_PARAMS;
		
		err = oh_init_textbuffer(0);
		if (err != expected_err) {
			printf("Error! oh_init_buffer: NULL buffer testcase failed\n");
			printf("Received error=%d\n", err);
			return -1;
		}
		
		/* oh_append_textbuffer: NULL buffer testcase */
		expected_err = SA_ERR_HPI_INVALID_PARAMS;

		err = oh_append_textbuffer(0, str, strlen(str));
		if (err != expected_err) {
			printf("Error! oh_append_buffer: NULL buffer testcase failed\n");
			printf("Received error=%d\n", err);
			return -1;
		}

		/* oh_append_textbuffer: NULL str testcase */
		expected_err = SA_ERR_HPI_INVALID_PARAMS;

		err = oh_append_textbuffer(&buffer, 0, strlen(str));
		if (err != expected_err) {
			printf("Error! oh_append_buffer: NULL str testcase failed\n");
			printf("Received error=%d\n", err);
			return -1;
		}

		/* oh_append_textbuffer: Zero size testcase */
		expected_err = SA_OK;
		memset(buffer.Data, 0, SAHPI_MAX_TEXT_BUFFER_LENGTH);
		strncpy(buffer.Data, str, strlen(str));
		expected_str = str;

		err = oh_append_textbuffer(&buffer, str, 0);
		if (err != SA_OK) {
			printf("Error! oh_append_buffer: Zero size testcase failed\n");
			printf("Received error=%d\n", err);
			return -1;
		}

                if (strcmp(expected_str, buffer.Data)) {
                        printf("Error! Testcase oh_append_texbuffer: Zero size failed\n");
			printf("Received string=%s\n", buffer.Data);
                        return -1;             
                }


		/* oh_append_textbuffer: Out of space testcase */
		{ 
			char bigstr[SAHPI_MAX_TEXT_BUFFER_LENGTH +1];
			
			expected_err = SA_ERR_HPI_OUT_OF_SPACE;
			memset(bigstr, 0x32, SAHPI_MAX_TEXT_BUFFER_LENGTH +1);
			
			err = oh_append_textbuffer(&buffer, bigstr, strlen(bigstr));
			if (err != SA_ERR_HPI_OUT_OF_SPACE) {
				printf("Error! oh_append_buffer:Out of space testcase failed\n");
				printf("Received error=%d\n", err);
				return -1;
			}
		}

		/* oh_copy_textbuffer: NULL buffer testcase */
		expected_err = SA_ERR_HPI_INVALID_PARAMS;

		err = oh_copy_textbuffer(0, 0);
		if (err != expected_err) {
			printf("Error! oh_copy_buffer: NULL buffer testcase failed\n");
			printf("Received error=%d\n", err);
			return -1;
		}
	}

	/*********************************************************** 
	 * oh_fprint_textbuffer/oh_fprint_big_textbuffer testcases
         ***********************************************************/
	{
		str = "OK - Printing Line 1\nOK - Printing Line 2";
		oh_big_textbuffer big_buffer, big_bad_buffer;

		/* Don't need this if expose the oh_xxx_big_textbuffer routines */
		big_buffer.DataType = SAHPI_TL_TYPE_TEXT;
		big_buffer.Language = SAHPI_LANG_ENGLISH;
		memset(big_buffer.Data, 0x32, SAHPI_MAX_TEXT_BUFFER_LENGTH + 2);
		big_buffer.Data[SAHPI_MAX_TEXT_BUFFER_LENGTH + 2] = 0x00;
		big_buffer.Data[SAHPI_MAX_TEXT_BUFFER_LENGTH + 1] = 0x33;
		big_bad_buffer = big_buffer;

		err = oh_init_textbuffer(&buffer);
		if (err != SA_OK) {
			printf("Error! oh_fprint_textbuffer: oh_init_textbuffer failed\n");
			printf("Received error=%d\n", err);
			return -1;
		}
		err = oh_append_textbuffer(&buffer, str, strlen(str));	
		if (err != SA_OK) {
			printf("Error! oh_fprint_textbuffer: oh_append_textbuffer failed\n");
			printf("Received error=%d\n", err);
			return -1;
		}

		/* oh_fprint_textbuffer: oh_printf_textbuffer MACRO testcase */
		err = oh_print_textbuffer(&buffer);
		if (err != SA_OK) {
			printf("Error! oh_fprint_textbuffer: oh_print_textbuffer testcase failed\n");
			printf("Received error=%d\n", err);
			return -1;
		}

		err = oh_print_big_textbuffer(&big_buffer);
		if (err != SA_OK) {
			printf("Error! oh_fprint_textbuffer: oh_print_big_textbuffer testcase failed\n");
			printf("Received error=%d\n", err);
			return -1;
		}

                /* Bad data type testcase */
		expected_err = SA_ERR_HPI_INVALID_DATA;
		err = oh_copy_textbuffer(&bad_buffer, &buffer);
		if (err != SA_OK) {
			printf("Error! oh_fprint_textbuffer: oh_copy_textbuffer failed\n");
			printf("Received error=%d\n", err);
			return -1;
		}

		bad_buffer.DataType = BAD_TYPE;
		err = oh_print_textbuffer(&bad_buffer);
		if (err != expected_err) {
			printf("Error! oh_fprint_textbuffer: Bad data type testcase failed\n");
			printf("Received error=%d\n", err);
			return -1;
		}

		big_bad_buffer.DataType = BAD_TYPE;
		err = oh_print_big_textbuffer(&big_bad_buffer);
		if (err != expected_err) {
			printf("Error! oh_fprint_textbuffer: Bad data type (big buffer) testcase failed\n");
			printf("Received error=%d\n", err);
			return -1;
		}

#if 0
		/* FIXME :: ??? Is there a way to force a bad FILE ID, without blowing up??? */
		/* Bad file handler testcase */
		expected_err = SA_ERR_HPI_INVALID_PARAMS;

		err = oh_fprint_textbuffer(0, &buffer);
		if (err != expected_err) {
			printf("Error! oh_fprint_textbuffer: Bad file handler testcase failed\n");
			printf("Received error=%d\n", err);
			return -1;
		}

		err = oh_fprint_big_textbuffer(0, &big_buffer);
		if (err != expected_err) {
			printf("Error! oh_fprint_textbuffer: Bad file handler (big buffer) testcase failed\n");
			printf("Received error=%d\n", err);
			return -1;
		}
#endif
		/* Normal write to file testcase */

		{
			FILE *fp, *big_fp;
			const char *name = "tmp";
			const char *big_name = "tmpbig";
			const char *mode = "a";

			fp = fopen(name, mode);
			if (fp == NULL) {
				printf("Error! oh_fprint_textbuffer: fopen failed\n");
				return -1;
			}
			err = oh_fprint_textbuffer(fp, &buffer);
			if (err != SA_OK) {
				printf("Error! oh_fprint_textbuffer: Normal write to file testcase failed\n");
				printf("Received error=%d\n", err);
				return -1;
			}

			big_fp = fopen(big_name, mode);
			if (big_fp == NULL) {
				printf("Error! oh_fprint_textbuffer: fopen (big buffer) failed\n");
				return -1;
			}
			err = oh_fprint_big_textbuffer(big_fp, &big_buffer);
			if (err != SA_OK) {
				printf("Error! oh_fprint_textbuffer:  Normal write to file (big buffer) testcase failed\n");
				printf("Received error=%d\n", err);
				return -1;
			}

			fclose(fp);
			fclose(big_fp);

			unlink(name);
			unlink(big_name);
		}
	}

	/****************************** 
	 * oh_print_sensorrec testcases
         ******************************/	
	{
		SaHpiSensorRecT sensor, default_sensor;
		
		sensor.Num = 1;
		sensor.Type = SAHPI_VOLTAGE;
		sensor.Category = SAHPI_EC_THRESHOLD;
		sensor.Events = SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_MINOR;
		sensor.EventCtrl = SAHPI_SEC_READ_ONLY;
		sensor.DataFormat.IsSupported = SAHPI_TRUE;
		sensor.DataFormat.ReadingType = SAHPI_SENSOR_READING_TYPE_INT64;
		sensor.DataFormat.BaseUnits = SAHPI_SU_VOLTS;
		sensor.DataFormat.ModifierUnits = SAHPI_SU_SECOND;
		sensor.DataFormat.ModifierUse = SAHPI_SMUU_BASIC_TIMES_MODIFIER;
		sensor.DataFormat.Percentage = SAHPI_FALSE;
		sensor.DataFormat.Range.Flags = SAHPI_SRF_MIN | SAHPI_SRF_MAX | SAHPI_SRF_NOMINAL |
			                        SAHPI_SRF_NORMAL_MIN | SAHPI_SRF_NORMAL_MAX;
		sensor.DataFormat.Range.Min.IsSupported = SAHPI_TRUE;
		sensor.DataFormat.Range.Min.Type = SAHPI_SENSOR_READING_TYPE_INT64;
		sensor.DataFormat.Range.Min.Value.SensorInt64 = 0;
		sensor.DataFormat.Range.Max.IsSupported = SAHPI_TRUE;
		sensor.DataFormat.Range.Max.Type = SAHPI_SENSOR_READING_TYPE_INT64;
		sensor.DataFormat.Range.Max.Value.SensorInt64 = 100;
		sensor.DataFormat.Range.Nominal.IsSupported = SAHPI_TRUE;
		sensor.DataFormat.Range.Nominal.Type = SAHPI_SENSOR_READING_TYPE_INT64;
		sensor.DataFormat.Range.Nominal.Value.SensorInt64 = 50;
		sensor.DataFormat.Range.NormalMax.IsSupported = SAHPI_TRUE;
		sensor.DataFormat.Range.NormalMax.Type = SAHPI_SENSOR_READING_TYPE_INT64;
		sensor.DataFormat.Range.NormalMax.Value.SensorInt64 = 75;
		sensor.DataFormat.Range.NormalMin.IsSupported = SAHPI_TRUE;
		sensor.DataFormat.Range.NormalMin.Type = SAHPI_SENSOR_READING_TYPE_INT64;
		sensor.DataFormat.Range.NormalMin.Value.SensorInt64 = 25;
		sensor.DataFormat.AccuracyFactor = 0x1234;
		sensor.Oem = 0xFF;
		sensor.ThresholdDefn.IsAccessible = SAHPI_TRUE;
		sensor.ThresholdDefn.ReadThold = SAHPI_STM_LOW_MINOR | SAHPI_STM_LOW_MAJOR | SAHPI_STM_LOW_CRIT | SAHPI_STM_LOW_HYSTERESIS;
		sensor.ThresholdDefn.WriteThold = SAHPI_STM_UP_MINOR | SAHPI_STM_UP_MAJOR | SAHPI_STM_UP_CRIT | SAHPI_STM_UP_HYSTERESIS; 
		sensor.ThresholdDefn.Nonlinear = SAHPI_TRUE;
			
		/* oh_print_sensorrec: Bad parameter testcase */
		expected_err = SA_ERR_HPI_INVALID_PARAMS;

		err = oh_print_sensorrec(0);
		if (err != expected_err) {
			printf("Error! oh_print_sensorrec: Bad parameter testcase failed\n");
			printf("Received error=%d; Expected error=%d\n", err, expected_err);
			return -1;
		}

		/* oh_print_sensorrec: Default sensor testcase */
		err = oh_print_sensorrec(&default_sensor);
		if (err != SA_OK) {
			printf("Error!oh_print_sensorrec: Default sensor testcase failed\n");
			printf("Received error=%d\n", err);
			return -1;
		}

		/* oh_print_sensorrec: Normal sensor testcase */
		err = oh_print_sensorrec(&sensor);
		if (err != SA_OK) {
			printf("Error! oh_print_sensorrec: Normal sensor testcase failed\n");
			printf("Received error=%d\n", err);
			return -1;
		}
	}

	return(0);
}
