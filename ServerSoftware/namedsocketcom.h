#ifndef NAMEDSOCKETCOM_H
#define NAMEDSOCKETCOM_H


using namespace std;

class NamedSocketCom
{
public:
    NamedSocketCom();
    NamedSocketCom(ProcMessage *procMsg);
    virtual ~NamedSocketCom(); 

public:
    void init();
    void process();
    
private:
    ProcMessage *procMsg;
    int socket_server;
    struct sockaddr_un server_addr;
    struct sockaddr_un remote_addr;
    socklen_t remote_addr_size;
    int socket_accept; 
    string strOut;
    char recbuffer[1024];
    bool initfailed;
};


#endif 
