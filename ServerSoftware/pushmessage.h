#ifndef PUSHMESSAGE_H
#define PUSHMESSAGE_H

using namespace std;

class PushMessage
{
public:
    enum DetType { DetBathroomWater = 1, DetKitchenWater, DetToiletWater };

    PushMessage();
    virtual ~PushMessage();

public:
    string getMessage() { return strMessage; }
    void sendMessageWater( DetType source, int data );
    int getRetry() { return retry; }
    int getExpire() { return expire; }
    
private:
    vector<string> strHead;
    vector<string> strBattery;
    string strUrl;
    string strPrivateKey;
    string strMessage;
    int retry;
    int expire;
};




#endif
