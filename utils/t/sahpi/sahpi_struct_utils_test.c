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

		err = oh_decode_manufacturerid(mid, &buffer); 
		
                if (strcmp(expected_str, buffer.Data) || err) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received string=%s; Expected string=%s; Error=%d\n", 
			       buffer.Data, expected_str, err);
                        return -1;
                }
		
		/* oh_decode_manufacturerid: IBM testcase */
		expected_str = "IBM";
		mid = 2;

		err = oh_decode_manufacturerid(mid, &buffer); 
		
                if (strcmp(expected_str, buffer.Data) || err) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received string=%s; Expected string=%s; Error=%d\n", 
			       buffer.Data, expected_str, err);
                         return -1;
                }
		
		/* oh_decode_manufacturerid: Undefined manufacturer testcase */
		expected_str = "Unknown Manufacturer";
		mid = UNDEFINED_MANUFACTURER;

		err = oh_decode_manufacturerid(mid, &buffer);
		
                if (strcmp(expected_str, buffer.Data) || err) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received string=%s; Expected string=%s; Error=%d\n", 
			       buffer.Data, expected_str, err);
                         return -1;
                }

		/* oh_decode_manufacturerid: NULL buffer testcase */
		expected_err = SA_ERR_HPI_INVALID_PARAMS;
		mid = UNDEFINED_MANUFACTURER;

		err = oh_decode_manufacturerid(mid, 0);
		
                if (err != expected_err) {
 			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d; Expected error=%d\n", err, expected_err);
                         return -1;
                }
	}

	/*********************************************************** 
	 * oh_decode_sensorreading/oh_encode_sensorreading testcases
         ***********************************************************/
	{
		SaHpiSensorDataFormatT format_default, format_test;
		SaHpiSensorReadingT reading_default, reading_test, encode_reading;
		memset(&reading_default, 0, sizeof(SaHpiSensorReadingT));

		reading_default.IsSupported = SAHPI_TRUE;
		reading_default.Type = SAHPI_SENSOR_READING_TYPE_INT64;
		reading_default.Value.SensorInt64 = 20;
		
		format_default.IsSupported = SAHPI_TRUE;
		format_default.ReadingType = SAHPI_SENSOR_READING_TYPE_INT64;
		format_default.BaseUnits = SAHPI_SU_VOLTS;
		format_default.ModifierUnits = SAHPI_SU_UNSPECIFIED;
		format_default.ModifierUse = SAHPI_SMUU_NONE;
		format_default.Percentage = SAHPI_FALSE;

		/* oh_decode_sensorreading: NULL buffer testcase */
		expected_err = SA_ERR_HPI_INVALID_PARAMS;
		
		err = oh_decode_sensorreading(reading_default, format_default, 0);
		if (err != expected_err) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d; Expected error=%d\n", err, expected_err);
 			return -1;
		}
		
		/* oh_decode_sensorreading: IsSupported == FALSE testcase */
		expected_err = SA_ERR_HPI_INVALID_CMD;
		reading_test = reading_default;
		format_test = format_default;
		format_test.IsSupported = SAHPI_FALSE;
	
		err = oh_decode_sensorreading(reading_test, format_test, &buffer);
		if (err != expected_err) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d; Expected error=%d\n", err, expected_err);
 			return -1;
		}

		/* oh_decode_sensorreading: Bad SaHpiSensorModifierUseT testcase */
		expected_err = SA_ERR_HPI_INVALID_PARAMS;
		reading_test = reading_default;
		format_test = format_default;
		format_test.ModifierUnits = SAHPI_SU_WEEK;
		format_test.ModifierUse = BAD_TYPE;
		
		err = oh_decode_sensorreading(reading_test, format_test, &buffer);
		if (err != expected_err) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d; Expected error=%d\n", err, expected_err);
			return -1;
		}

		/* oh_decode_sensorreading: Bad SaHpiSensorReadingT testcase */
		expected_err = SA_ERR_HPI_INVALID_PARAMS;
		reading_test = reading_default;
		reading_test.Type = BAD_TYPE;
		format_test = format_default;
		format_test.ReadingType = BAD_TYPE;
		
		err = oh_decode_sensorreading(reading_test, format_test, &buffer);
		if (err != expected_err) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d; Expected error=%d\n", err, expected_err);
			return -1;
		}
		
		/* oh_decode_sensorreading: Reading Types not equal testcase */
		expected_err = SA_ERR_HPI_INVALID_DATA;
		reading_test = reading_default;
		format_test = format_default;
		format_test.ReadingType = format_default.ReadingType + 1;
		
		err = oh_decode_sensorreading(reading_test, format_test, &buffer);
		if (err != expected_err) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d; Expected error=%d\n", err, expected_err);
			return -1;
		}

		/* oh_decode_sensorreading: SAHPI_SENSOR_READING_TYPE_INT64 testcase */
		expected_str = "20 Volts";
		reading_test = reading_default;
		format_test = format_default;
		
		err = oh_decode_sensorreading(reading_test, format_test, &buffer);
		if (err != SA_OK) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d\n", err);
			return -1;
		}
		
                if (strcmp(expected_str, buffer.Data)) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received string=%s; Expected string=%s\n", 
			       buffer.Data, expected_str);
                        return -1;             
                }

		memset(&encode_reading, 0, sizeof(SaHpiSensorReadingT));
		err = oh_encode_sensorreading(&buffer, format_test.ReadingType, &encode_reading);
		if (err != SA_OK) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d\n", err);
			return -1;
		}
		
		if (memcmp((void *)&encode_reading, (void *)&reading_test, 
			   sizeof(SaHpiSensorReadingT))) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d\n", err);
			return -1;
		}
		
		/* oh_decode_sensorreading: SAHPI_SMUU_BASIC_OVER_MODIFIER testcase */
		expected_str = "20 Volts / Week";
		reading_test = reading_default;
		format_test = format_default;
		format_test.ModifierUnits = SAHPI_SU_WEEK;
		format_test.ModifierUse = SAHPI_SMUU_BASIC_OVER_MODIFIER;
		
		err = oh_decode_sensorreading(reading_test, format_test, &buffer);
		if (err != SA_OK) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d\n", err);
			return -1;
		}
		
                if (strcmp(expected_str, buffer.Data)) {
 			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received string=%s; Expected string=%s\n", 
			       buffer.Data, expected_str);
                       return -1;             
                }

		err = oh_encode_sensorreading(&buffer, format_test.ReadingType, &encode_reading);
		if (err != SA_OK) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d\n", err);
			return -1;
		}
		
		if (memcmp((void *)&encode_reading, (void *)&reading_test, 
			   sizeof(SaHpiSensorReadingTypeT))) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d\n", err);
			return -1;
		}

		/* oh_decode_sensorreading: SAHPI_SMUU_BASIC_TIMES_MODIFIER testcase */
		expected_str = "20 Volts * Week";
		reading_test = reading_default;
		format_test = format_default;
		format_test.ModifierUnits = SAHPI_SU_WEEK;
		format_test.ModifierUse = SAHPI_SMUU_BASIC_TIMES_MODIFIER;
		
		err = oh_decode_sensorreading(reading_test, format_test, &buffer);
		if (err != SA_OK) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d\n", err);
			return -1;
		}
		
                if (strcmp(expected_str, buffer.Data)) {
  			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received string=%s; Expected string=%s\n", 
			       buffer.Data, expected_str);
                       return -1;             
                }

		err = oh_encode_sensorreading(&buffer, format_test.ReadingType, &encode_reading);
		if (err != SA_OK) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d\n", err);
			return -1;
		}
		
		if (memcmp((void *)&encode_reading, (void *)&reading_test, 
			   sizeof(SaHpiSensorReadingTypeT))) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d\n", err);
			return -1;
		}

		/* oh_decode_sensorreading: Percentage testcase */
		expected_str = "20%";
		reading_test = reading_default;
		
		format_test = format_default;
		format_test.Percentage = SAHPI_TRUE;
		format_test.ModifierUnits = SAHPI_SU_WEEK;
		format_test.ModifierUse = SAHPI_SMUU_BASIC_TIMES_MODIFIER;
		
		err = oh_decode_sensorreading(reading_test, format_test, &buffer);
		if (err != SA_OK) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d\n", err);
			return -1;
		}
		
                if (strcmp(expected_str, buffer.Data)) {
   			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received string=%s; Expected string=%s\n", 
			       buffer.Data, expected_str);
                       return -1;             
                }

		/* oh_decode_sensorreading: SAHPI_SENSOR_READING_TYPE_UINT64 testcase */
		expected_str = "20 Volts";
		reading_test = reading_default;
		reading_test.Type = SAHPI_SENSOR_READING_TYPE_UINT64;
		reading_test.Value.SensorUint64 = 20;
		format_test = format_default;
		format_test.ReadingType = SAHPI_SENSOR_READING_TYPE_UINT64;
		
		err = oh_decode_sensorreading(reading_test, format_test, &buffer);
		if (err != SA_OK) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d\n", err);
			return -1;
		}
		
                if (strcmp(expected_str, buffer.Data)) {
    			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received string=%s; Expected string=%s\n", 
			       buffer.Data, expected_str);
                       return -1;             
                }

		err = oh_encode_sensorreading(&buffer, format_test.ReadingType, &encode_reading);
		if (err != SA_OK) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d\n", err);
			return -1;
		}
		
		if (memcmp((void *)&encode_reading, (void *)&reading_test, 
			   sizeof(SaHpiSensorReadingTypeT))) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d\n", err);
			return -1;
		}

		/* oh_decode_sensorreading: SAHPI_SENSOR_READING_TYPE_FLOAT64 testcase */
		expected_str = "2.020000e+01 Volts";
		reading_test = reading_default;
		reading_test.Type = SAHPI_SENSOR_READING_TYPE_FLOAT64;
		reading_test.Value.SensorFloat64 = 20.2;
		format_test = format_default;
		format_test.ReadingType = SAHPI_SENSOR_READING_TYPE_FLOAT64;
		
		err = oh_decode_sensorreading(reading_test, format_test, &buffer);
		if (err != SA_OK) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d\n", err);
			return -1;
		}
		
                if (strcmp(expected_str, buffer.Data)) {
    			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received string=%s; Expected string=%s\n", 
			       buffer.Data, expected_str);
			return -1;             
                }

		/* oh_decode_sensorreading: SAHPI_SENSOR_READING_TYPE_BUFFER testcase */
		expected_str = "22222222222222222222222222222222 Volts";
		reading_test = reading_default;
		reading_test.Type = SAHPI_SENSOR_READING_TYPE_BUFFER;
		memset(reading_test.Value.SensorBuffer, 0x32, SAHPI_SENSOR_BUFFER_LENGTH);
		format_test = format_default;
		format_test.ReadingType = SAHPI_SENSOR_READING_TYPE_BUFFER;
		
		err = oh_decode_sensorreading(reading_test, format_test, &buffer);
		if (err != SA_OK) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d\n", err);
			return -1;
		}
		
                if (strcmp(expected_str, buffer.Data)) {
    			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received string=%s; Expected string=%s\n", 
			       buffer.Data, expected_str);
                        return -1;             
                }

		err = oh_encode_sensorreading(&buffer, format_test.ReadingType, &encode_reading);
		if (err != SA_OK) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d\n", err);
			return -1;
		}
		
		if (memcmp((void *)&encode_reading, (void *)&reading_test, 
			   sizeof(SaHpiSensorReadingTypeT))) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d\n", err);
			return -1;
		}
	}

	/*********************************** 
         * oh_encode_sensorreading testcases
	 ***********************************/
	{
		const char *str;
		SaHpiTextBufferT buffer;
		SaHpiSensorReadingT reading;
		SaHpiInt64T   expected_int64;
		/* SaHpiUint64T  expected_uint64; */
		SaHpiFloat64T expected_float64;

		/* oh_encode_sensorreading: Bad type testcase */
		expected_err = SA_ERR_HPI_INVALID_PARAMS;
		err = oh_encode_sensorreading(&buffer, BAD_TYPE, &reading);
		if (err != expected_err) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d; Expected error=%d\n", err, expected_err);
			return -1;
		}

		/* oh_encode_sensorreading: Extra spaces testcase */
		str = " + 20  Volts";
		expected_int64 = 20;
		oh_init_textbuffer(&buffer);
		oh_append_textbuffer(&buffer, str);
		err = oh_encode_sensorreading(&buffer, SAHPI_SENSOR_READING_TYPE_INT64, &reading);
		if (err != SA_OK) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d\n", err);
			return -1;
		}

		if (reading.Value.SensorInt64 != expected_int64) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received value=%lld; Expected value=%lld\n", 
			       reading.Value.SensorInt64, expected_int64);
			return -1;
		}

		/* oh_encode_sensorreading: Extra non-digits/commas testcase */
		str = "The, happy, %% result is ... +2,000Volts ,,... ";
		expected_int64 = 2000;
		oh_init_textbuffer(&buffer);
		oh_append_textbuffer(&buffer, str);
		err = oh_encode_sensorreading(&buffer, SAHPI_SENSOR_READING_TYPE_INT64, &reading);
		if (err != SA_OK) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d\n", err);
			return -1;
		}

		if (reading.Value.SensorInt64 != expected_int64) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received value=%lld; Expected value=%lld\n", 
			       reading.Value.SensorInt64, expected_int64);
			return -1;
		}

		/* oh_encode_sensorreading: No digits testcase */
		str = "There are no numbers in this string";
		expected_err = SA_ERR_HPI_INVALID_DATA;
		oh_init_textbuffer(&buffer);
		oh_append_textbuffer(&buffer, str);
		err = oh_encode_sensorreading(&buffer, SAHPI_SENSOR_READING_TYPE_INT64, &reading);
		if (err != expected_err) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d; Expected error=%d\n", err, expected_err);
			return -1;
		}

		/* oh_encode_sensorreading: Decimal point testcase */
		str = "-2.5volts";
		expected_float64 = -2.5;
		oh_init_textbuffer(&buffer);
		oh_append_textbuffer(&buffer, str);
		err = oh_encode_sensorreading(&buffer, SAHPI_SENSOR_READING_TYPE_FLOAT64, &reading);
		if (err != SA_OK) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d\n", err);
			return -1;
		}

		if (reading.Value.SensorFloat64 != expected_float64) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received value=%le; Expected value=%le\n", 
			       reading.Value.SensorFloat64, expected_float64);
			return -1;
		}

		/* oh_encode_sensorreading: Too many decimal points testcase */
		str = "1.000.000 volts";
		expected_err = SA_ERR_HPI_INVALID_DATA;
		oh_init_textbuffer(&buffer);
		oh_append_textbuffer(&buffer, str);
		err = oh_encode_sensorreading(&buffer, SAHPI_SENSOR_READING_TYPE_FLOAT64, &reading);
		if (err != expected_err) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d; Expected error=%d\n", err, expected_err);
			return -1;
		}

		/* oh_encode_sensorreading: Too many signs */
		str = "+-33e02";
		expected_err = SA_ERR_HPI_INVALID_DATA;
		oh_init_textbuffer(&buffer);
		oh_append_textbuffer(&buffer, str);
		err = oh_encode_sensorreading(&buffer, SAHPI_SENSOR_READING_TYPE_INT64, &reading);
		if (err != expected_err) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d; Expected error=%d\n", err, expected_err);
			return -1;
		}

		/* oh_encode_sensorreading: Percentage testcase */
		str = "33% RPM";
		expected_float64 = 0.33;
		oh_init_textbuffer(&buffer);
		oh_append_textbuffer(&buffer, str);
		err = oh_encode_sensorreading(&buffer, SAHPI_SENSOR_READING_TYPE_FLOAT64, &reading);
		if (err != SA_OK) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d\n", err);
			return -1;
		}

		if (reading.Value.SensorFloat64 != expected_float64) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received value=%le; Expected value=%le\n", 
			       reading.Value.SensorFloat64, expected_float64);
			return -1;
		}

		/* oh_encode_sensorreading: Percentage bad type testcase */
		str = "33% RPM";
		expected_err = SA_ERR_HPI_INVALID_DATA;
		oh_init_textbuffer(&buffer);
		oh_append_textbuffer(&buffer, str);
		err = oh_encode_sensorreading(&buffer, SAHPI_SENSOR_READING_TYPE_INT64, &reading);
		if (err != expected_err) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d; Expected error=%d\n", err, expected_err);
			return -1;
		}

		/* oh_encode_sensorreading: Too big int64 testcase */
		str = "99999999999999999999999999999999";
		expected_err = SA_ERR_HPI_INVALID_DATA;
		oh_init_textbuffer(&buffer);
		oh_append_textbuffer(&buffer, str);
		err = oh_encode_sensorreading(&buffer, SAHPI_SENSOR_READING_TYPE_INT64, &reading);
		if (err != expected_err) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d; Expected error=%d\n", err, expected_err);
			return -1;
		}

		/* oh_encode_sensorreading: Too big uint64 testcase */
		str = "99999999999999999999999999999999";
		expected_err = SA_ERR_HPI_INVALID_DATA;
		oh_init_textbuffer(&buffer);
		oh_append_textbuffer(&buffer, str);
		err = oh_encode_sensorreading(&buffer, SAHPI_SENSOR_READING_TYPE_UINT64, &reading);
		if (err != expected_err) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d; Expected error=%d\n", err, expected_err);
			return -1;
		}

#if 0
		/* oh_encode_sensorreading: Too big float64 testcase */
		str = "99999999999999999999999999999999";
		expected_err = SA_ERR_HPI_INVALID_DATA;
		oh_init_textbuffer(&buffer);
		oh_append_textbuffer(&buffer, str);
		err = oh_encode_sensorreading(&buffer, SAHPI_SENSOR_READING_TYPE_FLOAT64, &reading);
		if (err != expected_err) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d; Expected error=%d\n", err, expected_err);
			return -1;
		}
#endif
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
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d; Expected error=%d\n", err, expected_err);
			return -1;
		}
		
		/* oh_append_textbuffer: NULL buffer testcase */
		expected_err = SA_ERR_HPI_INVALID_PARAMS;

		err = oh_append_textbuffer(0, str);
		if (err != expected_err) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d; Expected error=%d\n", err, expected_err);
			return -1;
		}

		/* oh_append_textbuffer: NULL str testcase */
		expected_err = SA_ERR_HPI_INVALID_PARAMS;

		err = oh_append_textbuffer(&buffer, 0);
		if (err != expected_err) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d; Expected error=%d\n", err, expected_err);
			return -1;
		}

		/* oh_append_textbuffer: Out of space testcase */
		{ 
			char bigstr[SAHPI_MAX_TEXT_BUFFER_LENGTH +1];
			
			expected_err = SA_ERR_HPI_OUT_OF_SPACE;
			memset(bigstr, 0x32, SAHPI_MAX_TEXT_BUFFER_LENGTH +1);
			
			err = oh_append_textbuffer(&buffer, bigstr);
			if (err != expected_err) {
				printf("  Error! Testcase failed. Line=%d\n", __LINE__);
				printf("  Received error=%d; Expected error=%d\n", err, expected_err);
				return -1;
			}
		}

		/* oh_copy_textbuffer: NULL buffer testcase */
		expected_err = SA_ERR_HPI_INVALID_PARAMS;

		err = oh_copy_textbuffer(0, 0);
		if (err != expected_err) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d; Expected error=%d\n", err, expected_err);
			return -1;
		}
	}

	/******************************************** 
	 * oh_fprint_text/oh_fprint_bigtext testcases
         ********************************************/
	{
		str = "OK - Printing Line 1\nOK - Printing Line 2";
		oh_big_textbuffer big_buffer, big_bad_buffer;

		/* Don't need this if expose the oh_xxx_bigtext routines */
		big_buffer.DataType = SAHPI_TL_TYPE_TEXT;
		big_buffer.Language = SAHPI_LANG_ENGLISH;
		memset(big_buffer.Data, 0x32, SAHPI_MAX_TEXT_BUFFER_LENGTH + 2);
		big_buffer.Data[SAHPI_MAX_TEXT_BUFFER_LENGTH + 2] = 0x00;
		big_buffer.Data[SAHPI_MAX_TEXT_BUFFER_LENGTH + 1] = 0x33;
		big_bad_buffer = big_buffer;

		err = oh_init_textbuffer(&buffer);
		if (err != SA_OK) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d\n", err);
			return -1;
		}
		err = oh_append_textbuffer(&buffer, str);	
		if (err != SA_OK) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d\n", err);
			return -1;
		}

		/* oh_fprint_text: oh_print_text MACRO testcase */
		err = oh_print_text(&buffer);
		if (err != SA_OK) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d\n", err);
			return -1;
		}

		err = oh_print_bigtext(&big_buffer);
		if (err != SA_OK) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d\n", err);
			return -1;
		}

                /* Bad data type testcase */
		expected_err = SA_ERR_HPI_INVALID_DATA;
		err = oh_copy_textbuffer(&bad_buffer, &buffer);
		if (err != SA_OK) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d\n", err);
			return -1;
		}

		bad_buffer.DataType = BAD_TYPE;
		err = oh_print_text(&bad_buffer);
		if (err != expected_err) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d; Expected error=%d\n", err, expected_err);
			return -1;
		}

		big_bad_buffer.DataType = BAD_TYPE;
		err = oh_print_bigtext(&big_bad_buffer);
		if (err != expected_err) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d; Expected error=%d\n", err, expected_err);
			return -1;
		}

#if 0
		/* FIXME :: ??? Is there a way to force a bad FILE ID, without blowing up??? */
		/* Bad file handler testcase */
		expected_err = SA_ERR_HPI_INVALID_PARAMS;

		err = oh_fprint_text(0, &buffer);
		if (err != expected_err) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d; Expected error=%d\n", err, expected_err);
			return -1;
		}

		err = oh_fprint_bigtext(0, &big_buffer);
		if (err != expected_err) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d; Expected error=%d\n", err, expected_err);
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
				printf("  Error! Testcase failed. Line=%d\n", __LINE__);
				return -1;
			}
			err = oh_fprint_text(fp, &buffer);
			if (err != SA_OK) {
				printf("  Error! Testcase failed. Line=%d\n", __LINE__);
				printf("  Received error=%d\n", err);
				return -1;
			}

			big_fp = fopen(big_name, mode);
			if (big_fp == NULL) {
				printf("  Error! Testcase failed. Line=%d\n", __LINE__);
				return -1;
			}
			err = oh_fprint_bigtext(big_fp, &big_buffer);
			if (err != SA_OK) {
				printf("  Error! Testcase failed. Line=%d\n", __LINE__);
				printf("  Received error=%d\n", err);
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
		sensor.DataFormat.AccuracyFactor = 0.05;
		sensor.Oem = 0xFF;
		sensor.ThresholdDefn.IsAccessible = SAHPI_TRUE;
		sensor.ThresholdDefn.ReadThold = SAHPI_STM_LOW_MINOR | SAHPI_STM_LOW_MAJOR | SAHPI_STM_LOW_CRIT | SAHPI_STM_LOW_HYSTERESIS;
		sensor.ThresholdDefn.WriteThold = SAHPI_STM_UP_MINOR | SAHPI_STM_UP_MAJOR | SAHPI_STM_UP_CRIT | SAHPI_STM_UP_HYSTERESIS; 
		sensor.ThresholdDefn.Nonlinear = SAHPI_TRUE;
			
		/* oh_print_sensorrec: Bad parameter testcase */
		expected_err = SA_ERR_HPI_INVALID_PARAMS;

		err = oh_print_sensorrec(0, 0);
		if (err != expected_err) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d; Expected error=%d\n", err, expected_err);
			return -1;
		}

		/* oh_print_sensorrec: Default sensor testcase */
		err = oh_print_sensorrec(&default_sensor, 0);
		if (err != SA_OK) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d\n", err);
			return -1;
		}

		/* oh_print_sensorrec: Normal sensor testcase */
		err = oh_print_sensorrec(&sensor, 0);
		if (err != SA_OK) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d\n", err);
			return -1;
		}
	}

	/******************************* 
	 * oh_print_textbuffer testcases
         *******************************/	
	{
		SaHpiTextBufferT textbuffer, default_textbuffer;
		
		textbuffer.DataType = SAHPI_TL_TYPE_TEXT;
		textbuffer.Language = SAHPI_LANG_ZULU;
		strcpy(textbuffer.Data, "Test Data");
		textbuffer.DataLength = strlen(textbuffer.Data);

		/* oh_print_textbuffer: Bad parameter testcase */
		expected_err = SA_ERR_HPI_INVALID_PARAMS;

		err = oh_print_textbuffer(0, 0);
		if (err != expected_err) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d; Expected error=%d\n", err, expected_err);
			return -1;
		}
		
		/* oh_print_textbuffer: Default textbuffer testcase */
		printf("Default TextBuffer\n");
		err = oh_print_textbuffer(&default_textbuffer, 1);
		if (err != SA_OK) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d\n", err);
			return -1;
		}

		/* oh_print_textbuffer: Normal textbuffer testcase */
		printf("Normal TextBuffer\n");
		err = oh_print_textbuffer(&textbuffer, 1);
		if (err != SA_OK) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d\n", err);
			return -1;
		}
	}

	/************************** 
	 * oh_print_event testcases
         **************************/	
	{
		SaHpiEventT sensor_event, default_event;
		
		sensor_event.Source = 1;
		sensor_event.EventType = SAHPI_ET_SENSOR;
		sensor_event.Severity = SAHPI_CRITICAL;
		sensor_event.EventDataUnion.SensorEvent.SensorNum = 2;
		sensor_event.EventDataUnion.SensorEvent.SensorType = SAHPI_VOLTAGE;
		sensor_event.EventDataUnion.SensorEvent.EventCategory = SAHPI_EC_THRESHOLD;
		sensor_event.EventDataUnion.SensorEvent.Assertion = SAHPI_TRUE;
		sensor_event.EventDataUnion.SensorEvent.EventState = SAHPI_ES_LOWER_MINOR;
		sensor_event.EventDataUnion.SensorEvent.OptionalDataPresent = sensor_event.EventDataUnion.SensorEvent.OptionalDataPresent
			| SAHPI_SOD_TRIGGER_READING
			| SAHPI_SOD_TRIGGER_THRESHOLD
			| SAHPI_SOD_PREVIOUS_STATE
			| SAHPI_SOD_CURRENT_STATE
			| SAHPI_SOD_OEM
			| SAHPI_SOD_SENSOR_SPECIFIC;
		sensor_event.EventDataUnion.SensorEvent.TriggerReading.IsSupported = SAHPI_TRUE;
		sensor_event.EventDataUnion.SensorEvent.TriggerReading.Type = SAHPI_SENSOR_READING_TYPE_INT64;
		sensor_event.EventDataUnion.SensorEvent.TriggerReading.Value.SensorInt64 = 100;
		sensor_event.EventDataUnion.SensorEvent.TriggerThreshold.IsSupported = SAHPI_TRUE;
		sensor_event.EventDataUnion.SensorEvent.TriggerThreshold.Type = SAHPI_SENSOR_READING_TYPE_INT64;
		sensor_event.EventDataUnion.SensorEvent.TriggerThreshold.Value.SensorInt64 = 101;
		sensor_event.EventDataUnion.SensorEvent.PreviousState = SAHPI_ES_LOWER_MINOR | SAHPI_ES_LOWER_MAJOR;
		sensor_event.EventDataUnion.SensorEvent.CurrentState = SAHPI_ES_LOWER_MINOR;
		sensor_event.EventDataUnion.SensorEvent.Oem = 32;
		sensor_event.EventDataUnion.SensorEvent.SensorSpecific = 33;

		/* oh_print_event: Bad parameter testcase */
		expected_err = SA_ERR_HPI_INVALID_PARAMS;

		err = oh_print_event(0, 0);
		if (err != expected_err) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d; Expected error=%d\n", err, expected_err);
			return -1;
		}
		
		/* oh_print_event: Default event testcase */
		printf("Default Event\n");
		err = oh_print_event(&default_event, 1);
		if (err != SA_OK) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d\n", err);
			return -1;
		}

		/* oh_print_event: Normal sensor event testcase */
		printf("Normal Sensor Event\n");
		err = oh_print_event(&sensor_event, 1);
		if (err != SA_OK) {
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%d\n", err);
			return -1;
		}
	}


	/*******************************
	 * oh_valid_textbuffer testcases
         *******************************/
	{
		SaHpiTextBufferT buffer;
		SaHpiBoolT result, expected_result;

		/* oh_valid_textbuffer: NULL buffer testcase */
		expected_result = SAHPI_FALSE;
		result = oh_valid_textbuffer(0);
		if (result != expected_result) {	
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			return -1;
		}

		/* oh_valid_textbuffer: Bad text type testcase */
		expected_result = SAHPI_FALSE;
		oh_init_textbuffer(&buffer);
		buffer.DataType = BAD_TYPE;
		
		result = oh_valid_textbuffer(&buffer);
		if (result != expected_result) {	
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			return -1;
		}

		/* oh_valid_textbuffer: Bad language type testcase */
		expected_result = SAHPI_FALSE;
		oh_init_textbuffer(&buffer);
		buffer.Language = BAD_TYPE;
		
		result = oh_valid_textbuffer(&buffer);
		if (result != expected_result) {	
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			return -1;
		}
		
		/* oh_valid_textbuffer: Bad Unicode length testcase */
		expected_result = SAHPI_FALSE;
		oh_init_textbuffer(&buffer);
		buffer.DataType = SAHPI_TL_TYPE_UNICODE;
		strcpy(buffer.Data, "123");
		buffer.DataLength = strlen("123");

		result = oh_valid_textbuffer(&buffer);
		if (result != expected_result) {	
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			return -1;
		}
                
                /* oh_valid_textbuffer: Bad Data text cases */
                expected_result = SAHPI_FALSE;
		oh_init_textbuffer(&buffer);
		buffer.DataType = SAHPI_TL_TYPE_BCDPLUS;
		strcpy(buffer.Data, "123");
                buffer.Data[1] = ';';
		buffer.DataLength = strlen("123");

		result = oh_valid_textbuffer(&buffer);
		if (result != expected_result) {	
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			return -1;
		}
                
                buffer.DataType = SAHPI_TL_TYPE_ASCII6;
                buffer.Data[1] = 0xff;
                result = oh_valid_textbuffer(&buffer);
		if (result != expected_result) {	
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			return -1;
		}
		
		/* oh_valid_textbuffer: Good buffer testcase */
		expected_result = SAHPI_TRUE;
		oh_init_textbuffer(&buffer);

		result = oh_valid_textbuffer(&buffer);
		if (result != expected_result) {	
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			return -1;
		}
	}

	/******************************
	 * oh_valid_threshold testcases
         ******************************/
	{
		SaHpiSensorThresholdsT default_thresholds_int64, test_thresholds_int64;
		SaHpiSensorThresholdsT default_thresholds_float64, test_thresholds_float64;
		SaHpiSensorThresholdsT default_thresholds_uint64, test_thresholds_uint64;
		SaHpiSensorDataFormatT default_format_int64, test_format_int64;
		SaHpiSensorDataFormatT default_format_float64, test_format_float64;
		SaHpiSensorDataFormatT default_format_uint64, test_format_uint64;
		SaHpiSensorThdMaskT default_writable_thresholds, test_writable_thresholds;
		
		default_thresholds_int64.UpCritical.IsSupported = SAHPI_TRUE;
		default_thresholds_int64.UpCritical.Type = SAHPI_SENSOR_READING_TYPE_INT64;
		default_thresholds_int64.UpCritical.Value.SensorInt64 = 50;
		default_thresholds_int64.UpMajor.IsSupported = SAHPI_TRUE;
		default_thresholds_int64.UpMajor.Type = SAHPI_SENSOR_READING_TYPE_INT64;
		default_thresholds_int64.UpMajor.Value.SensorInt64 = 40;
		default_thresholds_int64.UpMinor.IsSupported = SAHPI_TRUE;
		default_thresholds_int64.UpMinor.Type = SAHPI_SENSOR_READING_TYPE_INT64;
		default_thresholds_int64.UpMinor.Value.SensorInt64 = 30;
		default_thresholds_int64.LowMinor.IsSupported = SAHPI_TRUE;
		default_thresholds_int64.LowMinor.Type = SAHPI_SENSOR_READING_TYPE_INT64;
		default_thresholds_int64.LowMinor.Value.SensorInt64 = 20;
		default_thresholds_int64.LowMajor.IsSupported = SAHPI_TRUE;
		default_thresholds_int64.LowMajor.Type = SAHPI_SENSOR_READING_TYPE_INT64;
		default_thresholds_int64.LowMajor.Value.SensorInt64 = 10;
		default_thresholds_int64.LowCritical.IsSupported = SAHPI_TRUE;
		default_thresholds_int64.LowCritical.Type = SAHPI_SENSOR_READING_TYPE_INT64;
		default_thresholds_int64.LowCritical.Value.SensorInt64 = 0;
		default_thresholds_int64.PosThdHysteresis.IsSupported = SAHPI_TRUE;
		default_thresholds_int64.PosThdHysteresis.Type = SAHPI_SENSOR_READING_TYPE_INT64;
		default_thresholds_int64.PosThdHysteresis.Value.SensorInt64 = 2;
		default_thresholds_int64.NegThdHysteresis.IsSupported = SAHPI_TRUE;
		default_thresholds_int64.NegThdHysteresis.Type = SAHPI_SENSOR_READING_TYPE_INT64;
		default_thresholds_int64.NegThdHysteresis.Value.SensorInt64 = 2;

		default_format_int64.IsSupported = SAHPI_TRUE;
		default_format_int64.ReadingType = SAHPI_SENSOR_READING_TYPE_INT64;
		default_format_int64.Range.Flags = SAHPI_SRF_MAX | SAHPI_SRF_MIN;
		default_format_int64.Range.Max.Value.SensorInt64 = 60;
		default_format_int64.Range.Min.Value.SensorInt64 = 0;

		default_writable_thresholds = 
			SAHPI_STM_LOW_MINOR | SAHPI_STM_LOW_MAJOR | SAHPI_STM_LOW_CRIT |
			SAHPI_STM_UP_MINOR | SAHPI_STM_UP_MAJOR |  SAHPI_STM_UP_CRIT |
			SAHPI_STM_UP_HYSTERESIS | SAHPI_STM_LOW_HYSTERESIS;

		/* oh_valid_threshold: Bad parameters testcase */
		expected_err = SA_ERR_HPI_INVALID_PARAMS;
		err = oh_valid_threshold(0, 0, default_writable_thresholds);
		if (err != expected_err) {	
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%s\n", oh_lookup_error(err));
			return -1;
		}
		
		/* oh_valid_threshold: Bad threshold type testcase */
		test_thresholds_int64 = default_thresholds_int64;
		test_format_int64 = default_format_int64;
		test_writable_thresholds = default_writable_thresholds;
		
		expected_err = SA_ERR_HPI_INVALID_CMD;
		test_thresholds_int64.LowCritical.Type = SAHPI_SENSOR_READING_TYPE_FLOAT64;
		
		err = oh_valid_threshold(&test_thresholds_int64, &test_format_int64, test_writable_thresholds);
		if (err != expected_err) {	
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%s\n", oh_lookup_error(err));
			return -1;
		}

		/* oh_valid_threshold: Bad text buffer type threshold testcase */
		test_thresholds_int64 = default_thresholds_int64;
		test_format_int64 = default_format_int64;
		test_writable_thresholds = default_writable_thresholds;
		
		expected_err = SA_ERR_HPI_INVALID_CMD;
		test_thresholds_int64.LowCritical.Type = SAHPI_SENSOR_READING_TYPE_BUFFER;
		
		err = oh_valid_threshold(&test_thresholds_int64, &test_format_int64, test_writable_thresholds);
		if (err != expected_err) {	
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%s\n", oh_lookup_error(err));
			return -1;
		}

		/* oh_valid_threshold: Bad threshold hysteresis testcase */
		test_thresholds_int64 = default_thresholds_int64;
		test_format_int64 = default_format_int64;
		test_writable_thresholds = default_writable_thresholds;
		
		expected_err = SA_ERR_HPI_INVALID_DATA;
		test_thresholds_int64.PosThdHysteresis.Value.SensorInt64 = -1;
		
		err = oh_valid_threshold(&test_thresholds_int64, &test_format_int64, test_writable_thresholds);
		if (err != expected_err) {	
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%s\n", oh_lookup_error(err));
			return -1;
		}

		/* oh_valid_threshold: Bad range threshold testcase */
		test_thresholds_int64 = default_thresholds_int64;
		test_format_int64 = default_format_int64;
		test_writable_thresholds = default_writable_thresholds;
		
		expected_err = SA_ERR_HPI_INVALID_CMD;
		test_format_int64.Range.Max.Value.SensorInt64 = 0;
		
		err = oh_valid_threshold(&test_thresholds_int64, &test_format_int64, test_writable_thresholds);
		if (err != expected_err) {	
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%s\n", oh_lookup_error(err));
			return -1;
		}
		
		/* oh_valid_threshold: Bad order threshold testcase */
		test_thresholds_int64 = default_thresholds_int64;
		test_format_int64 = default_format_int64;
		test_writable_thresholds = default_writable_thresholds;

		test_thresholds_int64.LowCritical.Value.SensorInt64 = 20;
		expected_err = SA_ERR_HPI_INVALID_DATA;
		
		err = oh_valid_threshold(&test_thresholds_int64, &test_format_int64, test_writable_thresholds);
		if (err != expected_err) {	
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%s\n", oh_lookup_error(err));
			return -1;
		}

		/* oh_valid_threshold: Bad writable threshold testcase */
		test_thresholds_int64 = default_thresholds_int64;
		test_format_int64 = default_format_int64;
		test_writable_thresholds = default_writable_thresholds;

		test_writable_thresholds = SAHPI_STM_LOW_MINOR | SAHPI_STM_LOW_MAJOR | SAHPI_STM_LOW_CRIT;
		expected_err = SA_ERR_HPI_INVALID_CMD;
		
		err = oh_valid_threshold(&test_thresholds_int64, &test_format_int64, test_writable_thresholds);
		if (err != expected_err) {	
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%s\n", oh_lookup_error(err));
			return -1;
		}

		/* oh_valid_threshold: Normal threshold testcase - int64 */
		test_thresholds_int64 = default_thresholds_int64;
		test_format_int64 = default_format_int64;
		test_writable_thresholds = default_writable_thresholds;

		expected_err = SA_OK;
		
		err = oh_valid_threshold(&test_thresholds_int64, &test_format_int64, test_writable_thresholds);
		if (err != expected_err) {	
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%s\n", oh_lookup_error(err));
			return -1;
		}

		/* oh_valid_threshold: Normal subset testcase */
		test_thresholds_int64 = default_thresholds_int64;
		test_format_int64 = default_format_int64;
		test_writable_thresholds = default_writable_thresholds;

		test_thresholds_int64.UpCritical.IsSupported = SAHPI_FALSE;
		test_thresholds_int64.UpCritical.Type = BAD_TYPE; /* This should be ignored */
		test_thresholds_int64.LowCritical.IsSupported = SAHPI_FALSE;
		test_thresholds_int64.LowCritical.Type = BAD_TYPE; /* This should be ignored */

		expected_err = SA_OK;
		
		err = oh_valid_threshold(&test_thresholds_int64, &test_format_int64, test_writable_thresholds);
		if (err != expected_err) {	
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%s\n", oh_lookup_error(err));
			return -1;
		}

 		default_thresholds_float64.UpCritical.IsSupported = SAHPI_TRUE;
		default_thresholds_float64.UpCritical.Type = SAHPI_SENSOR_READING_TYPE_FLOAT64;
		default_thresholds_float64.UpCritical.Value.SensorFloat64 = 50.3;
		default_thresholds_float64.UpMajor.IsSupported = SAHPI_TRUE;
		default_thresholds_float64.UpMajor.Type = SAHPI_SENSOR_READING_TYPE_FLOAT64;
		default_thresholds_float64.UpMajor.Value.SensorFloat64 = 40.2;
		default_thresholds_float64.UpMinor.IsSupported = SAHPI_TRUE;
		default_thresholds_float64.UpMinor.Type = SAHPI_SENSOR_READING_TYPE_FLOAT64;
		default_thresholds_float64.UpMinor.Value.SensorFloat64 = 30.1;
		default_thresholds_float64.LowMinor.IsSupported = SAHPI_TRUE;
		default_thresholds_float64.LowMinor.Type = SAHPI_SENSOR_READING_TYPE_FLOAT64;
		default_thresholds_float64.LowMinor.Value.SensorFloat64 = 20.3;
		default_thresholds_float64.LowMajor.IsSupported = SAHPI_TRUE;
		default_thresholds_float64.LowMajor.Type = SAHPI_SENSOR_READING_TYPE_FLOAT64;
		default_thresholds_float64.LowMajor.Value.SensorFloat64 = 10.2;
		default_thresholds_float64.LowCritical.IsSupported = SAHPI_TRUE;
		default_thresholds_float64.LowCritical.Type = SAHPI_SENSOR_READING_TYPE_FLOAT64;
		default_thresholds_float64.LowCritical.Value.SensorFloat64 = 0.5;
		default_thresholds_float64.PosThdHysteresis.IsSupported = SAHPI_TRUE;
		default_thresholds_float64.PosThdHysteresis.Type = SAHPI_SENSOR_READING_TYPE_FLOAT64;
		default_thresholds_float64.PosThdHysteresis.Value.SensorFloat64 = 2;
		default_thresholds_float64.NegThdHysteresis.IsSupported = SAHPI_TRUE;
		default_thresholds_float64.NegThdHysteresis.Type = SAHPI_SENSOR_READING_TYPE_FLOAT64;
		default_thresholds_float64.NegThdHysteresis.Value.SensorFloat64 = 2;

		default_format_float64.IsSupported = SAHPI_TRUE;
		default_format_float64.ReadingType = SAHPI_SENSOR_READING_TYPE_FLOAT64;
		default_format_float64.Range.Flags = SAHPI_SRF_MAX | SAHPI_SRF_MIN;
		default_format_float64.Range.Max.Value.SensorFloat64 = 60;
		default_format_float64.Range.Min.Value.SensorFloat64 = 0;

                /* oh_valid_threshold: Normal threshold testcase - float64 */
		test_thresholds_float64 = default_thresholds_float64;
		test_format_float64 = default_format_float64;
		test_writable_thresholds = default_writable_thresholds;

		expected_err = SA_OK;
		
		err = oh_valid_threshold(&test_thresholds_float64, &test_format_float64, test_writable_thresholds);
		if (err != expected_err) {	
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%s\n", oh_lookup_error(err));
			return -1;
		}

 		default_thresholds_uint64.UpCritical.IsSupported = SAHPI_TRUE;
		default_thresholds_uint64.UpCritical.Type = SAHPI_SENSOR_READING_TYPE_UINT64;
		default_thresholds_uint64.UpCritical.Value.SensorUint64 = 50.3;
		default_thresholds_uint64.UpMajor.IsSupported = SAHPI_TRUE;
		default_thresholds_uint64.UpMajor.Type = SAHPI_SENSOR_READING_TYPE_UINT64;
		default_thresholds_uint64.UpMajor.Value.SensorUint64 = 40.2;
		default_thresholds_uint64.UpMinor.IsSupported = SAHPI_TRUE;
		default_thresholds_uint64.UpMinor.Type = SAHPI_SENSOR_READING_TYPE_UINT64;
		default_thresholds_uint64.UpMinor.Value.SensorUint64 = 30.1;
		default_thresholds_uint64.LowMinor.IsSupported = SAHPI_TRUE;
		default_thresholds_uint64.LowMinor.Type = SAHPI_SENSOR_READING_TYPE_UINT64;
		default_thresholds_uint64.LowMinor.Value.SensorUint64 = 20.3;
		default_thresholds_uint64.LowMajor.IsSupported = SAHPI_TRUE;
		default_thresholds_uint64.LowMajor.Type = SAHPI_SENSOR_READING_TYPE_UINT64;
		default_thresholds_uint64.LowMajor.Value.SensorUint64 = 10.2;
		default_thresholds_uint64.LowCritical.IsSupported = SAHPI_TRUE;
		default_thresholds_uint64.LowCritical.Type = SAHPI_SENSOR_READING_TYPE_UINT64;
		default_thresholds_uint64.LowCritical.Value.SensorUint64 = 0.5;
		default_thresholds_uint64.PosThdHysteresis.IsSupported = SAHPI_TRUE;
		default_thresholds_uint64.PosThdHysteresis.Type = SAHPI_SENSOR_READING_TYPE_UINT64;
		default_thresholds_uint64.PosThdHysteresis.Value.SensorUint64 = 2;
		default_thresholds_uint64.NegThdHysteresis.IsSupported = SAHPI_TRUE;
		default_thresholds_uint64.NegThdHysteresis.Type = SAHPI_SENSOR_READING_TYPE_UINT64;
		default_thresholds_uint64.NegThdHysteresis.Value.SensorUint64 = 2;

		default_format_uint64.IsSupported = SAHPI_TRUE;
		default_format_uint64.ReadingType = SAHPI_SENSOR_READING_TYPE_UINT64;
		default_format_uint64.Range.Flags = SAHPI_SRF_MAX | SAHPI_SRF_MIN;
		default_format_uint64.Range.Max.Value.SensorUint64 = 60;
		default_format_uint64.Range.Min.Value.SensorUint64 = 0;

		/* oh_valid_threshold: Normal threshold testcase - uint64*/
		test_thresholds_uint64 = default_thresholds_uint64;
		test_format_uint64 = default_format_uint64;
		test_writable_thresholds = default_writable_thresholds;

		expected_err = SA_OK;
		
		err = oh_valid_threshold(&test_thresholds_uint64, &test_format_uint64, test_writable_thresholds);
		if (err != expected_err) {	
			printf("  Error! Testcase failed. Line=%d\n", __LINE__);
			printf("  Received error=%s\n", oh_lookup_error(err));
			return -1;
		}
	}

	return(0);
}
