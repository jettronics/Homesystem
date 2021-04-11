
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <time.h>
#include <sys/time.h>
#include <linux/spi/spidev.h>	//Needed for SPI port

#include "pushmessage.h"
#include "player.h"
#include "control.h"
#include "procmessage.h"
#include "spimessage.h"
#include "interpreter.h"


vector<Interpreter*> Interpreter::interVect;
ProcMessage* Interpreter::consolemsg = nullptr;

void Interpreter::init(vector<ProcMessage*> *procmsg, Control *controller)
{
    Interpreter *interp;
    interp = new RemoteCtrl(procmsg->at(1), controller);
    interVect.push_back(interp);
    interp = new WebServer(procmsg->at(0), controller);
    interVect.push_back(interp);
    interp = new WebCtrl(procmsg->at(1), controller);
    interVect.push_back(interp);
    
    consolemsg = procmsg->at(2);
    
    return;
}

void Interpreter::deinit()
{
    for( int i=0; i < interVect.size(); i++ )
    {
        Interpreter *remove = interVect.at(i);
        delete remove;
    }
    interVect.clear();
    
    return;
}

void Interpreter::call()
{
    for( int i=0; i < interVect.size(); i++ )
    {
        interVect.at(i)->interprete();
    }
    
    return;
}


Interpreter::Interpreter(ProcMessage *procmsg, Control *controller)
    : controller(controller)
    , procmsg(procmsg)
{
    
}

Interpreter::~Interpreter()
{

}

void Interpreter::response(string content)
{ 
    procmsg->sendServerToClient(content);
    
    return; 
}

RemoteCtrl::RemoteCtrl(ProcMessage *procmsg, Control *controller)
    : previousrxdata(0)
    , Interpreter(procmsg, controller)
{
    spimsg = new SpiMessage(0, SPI_MODE_0, 8, 1000000);
    spimsg->start();
}

RemoteCtrl::~RemoteCtrl()
{
    spimsg->stop();
    delete spimsg;
}


void RemoteCtrl::interprete()
{
    unsigned char txdata = 1;
    unsigned char rxdata = 0;
    unsigned char byDevice = 0;
	unsigned char byData = 0;
    
    spimsg->read(&txdata, &rxdata, 1, 0);
    /*
    string con = consolemsg->receiveServerFromClient();
    if( con == "1" )
    {
        rxdata = 0x30;
    }
    else
    if( con == "2" )
    {
        rxdata = 0x2D;
    }
    */
	if( rxdata > 0 )
    {
        if( rxdata != previousrxdata )
        {
            cout << "\nSpi read: " << to_string(rxdata) << endl;
            cout.flush();
            byDevice = (rxdata >> 2) & 0x0F;
            byData = rxdata & 0x03;
            controller->event((int)byDevice, (int)byData, this);
        }
    }
    previousrxdata = rxdata;
    
    return;
    
}


WebServer::WebServer(ProcMessage *procmsg, Control *controller)
    : Interpreter(procmsg, controller)
{
}

WebServer::~WebServer()
{
}


void WebServer::interprete()
{
    /*int key = 0;
    string rec = "";
    cin >> key;
    rec = "getalarmstatus";*/
    string rec = procmsg->receiveServerFromClient();
    if( rec.length() > 1 )
    {
        size_t pos;
        
        if( rec.find("getstatus") != string::npos )
        {
            // Detector status
            controller->command(Control::ClientDetGetStatus, 0, this);
        }
        else
        if( rec.find("radiostatus") != string::npos )
        {
            controller->command(Control::ClientRadioGetStatus, 0, this);
        }
        else
        if( (pos = rec.find("station=")) != string::npos )
        {
            string station = rec.substr(pos+8);
            int stationint = stoi(station)-1;
            controller->command(Control::ClientRadioStation, stationint, this);
        }
        else
        if( rec.find("switchoff") != string::npos )
        {
            controller->command(Control::ClientRadioOff, 0, this);
        }
        else
        if( rec.find("bathroomspeaker") != string::npos )
        {
            controller->command(Control::ClientRoom, PlayerStatus::RoomType::Bathroom, this);
        }
        else
        if( rec.find("kitchenspeaker") != string::npos )
        {
            controller->command(Control::ClientRoom, PlayerStatus::RoomType::Kitchen, this);
        }
        else
        if( rec.find("volumeplus") != string::npos )
        {
            controller->command(Control::ClientVolumeRel, 1, this);
        }
        else
        if( rec.find("volumeminus") != string::npos )
        {
            controller->command(Control::ClientVolumeRel, -1, this);
        }
        else
        if( rec.find("getalarmstatus") != string::npos )
        {
            controller->command(Control::ClientAlarmGetStatus, 0, this);
        }
        else
        if( rec.find("ClientResetAlarm") != string::npos )
        {
            controller->command(Control::ClientResetAlarm, 0, this);
        }
        else
        {
            response("Not supported");
        }
    }
    
    return;
}


WebCtrl::WebCtrl(ProcMessage *procmsg, Control *controller)
    : Interpreter(procmsg, controller)
{
}

WebCtrl::~WebCtrl()
{
}

//Kitchen=true; Volume=50
//Bathroom=true; Volume=30
//Next=true
void WebCtrl::interprete()
{
    /*int key = 0;
    string rec = "";
    cin >> key;
    rec = "Default=true";*/
    string rec = procmsg->receiveServerFromClient();
    if( rec.length() > 1 )
    {
        size_t pos;
        
        if( rec.find("Kitchen") != string::npos )
        {
            if( rec.find("true") != string::npos )
            {
                // On
                size_t pos = rec.find("Volume=");
                string volumestr = rec.substr(pos+7);
                int volumeint = stoi(volumestr);
                controller->command(Control::ClientRoomOn, PlayerStatus::RoomType::Kitchen, this);
                controller->command(Control::ClientVolumeAbs, volumeint, this);
            }
            else
            if( rec.find("false") != string::npos )
            {
                // Off
                controller->command(Control::ClientRoomOff, PlayerStatus::RoomType::Kitchen, this);
            }
        }
        else
        if( rec.find("Bathroom") != string::npos )
        {
            if( rec.find("true") != string::npos )
            {
                // On
                size_t pos = rec.find("Volume=");
                string volumestr = rec.substr(pos+7);
                int volumeint = stoi(volumestr);
                controller->command(Control::ClientRoomOn, PlayerStatus::RoomType::Bathroom, this);
                controller->command(Control::ClientVolumeAbs, volumeint, this);
            }
            else
            if( rec.find("false") != string::npos )
            {
                // Off
                controller->command(Control::ClientRoomOff, PlayerStatus::RoomType::Bathroom, this);
            }            
        }
        else
        if( rec.find("Next") != string::npos )
        {
            controller->command(Control::ClientRadioStationNext, 0, this);
        }
        else
        if( rec.find("Default") != string::npos )
        {
            if( rec.find("true") != string::npos )
            {
                controller->command(Control::ClientRadioStation, 1, this);
                controller->command(Control::ClientRoomOn, PlayerStatus::RoomType::Kitchen, this);
            }
            else
            if( rec.find("false") != string::npos )
            {
                controller->command(Control::ClientRoomOff, PlayerStatus::RoomType::Kitchen, this);
            }
        }
        else
        if( (pos = rec.find("station=")) != string::npos )
        {
            string station = rec.substr(pos+8);
            int stationint = stoi(station)-1;
            controller->command(Control::ClientRadioStation, stationint, this);
        }
        else
        if( rec.find("switchoff") != string::npos )
        {
            controller->command(Control::ClientRadioOff, 0, this);
        }
        else
        if( rec.find("bathroomspeaker") != string::npos )
        {
            controller->command(Control::ClientRoom, PlayerStatus::RoomType::Bathroom, this);
        }
        else
        if( rec.find("kitchenspeaker") != string::npos )
        {
            controller->command(Control::ClientRoom, PlayerStatus::RoomType::Kitchen, this);
        }
        else
        if( rec.find("volumeplus") != string::npos )
        {
            controller->command(Control::ClientVolumeRel, 1, this);
        }
        else
        if( rec.find("volumeminus") != string::npos )
        {
            controller->command(Control::ClientVolumeRel, -1, this);
        }
        else
        if( rec.find("ClientResetAlarm") != string::npos )
        {
            controller->command(Control::ClientResetAlarm, 0, this);
        }
        else
        {
            if( rec.find("radiostatus") != string::npos )
            {
                controller->command(Control::ClientRadioGetStatus, 0, this);
            }
            
            if( rec.find("getalarmstatus") != string::npos )
            {
                controller->command(Control::ClientAlarmGetStatus, 0, this);
            }
        }
    }
}


