
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
#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h> 

#include "procmessage.h"
#include "namedsocketcom.h"



NamedSocketCom::NamedSocketCom()
    : socket_server(0)
    , socket_accept(0)
    , strOut("")
    , initfailed(false)
{
 
}

NamedSocketCom::NamedSocketCom(ProcMessage *procMsg)
    : procMsg(procMsg)
    , socket_server(0)
    , socket_accept(0)
    , strOut("")
    , initfailed(false)
{
}

NamedSocketCom::~NamedSocketCom()
{

}

void NamedSocketCom::init()
{
    mode_t mode;
    
    cout << "\nStart Server communication\n";
    
    mode = strtol("0666", 0, 8);
    
    // Unix Domain Socket erstellen
    socket_server = socket(AF_UNIX, SOCK_STREAM, 0);
    if (socket_server == -1) 
    {
        cout << "\nError server socket";
        initfailed = true; 
	}

    // Socket an Datei binden
    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, "/var/run/mysocket.sock", sizeof(server_addr.sun_path) - 1);
    unlink(server_addr.sun_path);
    	
    if (bind(socket_server, (struct sockaddr *) &server_addr, sizeof(struct sockaddr_un)) == -1) 
    {
        cout << "\nError Bind";
        initfailed = true; 
	}

    if (chmod("/var/run/mysocket.sock", mode) == -1) 
    {
		cout << "\nError CHMOD socket file";
        initfailed = true; 
	}
    cout.flush();
    
    initfailed = false; 
    return;
}


void NamedSocketCom::process()
{
    if( initfailed == false )
    {
        while( 1 )
        {
            cout << "\nStatus wait for connection...\n";
            cout.flush();
           
            // Server Socket soll auf eingehende Verbindungen warten
            if (listen(socket_server, 1) == -1) 
            {
                cout << "\nError Listen\n";
                cout.flush();        
            }
            else
            {
                // Eingehende Verbindung akzeptieren und übergeben
                remote_addr_size = sizeof(struct sockaddr_un);
                socket_accept = accept(socket_server, (struct sockaddr *) &remote_addr, &remote_addr_size);
                
                if (socket_accept == -1) 
                {
                    cout << "\nError accept socket\n";
                    cout.flush();  
                }
                else
                {

                    int n;
                    
                    // Daten empfangen
                    n = recv(socket_accept, recbuffer, 100, 0);
                    if (n < 0) 
                    {
                        strOut = "Reception Error";
                    }
                    else
                    {
                        recbuffer[n] = 0;
                        procMsg->sendClientToServer(recbuffer);
                        
                        //sleep(1);
                                    
                        strOut = procMsg->receiveClientFromServer();
                        
                        if( strOut.length() == 0 )
                        {
                            strOut = "No data read";
                        }
                    }
                    
                    cout << "\nSend: " << strOut << "\n";
                    cout.flush();
                    
                    // Antwort senden
                    if (send(socket_accept, strOut.data(), strOut.length(), MSG_DONTWAIT) < 0) 
                    {
                        cout << "\nError data send\n";
                        cout.flush();
                    }
                }
                // Socket schließen
                close(socket_accept);
            }
        }
    }
    return;
}


