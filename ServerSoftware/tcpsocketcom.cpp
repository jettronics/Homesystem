
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <iostream>
#include <fcntl.h>

#include "procmessage.h"
#include "tcpsocketcom.h"


TcpSocketCom::TcpSocketCom(int port, DirType dir)
    : sockfd(0)
    , newsockfd(0) 
    , portno(port) //51717
    , clilen(0)
    , initfailed(false)
    , dir(dir)
{
 
}

TcpSocketCom::TcpSocketCom(ProcMessage *procMsg, int port, DirType dir)
    : sockfd(0)
    , newsockfd(0) 
    , portno(port) // 51717
    , clilen(0)
    , initfailed(false)
    , procMsg(procMsg)
    , dir(dir)
{
}

TcpSocketCom::~TcpSocketCom()
{

}

int TcpSocketCom::init()
{
    cout << "\nUsing port " << portno << "\n";

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    {
        cout << "\nSocket Error: " << sockfd;
        initfailed = true;
        return -1;
    }
    
    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons( portno );
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
    {
        cout << "\nBind Error\n";
        initfailed = true;
        return -1;
    }
    listen(sockfd,5);
    clilen = sizeof(cli_addr); 
    
    cout << "\nWaiting for new client...\n";
    if ( ( newsockfd = accept( sockfd, (struct sockaddr *) &cli_addr, (socklen_t*) &clilen) ) < 0 )
    {
        cout << "\nAccept Error: " << newsockfd;
        cout.flush();
        return -1;
    }
    
    cout << "\nOpened new communication with client\n";
    
    cout << "None blocking TCP handler\n";
    fcntl(newsockfd, F_SETFL, O_NONBLOCK);
    
    cout.flush();
    
    initfailed = false;   
    return 0;
}

void TcpSocketCom::deInit()
{
    close( newsockfd );
}

void TcpSocketCom::process()
{
    if( initfailed == false )
    {
#if 0        
        while( 1 )
        {
            cout << "\nWaiting for new client...\n";
            cout.flush();
            if ( ( newsockfd = accept( sockfd, (struct sockaddr *) &cli_addr, (socklen_t*) &clilen) ) < 0 )
            {
                cout << "\nAccept Error: " << newsockfd;
                cout.flush();
                sleep(1);
                break;
            }
            else
            {
                cout << "\nOpened new communication with client\n";
                cout.flush();
            
                while( 1 ) 
                {
#endif     
                    if( dir == SendToServer )
                    {
                        string recData = getData();
                        //cout << "\nData received: " << recData;
                        //cout.flush();
                        if( recData != "Empty" )
                        {
                            procMsg->sendClientToServer(recData);
                        }
                        else
                        {
                            //break;                        
                        }
                    }
                    else
                    if( dir == SendToClient )
                    {
                        string outData = procMsg->receiveClientFromServer();
                        if( outData.length() > 0 )
                        {
                            sendData(outData);
                        }
                    }
                    else
                    {
                        // ToDo
                    }
#if 0                     
                    usleep(50000);
                   
                }

            }
        }
#endif
        
    }
    return;
}

string TcpSocketCom::getData() 
{
    char buffer[100];
    int n = 0;

    n = read(newsockfd, buffer, 100);

    if ( n < 0 )
    {
        //cout << "\nRead Error: " << n;
        //buffer[0] = '\0';
        strcpy(buffer,"Empty\0");
    }
    else
    if( n == 0 )
    {
        //cout << "\nRead Empty: " << n;
        strcpy(buffer,"Empty\0");
    }
    else
    {
        buffer[n] = '\0';
    }

    //cout << "\nSocket read: " << buffer;
    //cout.flush();
    return buffer;    
}

void TcpSocketCom::sendData(string send)
{
    int n;

    cout << "\nsend: " << send << "\n";
    if ( (n = write( newsockfd, send.data(), send.length() ) ) < 0 )
    {
        cout << "\nWrite Error: " << n;
    }
    cout.flush();
    return;
}
