#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include "SocketServer.h"
#include "TcpStream.h"
#include "IConnectable.h"

class ServerA : public IConnectable {
private:
    std::thread serverThread;
    TCPServerSocket * server;
    int _port_;
    
    void startListening(TCPServerSocket & socket);
    
public:
    ServerA();
    
    ~ServerA();
    
    int getLocalPort();
    
    int waitEndServer();
};