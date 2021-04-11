
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <vector>

#include "pushmessage.h"


PushMessage::PushMessage()
    : strUrl("https://www.pushsafer.com")
    , strPrivateKey("onogE2Ja319OjwASXCYJ")
    , strMessage("No push message sent yet")
    , retry(60)
    , expire(300)
{
    strHead.clear();
    strHead.push_back("---------------");
    strHead.push_back("Water Alarm Bathroom");
    strHead.push_back("Water Alarm Kitchen");
    strHead.push_back("Water Alarm Guest Toilet");
    
    strBattery.clear();
    strBattery.push_back("Normal");
    strBattery.push_back("Low");
    strBattery.push_back("Very Low");
    strBattery.push_back("Critical");

}

PushMessage::~PushMessage()
{
    
}

void PushMessage::sendMessageWater( DetType source, int data )
{
    string strTitle;
    string strInfo;
    string strSymbol;
    string strData;
    
    strTitle = strHead[(int)source];
    strInfo = "Detector Battery Status: " + strBattery[data];
    strSymbol = "75";

#if 0	
    sprintf( strMessage, 
	"curl -s --form-string \"t=%s\" --form-string \"m=%s\" --form-string \"s=25\" --form-string \"v=1\" --form-string \"i=%s\" --form-string \"d=%c\" --form-string \"u=%s\" --form-string \"k=%s\" https://www.pushsafer.com/api", strTitle, strInfo, strSymbol, cDevice, strUrl, strPrivateKey);
#endif
    
    strMessage = "curl -s --form-string \"t=" + strTitle + "\" "
                 + "--form-string \"m=" + strInfo + "\" "
                 + "--form-string \"s=25\" " // sound
                 + "--form-string \"v=1\" " // vibration
                 + "--form-string \"i=" + strSymbol + "\" " 
                 + "--form-string \"d=a\" " // device
                 //+ "--form-string \"re=" + to_string(retry) + "\" "  // retry in s
                 //+ "--form-string \"ex=" + to_string(expire) + "\" " // expire in s
                 + "--form-string \"u=" + strUrl + "\" "
                 + "--form-string \"k=" + strPrivateKey + "\" "
                 + "https://www.pushsafer.com/api";
                 
    cout << "\n" << strMessage << "\n";
    cout.flush();
    
    system( strMessage.data() );  
    
    return;
}
