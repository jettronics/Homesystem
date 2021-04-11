#ifndef CONTROL_H
#define CONTROL_H

using namespace std;

class Interpreter;

class Control
{
public:
    enum DeviceType { DetBathroomWater = 1, DetKitchenWater, DetToiletWater, DetAlarmMax = 3,
                      DetBathroomControl = 11, DetKitchenControl };
    enum IdType { ClientDetGetStatus = 0, 
                  ClientRadioGetStatus, ClientRadioStation, ClientRadioStationNext, ClientRadioOff, ClientRoom, ClientRoomOn, ClientRoomOff, ClientVolumeAbs, ClientVolumeRel,
                  ClientAlarmGetStatus, ClientResetAlarm,
                  ClientMax };
    enum RemoteType { RemctrlOff = 0, RemctrlNext, RemctrlVolUp, RemctrlVolDn };
  
    Control();
    Control( PushMessage *pMsg, Interpreter *pResp );
    Control( Player *pPl, Interpreter *pResp );
    virtual ~Control();

public:
    void init();
    void deInit();
    void event(int device, int data, Interpreter *response);
    void command(int id, int subId, Interpreter *response);
    void process();
    virtual void task() { return; }
    virtual bool finished() { return false; }
    virtual int getDevice() { return 0; }
    virtual int getData() { return 0; }
    virtual void update(int data) { return; }
    
protected:
    void responseStatus();
    //void responseDetectorStatus();
    
private:
    vector<Control*> devices;
    
protected:
    PushMessage *pushMsg;
    Player *player;
    Interpreter *response;
};

  
class WaterAlarm : public Control
{
public:
    WaterAlarm( DeviceType device, int value, Interpreter *pResp, PushMessage *pMsg );
    virtual ~WaterAlarm();
    void task();
    bool finished();
    int getDevice() { return device; }
    int getData() { return data; }
    void update(int value) { data = value; }
    
public:
    static int alarmStatus[DetAlarmMax];
    
protected:
    DeviceType device;
    int data;
    bool start;
    bool stop;
    struct timeval tvAlarmStart; 
    struct timeval tvAlarmEnd;    
    int expirecnt;
};

class Remote : public Control
{
public:
    Remote( DeviceType device, int data, Interpreter *pResp, Player *pPl );
    virtual ~Remote();
    void task();
    bool finished();
    int getDevice() { return device; }
    int getData() { return data; }
    void update(int data) { this->data = data; }

protected:
    DeviceType device;
    int data;
    bool start;
    bool stop;
    struct timeval tvRemoteStart; 
    struct timeval tvRemoteEnd;
};

class Client : public Control
{
public:
    Client( IdType id, int subId, Interpreter *pResp, Player *pPl );
    virtual ~Client();
    void task();
    bool finished() { return true; }
    
protected:
    IdType id;
    int subId;
      
};


#endif
