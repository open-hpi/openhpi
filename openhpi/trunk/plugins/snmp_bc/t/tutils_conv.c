/* -*- linux-c -*-
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
 *     Steve Sherman <stevees@us.ibm.com>
 *     Chris Chia <cchia@users.sf.net>
 */

#include <glib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <SaHpi.h>
#include <snmp_bc_utils.h>

int main(int argc, char **argv) 
{
  	int err;
	SaHpiSensorInterpretedUnionT value, expected_value;
	SaHpiSensorInterpretedTypeT interpreted_type;
	gchar *snmp_string;

	/************************************
	 * Invalid parameter test
	 ************************************/
	interpreted_type = SAHPI_SENSOR_INTERPRETED_TYPE_INT8;
	snmp_string = "  Empty Number Test ";
	expected_value.SensorInt8 = 0;

	err = get_interpreted_value(snmp_string, interpreted_type+12345, &value);
	if (!err) {
		printf("Error! invalid parameter accepted by get_interpreted_value\n");
		return -1;
	}

	/************************************
	 * No numeric value test
	 ************************************/
	interpreted_type = SAHPI_SENSOR_INTERPRETED_TYPE_INT8;
	snmp_string = "  Empty Number Test ";
	expected_value.SensorInt8 = 0;

	err = get_interpreted_value(snmp_string, interpreted_type, &value);
	if (err) {
		printf("Error! Int8 - Positive Sign TestCase\n");
		printf("get_interpreted_value returned error=%d\n", err);
		return -1;
	}

	if (value.SensorInt8 != expected_value.SensorInt8) {
		printf("Error! Int8 - Positive Sign TestCase\n");
		printf ("Convert Value=%d; Expected Value=%d\n",
			value.SensorInt8, expected_value.SensorInt8);
		return -1;
	}

	/************************************
	 * No numeric value test2
	 ************************************/
	interpreted_type = SAHPI_SENSOR_INTERPRETED_TYPE_INT8;
	snmp_string = "Oh No ";
	expected_value.SensorInt8 = 0;

	err = get_interpreted_value(snmp_string, interpreted_type, &value);
	if (err) {
		printf("Error! Int8 - Positive Sign TestCase\n");
		printf("get_interpreted_value returned error=%d\n", err);
		return -1;
	}

	if (value.SensorInt8 != expected_value.SensorInt8) {
		printf("Error! Int8 - Positive Sign TestCase\n");
		printf ("Convert Value=%d; Expected Value=%d\n",
			value.SensorInt8, expected_value.SensorInt8);
		return -1;
	}

	/************************************
	 * Int8 - Positive Sign TestCase
	 ************************************/
	interpreted_type = SAHPI_SENSOR_INTERPRETED_TYPE_INT8;
	snmp_string = " + 127 Volts";
	expected_value.SensorInt8 = 127;

	err = get_interpreted_value(snmp_string, interpreted_type, &value);
	if (err) {
		printf("Error! Int8 - Positive Sign TestCase\n");
		printf("get_interpreted_value returned error=%d\n", err);
		return -1;
	}

	if (value.SensorInt8 != expected_value.SensorInt8) {
		printf("Error! Int8 - Positive Sign TestCase\n");
		printf ("Convert Value=%d; Expected Value=%d\n",
			value.SensorInt8, expected_value.SensorInt8);
		return -1;
	}

	/************************************
	 * Int8 - Positive Sign Overflow TestCase
	 ************************************/
	interpreted_type = SAHPI_SENSOR_INTERPRETED_TYPE_INT8;
	snmp_string = " + 128 Volts";
	expected_value.SensorInt8 = 128;

	err = get_interpreted_value(snmp_string, interpreted_type, &value);
	if (!err) {
		printf("Error! Int8 - Positive Sign Overflow TestCase\n");
		printf("get_interpreted_value returned error=%d\n", err);
		return -1;
	}

	/************************************
	 * Int8 - Negative Sign TestCase
	 ************************************/
	interpreted_type = SAHPI_SENSOR_INTERPRETED_TYPE_INT8;
	snmp_string = " -128 Volts";
	expected_value.SensorInt8 = -128;

	err = get_interpreted_value(snmp_string, interpreted_type, &value);
	if (err) {
		printf("Error! Int8 - Negative Sign TestCase\n");
		printf("get_interpreted_value returned error=%d\n", err);
		return -1;
	}

	if (value.SensorInt8 != expected_value.SensorInt8) {
		printf("Error! Int8 - Negative Sign TestCase\n");
		printf ("Convert Value=%d; Expected Value=%d\n",
			value.SensorInt8, expected_value.SensorInt8);
		return -1;
	}

	/************************************
	 * Int8 - Negative Sign Overflow TestCase
	 ************************************/
	interpreted_type = SAHPI_SENSOR_INTERPRETED_TYPE_INT8;
	snmp_string = " - 129 Volts";

	err = get_interpreted_value(snmp_string, interpreted_type, &value);
	if (!err) {
		printf("Error! Int8 - Negative Sign Overflow TestCase\n");
		printf("get_interpreted_value returned error=%d\n", err);
		return -1;
	}

	/************************************
	 * Int16 - Positive Sign TestCase
	 ************************************/
	interpreted_type = SAHPI_SENSOR_INTERPRETED_TYPE_INT16;
	snmp_string = " +32767 Volts";
	expected_value.SensorInt32 = 32767;

	err = get_interpreted_value(snmp_string, interpreted_type, &value);
	if (err) {
		printf("Error! Int16 - Positive Sign TestCase\n");
		printf("get_interpreted_value returned error=%d\n", err);
		return -1;
	}

	if (value.SensorInt16 != expected_value.SensorInt32) {
		printf("Error! Int16 - Positive Sign TestCase\n");
		printf ("Convert Value=%d; Expected Value=%d\n",
			value.SensorInt16, expected_value.SensorInt32);
		return -1;
	}

	/************************************
	 * Int16 - Positive Sign Overflow TestCase
	 ************************************/
	interpreted_type = SAHPI_SENSOR_INTERPRETED_TYPE_INT16;
	snmp_string = " + 32768 Volts";

	err = get_interpreted_value(snmp_string, interpreted_type, &value);
	if (!err) {
		printf("Error! Int16 - Positive Sign Overflow TestCase\n");
		printf("get_interpreted_value returned error=%d\n", err);
		return -1;
	}

	/************************************
	 * Int16 - Negative Sign TestCase
	 ************************************/
	interpreted_type = SAHPI_SENSOR_INTERPRETED_TYPE_INT16;
	snmp_string = " - 32768 Volts";
	expected_value.SensorInt32 = -32768;

	err = get_interpreted_value(snmp_string, interpreted_type, &value);
	if (err) {
		printf("Error! Int16 - Positive Sign TestCase\n");
		printf("get_interpreted_value returned error=%d\n", err);
		return -1;
	}

	if (value.SensorInt16 != expected_value.SensorInt32) {
		printf("Error! Int16 - Positive Sign TestCase\n");
		printf ("Convert Value=%d; Expected Value=%d\n",
			value.SensorInt16, expected_value.SensorInt32);
		return -1;
	}


	/************************************
	 * Int16 - Negative Sign Overflow TestCase
	 ************************************/
	interpreted_type = SAHPI_SENSOR_INTERPRETED_TYPE_INT16;
	snmp_string = " -32769 Volts";

	err = get_interpreted_value(snmp_string, interpreted_type, &value);
	if (!err) {
		printf("Error! Int16 - Negative Sign Overflow TestCase\n");
		printf("get_interpreted_value returned error=%d\n", err);
		return -1;
	}

	/************************************
	 * Int32 - Positive Sign TestCase
	 ************************************/
	interpreted_type = SAHPI_SENSOR_INTERPRETED_TYPE_INT32;
	snmp_string = "+ 2147483647 Volts";
	expected_value.SensorInt32 = 2147483647;

	err = get_interpreted_value(snmp_string, interpreted_type, &value);
	if (err) {
		printf("Error! Int32 - Positive Sign TestCase\n");
		printf("get_interpreted_value returned error=%d\n", err);
		return -1;
	}

	if (value.SensorInt32 != expected_value.SensorInt32) {
		printf("Error! Int32 - Positive Sign TestCase\n");
		printf ("Convert Value=%d; Expected Value=%d\n",
			value.SensorInt32, expected_value.SensorInt32);
		return -1;
	}

	/************************************
	 * Int32 - Positive Sign Overflow TestCase
	 ************************************/
	interpreted_type = SAHPI_SENSOR_INTERPRETED_TYPE_INT32;
	snmp_string = " + 2147483648 Volts";

	err = get_interpreted_value(snmp_string, interpreted_type, &value);
	if (!err) {
		printf("Error! Int32 - Positive Sign Overflow TestCase\n");
		printf("get_interpreted_value returned error=%d\n", err);
	//	return -1;
	}

	/************************************
	 * Int32 - Negative Sign TestCase
	 ************************************/
	interpreted_type = SAHPI_SENSOR_INTERPRETED_TYPE_INT32;
	snmp_string = " -2147483648 Volts";
	expected_value.SensorInt32 = INT_MIN;

	err = get_interpreted_value(snmp_string, interpreted_type, &value);
	if (err) {
		printf("Error! Int32 - Positive Sign TestCase\n");
		printf("get_interpreted_value returned error=%d\n", err);
		return -1;
	}

	if (value.SensorInt32 != expected_value.SensorInt32) {
		printf("Error! Int32 - Positive Sign TestCase\n");
		printf ("Convert Value=%d; Expected Value=%d\n",
			value.SensorInt32, expected_value.SensorInt32);
		return -1;
	}


	/************************************
	 * Int32 - Negative Sign Overflow TestCase
	 ************************************/
	interpreted_type = SAHPI_SENSOR_INTERPRETED_TYPE_INT32;
	snmp_string = " - 2147483649 Volts";

	err = get_interpreted_value(snmp_string, interpreted_type, &value);
	if (!err) {
		printf("Error! Int32 - Negative Sign Overflow TestCase\n");
		printf("get_interpreted_value returned error=%d\n", err);
		return -1;
	}


	/************************************
	 * Float32 - Positive Sign TestCase
	 ************************************/
	interpreted_type = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32;
	snmp_string = " + 33.33 Volts";
	expected_value.SensorFloat32 = 33.33;

	err = get_interpreted_value(snmp_string, interpreted_type, &value);
	if (err) {
		printf("Error! Float32 - Positive Sign TestCase\n");
		printf("get_interpreted_value returned error=%d\n", err);
		return -1;
	}

	if (value.SensorFloat32 != expected_value.SensorFloat32) {
		printf("Error! Float32 - Positive Sign TestCase\n");
		printf ("Convert Value=%f; Expected Value=%f\n",
			value.SensorFloat32, expected_value.SensorFloat32);
		return -1;
	}

	/**********************************
	 * Float32 - Negative Sign TestCase
	 **********************************/
	interpreted_type = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32;
	snmp_string = " - 3.3 Volts";
	expected_value.SensorFloat32 = -3.3;

	err = get_interpreted_value(snmp_string, interpreted_type, &value);
	if (err) {
		printf("Error! Float32 - Negative Sign TestCase\n");
		printf("get_interpreted_value returned error=%d\n", err);
		return -1;
	}

	if (value.SensorFloat32 != expected_value.SensorFloat32) {
		printf("Error! Float32 - Negative Sign TestCase\n");
		printf ("Convert Value=%f; Expected Value=%f\n",
			value.SensorFloat32, expected_value.SensorFloat32);
		return -1;
	}

	/**************************************
	 * Float32 - No Sign TestCase
	 **************************************/
	interpreted_type = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32;
	snmp_string = "  3.33 Volts";
	expected_value.SensorFloat32 = 3.33;

	err = get_interpreted_value(snmp_string, interpreted_type, &value);
	if (err) {
		printf("Error! Float32 - No Sign TestCase\n");
		printf("get_interpreted_value returned error=%d\n", err);
		return -1;
	}

	if (value.SensorFloat32 != expected_value.SensorFloat32) {
		printf("Error! Float32 - No Sign TestCase\n");
		printf ("Convert Value=%f; Expected Value=%f\n",
			value.SensorFloat32, expected_value.SensorFloat32);
		return -1;
	}

	/**************************************
	 * Float32 - number end with an "v" test
	 **************************************/
	interpreted_type = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32;
	snmp_string = "+2.5v";
	expected_value.SensorFloat32 = 2.5;

	err = get_interpreted_value(snmp_string, interpreted_type, &value);
	if (err) {
		printf("Error! Float32 - No Sign TestCase\n");
		printf("get_interpreted_value returned error=%d\n", err);
		return -1;
	}

	if (value.SensorFloat32 != expected_value.SensorFloat32) {
		printf("Error! Float32 - No Sign TestCase\n");
		printf ("Convert Value=%f; Expected Value=%f\n",
			value.SensorFloat32, expected_value.SensorFloat32);
		return -1;
	}

	/*******************************
	 * Float32 - Percentage TestCase
	 *******************************/
	interpreted_type = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32;
	snmp_string = "    67% of maximum";
	expected_value.SensorFloat32 = 0.67;

	err = get_interpreted_value(snmp_string, interpreted_type, &value);
	if (err) {
		printf("Error! Float32 - Percentage TestCase\n");
		printf("get_interpreted_value returned error=%d\n", err);
		return -1;
	}

	if (value.SensorFloat32 != expected_value.SensorFloat32) {
		printf("Error! Float32 - Percentage TestCase\n");
		printf ("Convert Value=%f; Expected Value=%f\n",
			value.SensorFloat32, expected_value.SensorFloat32);
		return -1;
	}

	/*******************************
	 * Float32 - Percentage TestCase2
	 *******************************/
	interpreted_type = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32;
	snmp_string = "    12.7% of maximum";
	expected_value.SensorFloat32 = 0.127;

	err = get_interpreted_value(snmp_string, interpreted_type, &value);
	if (err) {
		printf("Error! Float32 - Percentage TestCase\n");
		printf("get_interpreted_value returned error=%d\n", err);
		return -1;
	}

	if (value.SensorFloat32 != expected_value.SensorFloat32) {
		printf("Error! Float32 - Percentage TestCase\n");
		printf ("Convert Value=%f; Expected Value=%f\n",
			value.SensorFloat32, expected_value.SensorFloat32);
		return -1;
	}

	/*******************************
	 * Float32 - Percentage TestCase3
	 *******************************/
	interpreted_type = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32;
	snmp_string = "    0.00% of maximum";
	expected_value.SensorFloat32 = 0;

	err = get_interpreted_value(snmp_string, interpreted_type, &value);
	if (err) {
		printf("Error! Float32 - Percentage TestCase\n");
		printf("get_interpreted_value returned error=%d\n", err);
		return -1;
	}

	if (value.SensorFloat32 != expected_value.SensorFloat32) {
		printf("Error! Float32 - Percentage TestCase\n");
		printf ("Convert Value=%f; Expected Value=%f\n",
			value.SensorFloat32, expected_value.SensorFloat32);
		return -1;
	}

	/*******************************
	 * Float32 - Invalid type test
	 *******************************/
	interpreted_type = SAHPI_SENSOR_INTERPRETED_TYPE_INT32;
	snmp_string = "    6.2 of maximum";

	err = get_interpreted_value(snmp_string, interpreted_type, &value);
	if (!err) {
		printf("Error! Float32 - Invalid type should\n");
		printf("get an error from get_interpreted_value\n");
		return -1;
	}

	/*******************************
	 * Float32 - Percentage - Invalid type test
	 *******************************/
	interpreted_type = SAHPI_SENSOR_INTERPRETED_TYPE_INT32;
	snmp_string = "    67% of maximum";
	expected_value.SensorFloat32 = 0.67;

	err = get_interpreted_value(snmp_string, interpreted_type, &value);
	if (!err) {
		printf("Error! Float32 - Percentage TestCase\n");
		printf("get_interpreted_value returned error=%d\n", err);
		return -1;
	}


	return 0;
}
