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

 // current version
#define dMhVersion 1

#define dMhGetVersion(flags) (((flags)>>4) & 0x7)

// message flags
#define dMhEndianBit	1

#define dMhRequest   0
#define dMhReply     1
#define dMhError     2  // only valid on server replies

// max message length including header
#define dMaxMessageLength 0xffff

// cMessageHeader::m_type
typedef enum
{
	eMhPing = 1,
	eMhOpen,
	eMhClose,
	eMhMsg,
} tMessageType;

typedef struct
{
	unsigned char	m_type;
	unsigned char	m_flags; // bit 0-3 flags, bit 4-7 version
	unsigned int	m_id;
	unsigned int	m_len;
} cMessageHeader;

// The Stream Sockets class
class strmsock
{
	protected:
	int			s;		 // the client socket handle
	unsigned long		ulBufSize;	// the read buffer size
	void			*pBuf;		// the read buffer
	int			domain;		// the socket domain
	int			type;		// the socket type
	int			protocol;		// the socket protocol
	int			errcode;		// errno contents
	bool			fOpen;		// open connection indicator

	public:
        cMessageHeader	header;		// the message header
	virtual		~strmsock		() { };
	virtual void	Close			(void);
	virtual int	GetErrcode		(void);
	virtual void	SetBufferSize		(unsigned long);
	virtual void	SetDomain		(int);
	virtual void	SetProtocol		(int);
	virtual void	SetType			(int);
	virtual void	MessageHeaderInit	(tMessageType, unsigned char,
		   				 unsigned int, unsigned int);
	virtual cMessageHeader	*GetHeader	(void);
	virtual void	ConWriteMsg		(const void *);
	virtual void	*ConReadMsg		(void);
//	virtual int	ConHandleMsg		(void);
};
typedef strmsock *pstrmsock;

// the Client Stream Sockets class
class cstrmsock : public strmsock
{
	public:
	cstrmsock		();
	~cstrmsock		();
	bool Open		(const char *, int);
	void    ClientWriteMsg	(const void *);
	void    *ClientReadMsg	(void);
};
typedef cstrmsock *pcstrmsock;

// the Server Stream Sockets class
class sstrmsock : public strmsock
{
	protected:
	char	acHostName[256];	 // host name
	int	ss;			// the server socket handle
	int	backlog;			// listen queue size
	struct	sockaddr_in addr;		// address structure
	bool	fOpenS;		 	// TRUE = valid server socket

	public:
	sstrmsock		();
	sstrmsock		(const sstrmsock&);
	~sstrmsock		();
	void    CloseSrv		(void);
	bool    Create		(int);
	bool    Accept		(void);
	void    ServerWriteMsg	(const void *);
	void    *ServerReadMsg	(void);
};
typedef sstrmsock *psstrmsock;


#endif  // STRMSOCK_H_INCLUDED__
