/**
 *  Created by TekuConcept on Mar 29, 2016
 */

#include "TcpClient.h"

TcpClient::TcpClient() :
    std::iostream(this),
    outputBuffer_(new char[BUF_SIZE + 1]),
    inputBuffer_(new char[BUF_SIZE + 1]),
    connected(false) {
    init_();
}



TcpClient::TcpClient(int port, std::string address) :
    std::iostream(this),
    outputBuffer_(new char[BUF_SIZE + 1]),
    inputBuffer_(new char[BUF_SIZE + 1]),
    connected(false) {
    init_();
    connect(port, address);
}



TcpClient::~TcpClient() {}



void TcpClient::init_() {
    setp(outputBuffer_, outputBuffer_ + BUF_SIZE - 1);
    setg(inputBuffer_, inputBuffer_ + BUF_SIZE - 1, inputBuffer_ + BUF_SIZE - 1);
}



int TcpClient::connect(int port, std::string address) {
    if(connected) return 2;
    try {
        socket = std::make_shared<TCPSocket>(address, port);
        connected = true;
    }
    catch (SocketException &e) {
        std::cerr << "TcpClient: " << e.what() << std::endl;
        connected = false;
        return 1;
    }
    return 0;
}



void TcpClient::disconnect() {
    if (socket != nullptr && connected) {
        socket->disconnect();
        connected = false;
    }
}



bool TcpClient::isConnected() {
    return connected;
}



int TcpClient::sync() {
    int len = int(pptr() - pbase());
    if(socket != nullptr && connected)
        socket->send(pbase(), len);
    setp(pbase(), epptr());
    return 0;
}

int TcpClient::underflow() {
    if(socket != nullptr && connected) {
        int bytesReceived = socket->recv(eback(), BUF_SIZE);
        setg(eback(), eback(), eback() + bytesReceived);
    }
    return *eback();
}