#ifndef _SOCKET_HOST_
#define _SOCKET_HOST_

#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <map>
#include "SocketServer.h"
#include "TcpStream.h"
#include "IConnectable.h"

/**
 * The master socket used to host a number of
 * threaded child sockets.
 */
class SocketHost : public IConnectable {
private:
    std::map<std::string, int> library;
    std::thread hostThread;
    TCPServerSocket * _server_;
    std::mutex hostMtx;
    bool _exit_;
    int _port_;

public:
    SocketHost(int port);
    
    ~SocketHost();
    
    int getLocalPort();
    
    void publish(std::string name, IConnectable *node);
    
    void unpublish(std::string name);

    void setShouldExit(bool flag);

    bool getShouldExit();

    void startListening();

    int getPublishedPort(std::string line);
};

#endif