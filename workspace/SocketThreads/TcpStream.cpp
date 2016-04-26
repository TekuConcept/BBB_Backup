/**
  *	Created by TekuConcept on Apr 20, 2016
  */

#include "TcpStream.h"

TcpStream::TcpStream(TCPSocket *node) :
    std::iostream(this),
    outputBuffer_(new char[BUF_SIZE+1]),
    inputBuffer_(new char[BUF_SIZE+1]),
    socket(node)
{
	setp( outputBuffer_, outputBuffer_ + BUF_SIZE - 1 );
    setg( inputBuffer_, inputBuffer_ + BUF_SIZE - 1, inputBuffer_ + BUF_SIZE - 1 );
}



TcpStream::~TcpStream() {
	if(socket != NULL)
	{
		socket = NULL;
	}
}



int TcpStream::getPort() {
	return socket->getLocalPort();
}



int TcpStream::sync() {
    socket->send( pbase(), int(pptr() - pbase()) );
    setp(pbase(), epptr());
    return 0;
}



int TcpStream::underflow() {
    int bytesReceived = socket->recv(eback(), BUF_SIZE);
    setg(eback(), eback(), eback()+bytesReceived);
    return *eback();
}