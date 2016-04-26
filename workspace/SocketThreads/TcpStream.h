/**
  *	Created by TekuConcept on Mar 29, 2016
  */

#ifndef _TCP_STREAM_
#define _TCP_STREAM_

#include <streambuf>
#include <stdio.h>
#include <string>
#include "SocketServer.h"

class TcpStream : private std::streambuf, public iostream
{
	static const unsigned int BUF_SIZE = 64;
	char* outputBuffer_;
	char* inputBuffer_;
	TCPSocket *socket;

public:
	TcpStream(TCPSocket *node);
	int getPort();
	int sync();
    int underflow();
	virtual ~TcpStream();
};

#endif