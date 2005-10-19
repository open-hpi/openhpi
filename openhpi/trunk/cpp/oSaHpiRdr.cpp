/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2005
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *    W. David Ashley <dashley@us.ibm.com>
 */


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
extern "C"
{
#include <SaHpi.h>
}
#include "oSaHpiTypesEnums.hpp"
#include "oSaHpiCtrlRec.hpp"
#include "oSaHpiSensorRec.hpp"
#include "oSaHpiInventoryRec.hpp"
#include "oSaHpiWatchdogRec.hpp"
#include "oSaHpiAnnunciatorRec.hpp"
#include "oSaHpiEntityPath.hpp"
#include "oSaHpiTextBuffer.hpp"
#include "oSaHpiRdr.hpp"


/**
 * Default constructor.
 */
oSaHpiRdr::oSaHpiRdr() {
    RecordId = 1;
    RdrType = SAHPI_NO_RECORD;
    Entity.Entry[0].EntityType = SAHPI_ENT_ROOT;
    Entity.Entry[0].EntityLocation = 0;
    IsFru = false;
    // no need to initialize the RdrTypeUnion since there is no record
    IdString.DataType = SAHPI_TL_TYPE_TEXT;
    IdString.Language = SAHPI_LANG_ENGLISH;
    IdString.DataLength = 0;
    IdString.Data[0] = '\0';
};


/**
 * Constructor.
 *
 * @param buf    The reference to the class to be copied.
 */
oSaHpiRdr::oSaHpiRdr(const oSaHpiRdr& buf) {
    memcpy(this, &buf, sizeof(SaHpiRdrT));
}



/**
 * Assign a field in the SaHpiRdrT struct a value.
 *
 * @param field  The pointer to the struct (class).
 * @param field  The field name as a text string (case sensitive).
 * @param value  The character string value to be assigned to the field. This
 *               value will be converted as necessary.
 *
 * @return True if there was an error, otherwise false.
 */
bool oSaHpiRdr::assignField(SaHpiRdrT *ptr,
                            const char *field,
                            const char *value) {
    if (ptr == NULL || field == NULL || value == NULL) {
        return true;
    }
    if (strcmp(field, "RecordId") == 0) {
        ptr->RecordId = strtoul(value, NULL, 10);
        return false;
    }
    else if (strcmp(field, "RdrType") == 0) {
        ptr->RdrType = oSaHpiTypesEnums::str2rdrtype(value);
        return false;
    }
    // Entity
    else if (strcmp(field, "IsFru") == 0) {
        ptr->IsFru = oSaHpiTypesEnums::str2torf(value);
        return false;
    }
    // RdrTypeUnion
    else if (strcmp(field, "IdString") == 0) {
        IdString.DataType = SAHPI_TL_TYPE_TEXT;
        IdString.Language = SAHPI_LANG_ENGLISH;
        if (strlen(value) < SAHPI_MAX_TEXT_BUFFER_LENGTH) {
            IdString.DataLength = strlen(value);
            strcpy((char *)IdString.Data, value);
        }
        else {
            IdString.DataLength = SAHPI_MAX_TEXT_BUFFER_LENGTH;
            memcpy(IdString.Data, value, SAHPI_MAX_TEXT_BUFFER_LENGTH);
        }
        return false;
    }
    return true;
};


/**
 * Print the contents of the buffer.
 *
 * @param stream Target stream.
 * @param buffer Address of the SaHpiRdrT struct.
 *
 * @return True if there was an error, otherwise false.
 */
bool oSaHpiRdr::fprint(FILE *stream,
                       const int indent,
                       const SaHpiRdrT *buffer) {
	int i, err;
    char buf[20];
    char indent_buf[indent + 1];

    if (stream == NULL || buffer == NULL) {
        return true;
    }
    for (i = 0; i < indent; i++) {
        indent_buf[i] = ' ';
    }
    indent_buf[indent] = '\0';

    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "RecordId = %d\n", buffer->RecordId);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "RdrType = %s\n", oSaHpiTypesEnums::rdrtype2str(buffer->RdrType));
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "Entity\n");
    if (err < 0) {
        return true;
    }
    oSaHpiEntityPath * ep = (oSaHpiEntityPath *)&buffer->Entity;
    err = ep->oSaHpiEntityPath::fprint(stream, indent + 3, (SaHpiEntityPathT *)ep);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "IsFru = %s\n", oSaHpiTypesEnums::torf2str(buffer->IsFru));
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "RdrTypeUnion\n");
    if (err < 0) {
        return true;
    }
    switch (buffer->RdrType) {
    case SAHPI_CTRL_RDR:
        oSaHpiCtrlRec * cr = (oSaHpiCtrlRec *)&buffer->RdrTypeUnion.CtrlRec;
        err = cr->oSaHpiCtrlRec::fprint(stream, indent + 3, (SaHpiCtrlRecT *)cr);
        if (err < 0) {
            return true;
        }
        break;
    case SAHPI_SENSOR_RDR:
        oSaHpiSensorRec * sr = (oSaHpiSensorRec *)&buffer->RdrTypeUnion.SensorRec;
        err = sr->oSaHpiSensorRec::fprint(stream, indent + 3, (SaHpiSensorRecT *)sr);
        if (err < 0) {
            return true;
        }
        break;
    case SAHPI_INVENTORY_RDR:
        oSaHpiInventoryRec * ir = (oSaHpiInventoryRec *)&buffer->RdrTypeUnion.InventoryRec;
        err = ir->oSaHpiInventoryRec::fprint(stream, indent + 3, (SaHpiInventoryRecT *)ir);
        if (err < 0) {
            return true;
        }
        break;
    case SAHPI_WATCHDOG_RDR:
        oSaHpiWatchdogRec * wr = (oSaHpiWatchdogRec *)&buffer->RdrTypeUnion.WatchdogRec;
        err = wr->oSaHpiWatchdogRec::fprint(stream, indent + 3, (SaHpiWatchdogRecT *)wr);
        if (err < 0) {
            return true;
        }
        break;
    case SAHPI_ANNUNCIATOR_RDR:
        oSaHpiAnnunciatorRec * ar = (oSaHpiAnnunciatorRec *)&buffer->RdrTypeUnion.AnnunciatorRec;
        err = ar->oSaHpiAnnunciatorRec::fprint(stream, indent + 3, (SaHpiAnnunciatorRecT *)ar);
        if (err < 0) {
            return true;
        }
        break;
    default:
        err = fprintf(stream, indent_buf);
        if (err < 0) {
            return true;
        }
        err = fprintf(stream, "   No Record\n");
        if (err < 0) {
            return true;
        }
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "IdString\n");
    if (err < 0) {
        return true;
    }
    oSaHpiTextBuffer * tb = (oSaHpiTextBuffer *)&buffer->IdString;
    err = tb->oSaHpiTextBuffer::fprint(stream, indent + 3, (SaHpiTextBufferT *)tb);
    if (err < 0) {
        return true;
    }

	return false;
}

