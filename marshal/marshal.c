/*
 * marshaling/demarshaling
 *
 * Copyright (c) 2004 by FORCE Computers.
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
 *     W. David Ashley <dashley@us.ibm.com.com>
 *     Anton Pak <anton.pak.pigeonpoint.com>
 */

#include <assert.h>
#include <stdlib.h>
//#include <stdio.h>
#include <string.h>

#include <glib.h>

#include "marshal.h"


cMarshalType Marshal_VoidType =
{
  .m_type = eMtVoid
};

cMarshalType Marshal_Uint8Type =
{
  .m_type = eMtUint8
};

cMarshalType Marshal_Uint16Type =
{
  .m_type = eMtUint16
};

cMarshalType Marshal_Uint32Type =
{
  .m_type = eMtUint32
};

cMarshalType Marshal_Uint64Type =
{
  .m_type = eMtUint64
};

cMarshalType Marshal_Int8Type =
{
  .m_type = eMtInt8
};

cMarshalType Marshal_Int16Type =
{
  .m_type = eMtInt16
};

cMarshalType Marshal_Int32Type =
{
  .m_type = eMtInt32
};

cMarshalType Marshal_Int64Type =
{
  .m_type = eMtInt64
};

cMarshalType Marshal_Float32Type =
{
  .m_type = eMtFloat32
};

cMarshalType Marshal_Float64Type =
{
  .m_type = eMtFloat64
};



static int
IsSimpleType( tMarshalType type )
{
  switch( type )
     {
       case eMtVoid:
       case eMtInt8:
       case eMtUint8:
       case eMtInt16:
       case eMtUint16:
       case eMtInt32:
       case eMtUint32:
       case eMtInt64:
       case eMtUint64:
       case eMtFloat32:
       case eMtFloat64:
	    return 1;

       case eMtArray:
       case eMtVarArray:
       case eMtStruct:
       case eMtStructElement:
       case eMtUnion:
       case eMtUnionElement:
       case eMtUserDefined:
	    return 0;

       default:
	    assert( 0 );
	    return 0;
     }
}


size_t
MarshalSize( const cMarshalType *type )
{
  switch( type->m_type )
     {
       case eMtVoid:
	    return 0;

       case eMtInt8:
	    return sizeof(tInt8);
       case eMtUint8:
	    return sizeof(tUint8);
       case eMtInt16:
	    return sizeof(tInt16);
       case eMtUint16:
	    return sizeof(tUint16);
       case eMtInt32:
	    return sizeof(tInt32);
       case eMtUint32:
	    return sizeof(tUint32);
       case eMtInt64:
	    return sizeof(tInt64);
       case eMtUint64:
	    return sizeof(tUint64);
       case eMtFloat32:
	    return sizeof(tFloat32);
       case eMtFloat64:
	    return sizeof(tFloat64);

       case eMtArray:
	    {
	      size_t nelems = type->u.m_array.m_nelements;
	      assert( nelems > 0 );
	      const cMarshalType *elem = type->u.m_array.m_element;
	      assert( elem );

	      return nelems * MarshalSize( elem );
	    }

       case eMtVarArray:
	    // TODO investigate and fix me if necessary
	    return 0xffff;

       case eMtStruct:
	    {
	      size_t size = 0;

	      const cMarshalType *elems = &type->u.m_struct.m_elements[0];
	      assert( elems );

	      size_t i;
	      for( i = 0; elems[i].m_type == eMtStructElement; i++ )
		 {
		   const cMarshalType *elem = elems[i].u.m_struct_element.m_element;
		   size += MarshalSize( elem );
		 }

	      return size;
	    }

       case eMtUnion:
	    {
	      size_t max = 0;

	      const cMarshalType *elems = &type->u.m_union.m_elements[0];
	      assert( elems );

	      size_t i;
	      for( i = 0; elems[i].m_type == eMtUnionElement; i++ )
		 {
		   const cMarshalType *elem = elems[i].u.m_union_element.m_element;
		   size_t s = MarshalSize( elem );
		   if ( max < s )
		      {
			max = s;
		      }
		 }

	      return max;
	    }

       case eMtUserDefined:
	    // TODO investigate and fix me if necessary
	    return 0xffff;

       default: // including eMtUnknown, eMtStructElement, eMtUnionElement
	    assert( 0 );
	    return 0;
     }
}


size_t
MarshalSizeArray( const cMarshalType **types )
{
  size_t size = 0;

  size_t i;
  for( i = 0; types[i]; i++ )
     {
       size += MarshalSize( types[i] );
     }

  return size;
}


static int
MarshalSimpleType( tMarshalType type, const void *data, void *buffer )
{
  switch( type )
     {
       case eMtVoid:
	    return 0;

       case eMtInt8:
	    memcpy(buffer, data, sizeof(tInt8));
	    return sizeof(tInt8);
       case eMtUint8:
	    memcpy(buffer, data, sizeof(tUint8));
	    return sizeof(tUint8);
       case eMtInt16:
	    memcpy(buffer, data, sizeof(tInt16));
	    return sizeof(tInt16);
       case eMtUint16:
	    memcpy(buffer, data, sizeof(tUint16));
	    return sizeof(tUint16);
       case eMtInt32:
	    memcpy(buffer, data, sizeof(tInt32));
	    return sizeof(tInt32);
       case eMtUint32:
	    memcpy(buffer, data, sizeof(tUint32));
	    return sizeof(tUint32);
       case eMtInt64:
	    memcpy(buffer, data, sizeof(tInt64));
	    return sizeof(tInt64);
       case eMtUint64:
	    memcpy(buffer, data, sizeof(tUint64));
	    return sizeof(tUint64);
       case eMtFloat32:
	    memcpy(buffer, data, sizeof(tFloat32));
	    return sizeof(tFloat32);
       case eMtFloat64:
	    memcpy(buffer, data, sizeof(tFloat64));
	    return sizeof(tFloat64);
       default:
	    assert( 0 );
	    return -1;
     }
}

/***********************************************************
 * Gets value of integer structure element
 * @type - marshal type of the structure
 * @idx  - element index in the structure
 * @data - raw pointer to the structure data
 *
 * structure element shall be of
 * eMtUint{8,16,32} and eMtInt{8,16,32} type
 *
 * returns obtained integer value (casted to size_t)
 * or
 * returns (size_t)(-1) and throws assertion
 *
 * NB
 * function does NOT check for
 * - type is valid pointer
 * - type is pointer to marshal type for a structure
 * - idx is valid for the type
 * - data is valid pointer
 ***********************************************************/
static size_t
GetStructElementIntegerValue( const cMarshalType *type, size_t idx, const void * data )
{
  const cMarshalType *elems = &type->u.m_struct.m_elements[0];
  const cMarshalType *elem = elems[idx].u.m_struct_element.m_element;
  const size_t offset      = elems[idx].u.m_struct_element.m_offset;

  union
  {
    const void    *raw;
    const tInt8   *i8;
    const tUint8  *ui8;
    const tInt16  *i16;
    const tUint16 *ui16;
    const tInt32  *i32;
    const tUint32 *ui32;
    const tInt64  *i64;
    const tUint64 *ui64;
  } u;

  u.raw = (const unsigned char *)data + offset;

  switch( elem->m_type )
     {
       case eMtInt8:
	    return (size_t)(*u.i8);
       case eMtUint8:
	    return (size_t)(*u.ui8);
       case eMtInt16:
	    return (size_t)(*u.i16);
       case eMtUint16:
	    return (size_t)(*u.ui16);
       case eMtInt32:
	    return (size_t)(*u.i32);
       case eMtUint32:
	    return (size_t)(*u.ui32);
       case eMtInt64:
	    return (size_t)(*u.i64);
       case eMtUint64:
	    return (size_t)(*u.ui64);
       default:
	    assert( 0 );
	    return (size_t)(-1);
     }
}

/***********************************************************
 * Gets marshal type for union element specified by modifier
 * @type      - marshal type of the union
 * @modifier  - modifier value
 *
 * returns union element or 0
 *
 * NB
 * function does NOT check for
 * - type is valid pointer
 * - type is pointer to marshal type for a union
 ***********************************************************/
static const cMarshalType *
GetUnionElement( const cMarshalType *type, size_t modifier )
{
  const cMarshalType *elems = &type->u.m_union.m_elements[0];

  size_t i;
  for( i = 0; elems[i].m_type == eMtUnionElement; i++ )
     {
       const size_t mod = elems[i].u.m_union_element.m_mod;
       if ( mod == modifier )
	  {
	    const cMarshalType *elem = elems[i].u.m_union_element.m_element;
	    return elem;
	  }
     }

  return 0;
}

int
Marshal( const cMarshalType *type, const void *d, void *b )
{
  if ( IsSimpleType( type->m_type ) )
     {
       return MarshalSimpleType( type->m_type, d, b );
     }

  int                  size   = 0;
  const unsigned char *data   = d;
  unsigned char       *buffer = b;

  switch( type->m_type )
     {
       case eMtArray:
	    {
	      const size_t nelems = type->u.m_array.m_nelements;
	      assert( nelems > 0 );

	      size_t i;
	      for( i = 0; i < nelems; i++ )
		 {
	           const cMarshalType *elem = type->u.m_array.m_element;
	           const size_t elem_sizeof = type->u.m_array.m_element_sizeof;

		   int s = Marshal( elem, data, buffer );
		   if ( s < 0 )
		      {
			   assert( 0 );
			   return -1;
		      }

		   data   += elem_sizeof;
		   buffer += s;
		   size   += s;
		 }
	    }
	    break;

       case eMtStruct:
	    {
	      const cMarshalType *elems = &type->u.m_struct.m_elements[0];
	      assert( elems );

	      size_t i;
	      for( i = 0; elems[i].m_type == eMtStructElement; i++ )
		 {
		   const cMarshalType *elem = elems[i].u.m_struct_element.m_element;
		   const size_t offset      = elems[i].u.m_struct_element.m_offset;
		   int s = 0;

		   if ( elem->m_type == eMtUnion )
		      {
			size_t mod2_idx = elem->u.m_union.m_mod_idx;
			// the union modifier field must be before this entry.
			// this is a limitation of demarshaling of unions
			assert( mod2_idx < i );
			size_t mod2 = GetStructElementIntegerValue( type, mod2_idx, data );
			const cMarshalType *elem2 = GetUnionElement( elem, mod2 );
			assert( elem2 );

			s = Marshal( elem2, data + offset, buffer );
			if ( s < 0 )
			   {
			     assert( 0 );
			     return -1;
			   }
		      }
		   else if ( elem->m_type == eMtVarArray )
		      {
			const size_t nelems2_idx   = elem->u.m_var_array.m_nelements_idx;
			const cMarshalType *elem2  = elem->u.m_var_array.m_element;
			const size_t elem2_sizeof  = elem->u.m_var_array.m_element_sizeof;

			// the array nelements field must be before this entry.
			// this is a limitation of demarshaling of var arrays
			assert( nelems2_idx < i );

			const size_t nelems2 = GetStructElementIntegerValue( type, nelems2_idx, data );

			// (data + offset ) points to pointer to var array content
			const unsigned char *data2;
			memcpy(&data2, data + offset, sizeof(void *));

			unsigned char *buffer2 = buffer;
			size_t i2;
			for( i2 = 0; i2 < nelems2; i2++ )
			   {
			     int s2 = Marshal( elem2, data2, buffer2 );
			     if ( s2 < 0 )
				{
				  assert( 0 );
				  return -1;
				}

			     data2   += elem2_sizeof;
			     buffer2 += s2;
			     s       += s2;
			   }
		      }
		   else
		      {
			s = Marshal( elem, data + offset, buffer );
			if ( s < 0 )
			   {
			     assert( 0 );
			     return -1;
			   }
		      }

		   buffer += s;
		   size   += s;
		 }
	    }
	    break;

       case eMtUnion:
	    assert( 0 );
	    return -1;

       case eMtUserDefined:
	    {
	      tMarshalFunction marshaller = type->u.m_user_defined.m_marshaller;
	      assert( marshaller );
	      void * user_data = type->u.m_user_defined.m_user_data;
	      size = marshaller( type, d, b, user_data );
            }
	    break;

       default:
	    assert( 0 );
	    return -1;
     }

  return size;
}


int
MarshalArray( const cMarshalType **types, const void **data, void *b )
{
  int            size = 0;
  unsigned char *buffer = b;

  size_t i;
  for( i = 0; types[i]; i++ )
     {
       int s = Marshal( types[i], data[i], buffer );
       if ( s < 0 )
	  {
	    assert( 0 );
	    return -1;
	  }

       size   += s;
       buffer += s;
     }

  return size;
}


static int
DemarshalSimpleTypes( int byte_order, tMarshalType type, void *data, const void *buffer )
{
  union
  {
    tInt16    i16;
    tUint16   ui16;
    tInt32    i32;
    tUint32   ui32;
    tInt64    i64;
    tUint64   ui64;
    tFloat32  f32;
    tFloat64  f64;
  } u;

  // NB Glib does not provide SWAP_LE_BE macro for signed types!

  switch( type )
     {
       case eMtVoid:
	    return 0;

       case eMtInt8:
	    memcpy(data, buffer, sizeof(tInt8));
	    return sizeof(tInt8);

       case eMtUint8:
	    memcpy(data, buffer, sizeof(tUint8));
	    return sizeof(tUint8);

       case eMtInt16:
	    memcpy(&u.i16, buffer, sizeof(tInt16));
	    if ( G_BYTE_ORDER != byte_order )
	       {
	         u.ui16 = GUINT16_SWAP_LE_BE( u.ui16 );
	       }
	    memcpy(data, &u.i16, sizeof(tInt16));
	    return sizeof(tInt16);

       case eMtUint16:
	    memcpy(&u.ui16, buffer, sizeof(tUint16));
	    if ( G_BYTE_ORDER != byte_order )
	       {
	         u.ui16 = GUINT16_SWAP_LE_BE( u.ui16 );
	       }
	    memcpy(data, &u.ui16, sizeof(tUint16));
	    return sizeof(tUint16);

       case eMtInt32:
	    memcpy(&u.i32, buffer, sizeof(tInt32));
	    if ( G_BYTE_ORDER != byte_order )
	       {
	         u.ui32 = GUINT32_SWAP_LE_BE( u.ui32 );
	       }
	    memcpy(data, &u.i32, sizeof(tInt32));
	    return sizeof(tInt32);
	
       case eMtUint32:
	    memcpy(&u.ui32, buffer, sizeof(tUint32));
	    if ( G_BYTE_ORDER != byte_order )
	       {
	         u.ui32 = GUINT32_SWAP_LE_BE( u.ui32 );
	       }
	    memcpy(data, &u.ui32, sizeof(tUint32));
	    return sizeof(tUint32);

       case eMtInt64:
	    memcpy(&u.i64, buffer, sizeof(tInt64));
	    if ( G_BYTE_ORDER != byte_order )
	       {
	         u.ui64 = GUINT64_SWAP_LE_BE( u.ui64 );
	       }
	    memcpy(data, &u.i64, sizeof(tInt64));
	    return sizeof(tInt64);
	
       case eMtUint64:
	    memcpy(&u.ui64, buffer, sizeof(tUint64));
	    if ( G_BYTE_ORDER != byte_order )
	       {
	         u.ui64 = GUINT64_SWAP_LE_BE( u.ui64 );
	       }
	    memcpy(data, &u.ui64, sizeof(tUint64));
	    return sizeof(tUint64);
	
       case eMtFloat32:
	
	    memcpy(&u.f32, buffer, sizeof(tFloat32));
	    if ( G_BYTE_ORDER != byte_order )
	       {
	         u.ui32 = GUINT32_SWAP_LE_BE( u.ui32 );
	       }
	    memcpy(data, &u.f32, sizeof(tFloat32));
	    return sizeof(tFloat32);
	
       case eMtFloat64:
	    memcpy(&u.f64, buffer, sizeof(tFloat64));
	    if ( G_BYTE_ORDER != byte_order )
	       {
	         u.ui64 = GUINT64_SWAP_LE_BE( u.ui64 );
	       }
	    memcpy(data, &u.f64, sizeof(tFloat64));
	    return sizeof(tFloat64);
	
       default:
	    assert( 0 );
	    return -1;
     }
}


int
Demarshal( int byte_order, const cMarshalType *type, void *d, const void *b )
{
  if ( IsSimpleType( type->m_type ) )
     {
       return DemarshalSimpleTypes( byte_order, type->m_type, d, b );
     }

  int                  size = 0;
  unsigned char       *data  = d;
  const unsigned char *buffer = b;

  switch( type->m_type )
     {
       case eMtArray:
	    {
	      const size_t nelems = type->u.m_array.m_nelements;
	      assert( nelems > 0 );

	      size_t i;
	      for( i = 0; i < nelems; i++ )
		 {
	           const cMarshalType *elem = type->u.m_array.m_element;
	           const size_t elem_sizeof = type->u.m_array.m_element_sizeof;

		   int s = Demarshal( byte_order, elem, data, buffer );
		   if ( s < 0 )
		      {
			   assert( 0 );
			   return -1;
		      }

		   data   += elem_sizeof;
		   buffer += s;
		   size   += s;
		 }
	    }
	    break;

       case eMtStruct:
	    {
	      const cMarshalType *elems = &type->u.m_struct.m_elements[0];
	      assert( elems );

	      size_t i;
	      for( i = 0; elems[i].m_type == eMtStructElement; i++ )
		 {
		   const cMarshalType *elem = elems[i].u.m_struct_element.m_element;
		   const size_t offset      = elems[i].u.m_struct_element.m_offset;
		   int s = 0;

		   if ( elem->m_type == eMtUnion )
		      {
			size_t mod2_idx = elem->u.m_union.m_mod_idx;
			// the union modifier field must be before this entry.
			// this is a limitation of demarshaling of unions
			assert( mod2_idx < i );
			size_t mod2 = GetStructElementIntegerValue( type, mod2_idx, data );
			const cMarshalType *elem2 = GetUnionElement( elem, mod2 );
			assert( elem2 );

			s = Demarshal( byte_order, elem2, data + offset, buffer );
			if ( s < 0 )
			   {
			     assert( 0 );
			     return -1;
			   }
		      }
		   else if ( elem->m_type == eMtVarArray )
		      {
			const size_t nelems2_idx   = elem->u.m_var_array.m_nelements_idx;
			const cMarshalType *elem2  = elem->u.m_var_array.m_element;
			const size_t elem2_sizeof  = elem->u.m_var_array.m_element_sizeof;

			// the array nelements field must be before this entry.
			// this is a limitation of demarshaling of var arrays
			assert( nelems2_idx < i );

			const size_t nelems2 = GetStructElementIntegerValue( type, nelems2_idx, data );

			// allocate storage for var array content
			unsigned char *data2 = (unsigned char *)g_malloc0( nelems2 * elem2_sizeof );
			// (data + offset ) points to pointer to var array content
			memcpy(data + offset, &data2, sizeof(void *));

			const unsigned char *buffer2 = buffer;
			size_t i2;
			for( i2 = 0; i2 < nelems2; i2++ )
			   {
			     int s2 = Demarshal( byte_order, elem2, data2, buffer2 );
			     if ( s2 < 0 )
				{
				  assert( 0 );
				  return -1;
				}

			     data2   += elem2_sizeof;
			     buffer2 += s2;
			     s       += s2;
			   }
		      }
		   else
		      {
			s = Demarshal( byte_order, elem, data + offset, buffer );
			if ( s < 0 )
			   {
			     assert( 0 );
			     return -1;
			   }
		      }

		   buffer += s;
		   size   += s;
		 }
	    }

	    break;

       case eMtUnion:
	    // unions must me encapsulate in structs
	    assert( 0 );
	    return -1;

       case eMtStructElement:
       case eMtUnionElement:
	    assert( 0 );
	    return -1;

       case eMtUserDefined:
	    {
	      tDemarshalFunction demarshaller = type->u.m_user_defined.m_demarshaller;
	      assert( demarshaller );
	      void * user_data = type->u.m_user_defined.m_user_data;
	      size = demarshaller( byte_order, type, d, b, user_data );
            }
	    break;

       default:
	    assert( 0 );
	    return -1;
     }

  return size;
}


int
DemarshalArray( int byte_order, const cMarshalType **types, void **data, const void *b )
{
  int size = 0;
  const unsigned char *buffer = b;

  size_t i;
  for( i = 0; types[i]; i++ )
     {
       int s = Demarshal( byte_order, types[i], data[i], buffer );
       if ( s < 0 )
	  {
	    assert( 0 );
	    return -1;
	  }

       size   += s;
       buffer += s;
     }

  return size;
}

