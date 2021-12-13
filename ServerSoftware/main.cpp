
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
#include <vector>

#include "pushmessage.h"
#include "procmessage.h"
#include "tcpsocketcom.h"
#include "namedsocketcom.h"
#include "player.h"
#include "control.h"
#include "spimessage.h"
#include "interpreter.h"

using namespace std;


// "./%e"

int main(int argc, char *argv[]) 
{
    pid_t pid[2];
    int stat;
    ProcMessage msgDetNodeRed; // Server = Det, Client = Node-red
    ProcMessage msgDetApache; // Server = Det, Client = Apache
    ProcMessage msgDetConsole;
    
    msgDetNodeRed.init(ProcMessage::UnblockingRead, ProcMessage::UnblockingRead);
    msgDetApache.init(ProcMessage::UnblockingRead, ProcMessage::BlockingAll);
    msgDetConsole.init(ProcMessage::UnblockingRead, ProcMessage::BlockingAll);
    
    pid[0] = fork(); 
    pid[1] = fork(); 
  
    if (pid[0] > 0 && pid[1] > 0) 
    { 
        cout << "\nDetector Server\n"; 
        cout.flush();
        
        Control controller;
        controller.init();
        
        vector<ProcMessage*> procmsgs;
        procmsgs.push_back(&msgDetApache);
        procmsgs.push_back(&msgDetNodeRed);
        procmsgs.push_back(&msgDetConsole);
        Interpreter::init(&procmsgs, &controller);
        
        while( 1 )
        {
            //string recNodered = msgDetNodeRed.receiveServerFromClient();
            //cout << "\nNode-red receive: " << recNodered;
            //cout.flush();
            //string recApache = msgDetApache.receiveServerFromClient();
            //cout << "\nApache receive: " << recApache;
            //cout.flush();
            Interpreter::call();
            controller.process();
            
            usleep(50000);
        }
        
        waitpid(pid[1], &stat, 0);
        cout << "\nTCP Socket Com Client closed\n"; 
        cout.flush();
    
        waitpid(pid[0], &stat, 0);
        cout << "\nNamed Socket Client closed\n"; 
        cout.flush();
        
        Interpreter::deinit();
        controller.deInit();

    } 
    else if (pid[0] == 0 && pid[1] > 0) 
    { 
        cout << "\nNamed Socket Com Client\n"; 
        cout.flush();
        
        NamedSocketCom namedsocketcom(&msgDetApache);
        
        namedsocketcom.init();
        namedsocketcom.process();

        waitpid(pid[1], &stat, 0);
        cout << "\nConsole Client closed\n"; 
        cout.flush();

        exit(0);
    } 
    else if (pid[0] > 0 && pid[1] == 0) 
    { 
        cout << "\nTCP Socket Com Client\n"; 
        cout.flush();
        
        TcpSocketCom tcpsocketcomToClient(&msgDetNodeRed, 51717, TcpSocketCom::DirType::SendToServer);
        TcpSocketCom tcpsocketcomToServer(&msgDetNodeRed, 52717, TcpSocketCom::DirType::SendToClient);
        
        while( tcpsocketcomToClient.init() < 0 )
        {
            sleep(1);
        }
        while( tcpsocketcomToServer.init() < 0 )
        {
            sleep(1);
        }
        sleep(1);
        msgDetNodeRed.sendClientToServer("radiostatus");
        msgDetNodeRed.sendClientToServer("getalarmstatus");
        
        while( 1 )
        {
            tcpsocketcomToClient.process();
            tcpsocketcomToServer.process();
            usleep(50000);
        }
        
        tcpsocketcomToClient.deInit();
        tcpsocketcomToServer.deInit();

        exit(0);
    } 
    else 
    { 
        cout << "\nConsole Client\n"; 
        cout.flush();
        
        cin.clear();
        fflush(stdin);
        
        while( 1 )
        {
            string key = "";
            cin >> key;
            cout << "\nKeyboard entered: " << key << "\n";
            cout.flush();
            
            //msgDetConsole.sendClientToServer(key);
        }
        
        exit(0);
    } 

    msgDetNodeRed.deInit();
    msgDetApache.deInit();
    msgDetConsole.deInit();
     
    return 0; 
}
