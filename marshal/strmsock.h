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
 *      W. David Ashley <dashley@us.ibm.com>
 *
 */


#ifndef STRMSOCK_H_INCLUDED

#define STRMSOCK_H_INCLUDED

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif


// The Stream Sockets class
class strmsock
   {
   protected:
   int            s;                  // the client socket handle
   unsigned long  ulBufSize;          // the read buffer size
   void          *pBuf;               // the read buffer
   int            domain;             // the socket domain
   int            type;               // the socket type
   int            protocol;           // the socket protocol
   int            errcode;            // errno contents      
   bool           fOpen;              // open connection indicator

   public:
   virtual ~strmsock               () { };
   virtual void  Close             (void);
   virtual int   GetErrcode        (void);
   virtual char *Read              (void);
   virtual void  SetBufferSize     (unsigned long);
   virtual void  SetDomain         (int);
   virtual void  SetProtocol       (int);
   virtual void  SetType           (int);
   virtual void  WriteBin          (const void *, int);
   virtual void  WriteStr          (const char *);
   };
typedef strmsock *pstrmsock;

// the Client Stream Sockets class
class cstrmsock : public strmsock
   {
   public:
   cstrmsock                       ();
   ~cstrmsock                      ();
   bool          Open              (const char *, int);
   };
typedef cstrmsock *pcstrmsock;

// the Server Stream Sockets class
class sstrmsock : public strmsock
   {
   protected:
   char          acHostName[256];    // host name
   int           ss;                 // the server socket handle
   int           backlog;            // listen queue size
   struct        sockaddr_in addr;   // address structure
   bool          fOpenS;             // TRUE = valid server socket

   public:
   sstrmsock                       ();
   sstrmsock                       (const sstrmsock&);
   ~sstrmsock                      ();
   void          CloseSrv          (void);
   bool          Create            (int);
   bool          Accept            (void);
   };
typedef sstrmsock *psstrmsock;


#endif  // STRMSOCK_H_INCLUDED__

