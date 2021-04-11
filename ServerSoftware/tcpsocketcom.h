#ifndef TCPSOCKETCOM_H
#define TCPSOCKETCOM_H

using namespace std;

class TcpSocketCom
{
public:
    enum DirType { SendToClient = 0, SendToServer, SendAndRecieve };

    TcpSocketCom(int port, DirType dir);
    TcpSocketCom(ProcMessage *procMsg, int port, DirType dir);
    virtual ~TcpSocketCom(); 

public:
    int init();
    void deInit();
    void process();
    
private:    
    string getData();
    void sendData(string send);

private:
    int sockfd; 
    int newsockfd; 
    int portno; 
    int clilen;
    struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;
    bool initfailed;
    ProcMessage *procMsg;
    DirType dir;
};


#endif 
