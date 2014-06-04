//============================================================================
// Name        : SocketServer.cpp
// Author      : TekuConcept
// Version     : 1.0
// Copyright   : Free to use. Give credit where credit is due!
// Description : Socket server in C++, Ansi-style
//============================================================================

#include "ThrusterControl.h"

#include <iostream>
#include <netdb.h>
#define MAXHOSTNAME 256

void processRequest(char data[], int size, char oData[], int oSize, int &nOut);
int sta = 0;

int main()
{
    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr;
    const int BUFF = 32;
    char ioBuff[BUFF];
    int RESP = 1024;
    int ROUT = 0;
    char response[RESP];

    // create socket and flush memory blocks
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    BinaryUtils::memset(&serv_addr, '0', sizeof(serv_addr));
    BinaryUtils::memset(ioBuff, (char)0x00, sizeof(ioBuff));
    BinaryUtils::memset(ioBuff, (char)0x00, sizeof(ioBuff));

    // set socket properties
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 0.0.0.0
    serv_addr.sin_port = htons(8421);

    // Initialize and start the socket
    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    listen(listenfd, 10);

    while(1)
    {
        // receive incoming connection
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);

        // continue to read and write until '~' is received
        while(true)
        {
            // read data from client
            sta = read(connfd, ioBuff, BUFF);

            processRequest(ioBuff, BUFF, response, RESP, ROUT);

            // write data to client
            if(ioBuff[0] != '~')
            	sta = write(connfd, response, ROUT);
            else break;

        }

        // close connection and repeat
        close(connfd);
        sleep(10);
    }
}

//int readTempurature();
void processRequest(char data[], int size, char oData[], int oSize, int &nOut)
{
	std::cout << "Message Received: " << data << "\n";
	if(data[0] == 'T' || data[0] == 't')
	{
		//data[0] = readTempurature();
	}
}
