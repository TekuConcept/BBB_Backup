#include "SocketHost.h"

#define INFO(x) std::cerr << x << std::endl

SocketHost::SocketHost(int port) {
    _server_ = new TCPServerSocket(port);
    // get server port
    // NOTE: This is not guranteed to be the same as the
    // function parameter: "port", rather, it is the actual
    // port of the socket server.
    _port_ = _server_->getLocalPort();
    std::cerr << "SERVER: Listening on threaded Port: " << _port_ << std::endl;
    //startListening(*server, library);
}



SocketHost::~SocketHost() {
    INFO("SERVER: called destructor");
    if(hostThread.joinable())
        hostThread.join();
    INFO("SERVER: joined thread");
    if(_server_ != NULL) {
        delete _server_;
        _server_ = NULL;
    }
    INFO("SERVER: deleted server");
}



void SocketHost::startListening() {
    // spawn server thread
    hostThread = std::thread([this](std::map<std::string, int> * lib){
        // wait for clients to connect
        std::mutex serverMutex;
        bool shouldExit = false;
        std::vector<std::thread> children;
        
        // // loop until a client calls the exit command
        while(!getShouldExit()) {
            try {
                TCPSocket * _client_;
                try {
                    _client_ = _server_->accept();
                } catch(SocketException e) { }
                
                if(_client_ != NULL)
                    children.push_back(std::thread([this](TCPSocket * client){
                        std::cout << "SERVER: Started client thread." << std::endl;
                        std::string line;
                        std::iostream * stream = new TcpStream(client);
                        
                        *stream >> line;
                        std::cout << line << std::endl;
                        if(line == "exit") {
                            std::cout << "- END OF H-CLIENT THREAD [exit] -" << std::endl;
                            setShouldExit(true);
                            std::cout << "Unreachable line breached!" << std::endl;
                        }
                        else {
                            int oport = getPublishedPort(line);
                            *stream << oport << std::endl;
                        }

                        delete stream;
                        delete client;
                        std::cout << "- END OF H-CLIENT THREAD -" << std::endl;
                    }, _client_));

                _client_ = NULL;
            } catch(SocketException e) {
                std::cout << e.what() << std::endl;
            }
        }

        // clean-up
        for(int i = 0; i < children.size(); i++) {
            if(children[i].joinable())
                children[i].join();
        }
        std::cout << "- END OF SERVER THREAD -" << std::endl;
    }, &library);
}



void SocketHost::setShouldExit(bool flag) {
    hostMtx.lock();
    _exit_ = flag;
    _server_->shutdown();
    hostMtx.unlock();
}



bool SocketHost::getShouldExit() {
    bool flag;
    hostMtx.lock();
    flag = _exit_;
    hostMtx.unlock();
    return flag;
}



int SocketHost::getPublishedPort(std::string line) {
    int oport;
    hostMtx.lock();
    if ( library.find(line) == library.end() ) oport = -1;
    else oport = library[line];
    hostMtx.unlock();
    return oport;
}



int SocketHost::getLocalPort() {
    int port;
    hostMtx.lock();
    port = _port_;
    hostMtx.unlock();
    return port;
}



void SocketHost::publish(std::string name, IConnectable *node) {
    if ( library.find(name) == library.end() ) {
        int port = node->getLocalPort();
        library[name] = port;
    } else {
        std::cerr << "Node's name already exists." << std::endl;
    }
}



void SocketHost::unpublish(std::string name) {
    if ( library.find(name) == library.end() ) return;
    else library.erase(name);
}