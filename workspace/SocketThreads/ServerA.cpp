#include "ServerA.h"

#define INFO(x) std::cerr << x << std::endl

ServerA::ServerA() {
    server = new TCPServerSocket(0);
    _port_ = server->getLocalPort();
    std::cout << "CHILDA: Listening on threaded port: " << _port_ << std::endl;
    startListening(*server);
}

ServerA::~ServerA() {
    INFO("C-SERVER-A: called destructor");
    if(serverThread.joinable())
        serverThread.join();
    INFO("C-SERVER-A: joined server thread");
    if(server != NULL)
        delete server;
    INFO("C-SERVER-A: deleted server");
}

int ServerA::getLocalPort() {
    return _port_;
}

void ServerA::startListening(TCPServerSocket & socket) {
    // spawn server thread
    serverThread = std::thread([&socket](){
        // wait for clients to connect
        std::mutex serverMutex;
        bool shouldExit = false;
        // loop until a client calls the exit command
        while(!shouldExit) {
            try {
                TCPSocket *client;
                try {
                    client = socket.accept();
                } catch(SocketException e) { }
                
                if(client != NULL) {
                    // spawn new thread for each new client
                    std::thread([&client, &socket,
                        &serverMutex, &shouldExit](){
                        
                        std::cout << "CN-SERVER-A: Started client thread." << std::endl;
                        std::string line;
                        std::iostream * stream = new TcpStream(client);
                        
                        // while(line != "EOL") {
                            *stream >> line;
                            std::cerr << "CN-SERVER-A: " << line << std::endl;
                            if(line == "exit") {
                                serverMutex.lock();
                                shouldExit = true;
                                delete stream;
                                delete client;
                                std::cout << "- END OF CN-SERVER-A THREAD [exit] -" << std::endl;
                                socket.shutdown(); // <- thread stop here
                                //serverMutex.unlock();
                                //return;
                            }
                            /*else {
                                // send line and stream to
                                // underlying object
                            }
                        }*/
                        
                        delete stream;
                        delete client;
                        std::cout << "- END OF C1-SERVER-A THREAD -" << std::endl;
                    }).detach();
                }
            } catch(SocketException e) {
                std::cout << e.what() << std::endl;
            }
        }
        std::cerr << "- END OF SERVER THREAD -" << std::endl;
    });
}

int ServerA::waitEndServer() {
    if(serverThread.joinable()) {
        serverThread.join();
        return 0;
    }
    return 1;
}