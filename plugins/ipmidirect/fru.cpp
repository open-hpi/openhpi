/*
 * fru.cpp
 *
 * Copyright (c) 2003,2004 by FORCE Computers
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     Thomas Kanngieser <thomas.kanngieser@fci.com>
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "ipmi.h"


SaErrorT
cIpmi::IfGetInventorySize( cIpmiFru *fru, SaHpiUint32T &size )
{
  size = fru->GetSize();
  assert( size );

  return SA_OK;
}


SaErrorT
cIpmi::IfGetInventoryInfo( cIpmiFru *fru, SaHpiInventoryDataT &data )
{
  assert( fru->GetSize() );

  fru->GetInventoryInfo( data );

  return SA_OK;
}
