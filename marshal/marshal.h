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

#ifndef dMarshal_h
#define dMarshal_h

#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif


// TODO use (u)int_xxx_t or SaHpi types instead
typedef unsigned char      tUint8;
typedef unsigned short     tUint16;
typedef unsigned int       tUint32;
typedef unsigned long long tUint64;
typedef char               tInt8;
typedef short              tInt16;
typedef int                tInt32;
typedef long long          tInt64;
typedef float              tFloat32;
typedef double             tFloat64;


typedef enum 
{
  eMtUnknown,
  eMtVoid,          // do nothing
  eMtUint8,
  eMtUint16,
  eMtUint32,
  eMtUint64,
  eMtInt8,
  eMtInt16,
  eMtInt32,
  eMtInt64,
  eMtFloat32,
  eMtFloat64,
  eMtArray,
  eMtVarArray,
  eMtStruct,
  eMtStructElement,
  eMtUnion,
  eMtUnionElement,
  eMtUserDefined    // user defined marshalling
} tMarshalType;

// NB
//
// VarArray can be used as a structure element only 
// The struct must have "number of elements" field before the VarArray field
//
// Union can be used as a structure element only 
// The struct must have "union modifier" field before the Union field
//


// helper macro for arrays
#define dArray( name, nelements, element_type, element )  \
{                                               \
  .m_type = eMtArray,                           \
  .m_name = name,                               \
  .u.m_array =                                  \
  {                                             \
    .m_nelements      = nelements,              \
    .m_element_sizeof = sizeof( element_type ), \
    .m_element        = &element                \
  }                                             \
}

// helper macro for var arrays
#define dVarArray( name, nelements_idx, element_type, element )  \
{                                               \
  .m_type = eMtVarArray,                        \
  .m_name = name,                               \
  .u.m_var_array =                              \
  {                                             \
    .m_nelements_idx  = nelements_idx,          \
    .m_element_sizeof = sizeof( element_type ), \
    .m_element        = &element                \
  }                                             \
}

// helper marco for struct
#define dStruct( elements ) \
{                              \
  .m_type = eMtStruct,         \
  .m_name = #elements,         \
  .u.m_struct =                \
  {                            \
    .m_elements = &elements[0] \
  }                            \
}

// helper marco for struct elements
#define dStructElement( struct_type, field, element ) \
{                                                \
  .m_type = eMtStructElement,                    \
  .m_name = #field,                              \
  .u.m_struct_element =                          \
  {                                              \
    .m_offset  = offsetof( struct_type, field ), \
    .m_element = &element                        \
  }                                              \
}

// helper marco for struct end
#define dStructElementEnd() \
{                            \
  .m_type = eMtUnknown,      \
  .m_name = ""               \
}

// helper marco for unions
#define dUnion( mod_idx, elements ) \
{                               \
  .m_type = eMtUnion,           \
  .m_name = #elements,          \
  .u.m_union =                  \
  {                             \
    .m_mod_idx = mod_idx,       \
    .m_elements  = &elements[0] \
  }                             \
}

// helper marco for union elements
#define dUnionElement( mod, element ) \
{                                     \
  .m_type = eMtUnionElement,          \
  .m_name = #mod,                     \
  .u.m_union_element =                \
  {                                   \
    .m_mod     = mod,                 \
    .m_element = &element             \
  }                                   \
}

// helper marco for union end
#define dUnionElementEnd() \
{                          \
  .m_type = eMtUnknown,    \
  .m_name = ""             \
}

// helper macro for user define mashaller
#define dUserDefined( marshaller, demarshaller, user_data )    \
{                                     \
  .m_type = eMtUserDefined,           \
  .m_name = "user",                   \
  .u.m_user_defined =                 \
  {                                   \
    .m_marshaller   = marshaller,     \
    .m_demarshaller = demarshaller,   \
    .m_user_data    = user_data       \
  }                                   \
}


struct sMarshalType;
typedef struct sMarshalType cMarshalType;

typedef int (*tMarshalFunction)( const cMarshalType *type, const void *data,
				 void *buffer, void *user_data );
typedef int (*tDemarshalFunction)( int byte_order, const cMarshalType *type, void *data,
				  const void *buffer, void *user_data );

struct sMarshalType
{
  tMarshalType m_type;
  const char * m_name;

  union
  {
    struct
    {
      size_t         m_nelements;
      size_t         m_element_sizeof;
      cMarshalType  *m_element;
    } m_array;

    struct
    {
      size_t         m_nelements_idx; // nelements field ids in parent struct
      size_t         m_element_sizeof;
      cMarshalType  *m_element;
    } m_var_array;

    struct
    {
      cMarshalType  *m_elements;
    } m_struct;

    struct
    {
      size_t         m_offset; // element offset in parent struct
      cMarshalType  *m_element;
    } m_struct_element;

    struct
    {
      size_t         m_mod_idx;   // mod field index in parent struct
      cMarshalType  *m_elements;
    } m_union;

    struct
    {
      size_t         m_mod;
      cMarshalType  *m_element;
    } m_union_element;

    struct
    {
      tMarshalFunction   m_marshaller;
      tDemarshalFunction m_demarshaller;
      void              *m_user_data;
    } m_user_defined;
  } u;
};


// some simple types
extern cMarshalType Marshal_VoidType;
extern cMarshalType Marshal_Uint8Type;
extern cMarshalType Marshal_Uint16Type;
extern cMarshalType Marshal_Uint32Type;
extern cMarshalType Marshal_Uint64Type;
extern cMarshalType Marshal_Int8Type;
extern cMarshalType Marshal_Int16Type;
extern cMarshalType Marshal_Int32Type;
extern cMarshalType Marshal_Int64Type;
extern cMarshalType Marshal_Float32Type;
extern cMarshalType Marshal_Float64Type;


// marshal data into buffer
int Marshal( const cMarshalType *type, const void *data, void  *buffer );
int MarshalArray( const cMarshalType **types, const void **data, void *buffer );

// demarshal buffer into data
int Demarshal( int byte_order, const cMarshalType *type, void *data, const void *buffer );
int DemarshalArray( int byte_order, const cMarshalType **types, void **data, const void *buffer );


#ifdef __cplusplus
}
#endif

#endif
