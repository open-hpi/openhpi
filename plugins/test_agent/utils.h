/*      -*- c++ -*-
 *
 * (C) Copyright Pigeon Point Systems. 2011
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTUTILSLITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *        Anton Pak <anton.pak@pigeonpoint.com>
 */

#ifndef UTILS_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010
#define UTILS_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include <SaHpi.h>


namespace TA {


/**************************************************************
 * uint16_t <-> uint8_t
 *************************************************************/
inline uint16_t MakeUint16T( uint8_t high, uint8_t low )
{
    return ( ( (uint16_t)high ) << 8 ) | ( (uint16_t)low );
}

inline uint8_t GetHighByte( uint16_t x ) {
    return (uint8_t)( x >> 8 );
}

inline uint8_t GetLowByte( uint16_t x ) {
    return (uint8_t)( x & 0xFFU );
}


/**************************************************************
 * Cast Templates
 *************************************************************/
template <typename T>
static inline const T * ConstPtr( const void * ptr )
{
    return reinterpret_cast<const T *>(ptr);
}

template <typename T>
static inline T * Ptr( void * ptr )
{
    return reinterpret_cast<T *>(ptr);
}

template <typename T>
static inline const T& ConstRef( const void * ptr )
{
    return *ConstPtr<T>( ptr );
}

template <typename T>
static inline T& Ref( void * ptr )
{
    return *Ptr<T>( ptr );
}


/**************************************************************
 * cLocker: class for for locking object while in scope
 *************************************************************/
template <class T>
class cLocker
{
public:

    explicit cLocker( T * t )
        : m_t( t )
    {
        m_t->Lock();
    }
    ~cLocker()
    {
        m_t->Unlock();
    }

private:
    cLocker( const cLocker& );
    cLocker& operator =( const cLocker& );

private: // data

    T * m_t;
};


/**************************************************************
 * Entity Path Helpers
 *************************************************************/
void MakeUnspecifiedHpiEntityPath( SaHpiEntityPathT& ep );


/**************************************************************
 * Text Buffer Helpers
 *************************************************************/
void MakeHpiTextBuffer( SaHpiTextBufferT& tb, const char * s );
void MakeHpiTextBuffer( SaHpiTextBufferT& tb, const char * s, size_t size );
void MakeHpiTextBuffer( SaHpiTextBufferT& tb, char c, size_t size );
void FormatHpiTextBuffer( SaHpiTextBufferT& tb, const char * fmt, ... );
void vFormatHpiTextBuffer( SaHpiTextBufferT& tb, const char * fmt, va_list ap );
void AppendToTextBuffer( SaHpiTextBufferT& tb, const SaHpiTextBufferT& appendix );


}; // namespace TA


#endif // UTILS_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

