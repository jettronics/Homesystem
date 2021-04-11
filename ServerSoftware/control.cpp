
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <time.h>
#include <sys/time.h>

#include "player.h"
#include "pushmessage.h"
#include "spimessage.h"
#include "interpreter.h"
#include "control.h"

int WaterAlarm::alarmStatus[] = {0, 0, 0};


Control::Control()
{
    devices.clear();
    pushMsg = new PushMessage;
    player = new Player;
    response = nullptr;
    cout << "\nControl constructor";
    cout.flush();
}

Control::Control( PushMessage *pMsg, Interpreter *pResp )
    : pushMsg(pMsg)
    , player(nullptr)
    , response(pResp)
{
    devices.clear();
}

Control::Control( Player *pPl, Interpreter *pResp )
    : pushMsg(nullptr)
    , player(pPl)
    , response(pResp)
{
    devices.clear();
}

Control::~Control()
{
    cout << "\nControl destructor";
    cout.flush();
}

void Control::init()
{
    // Player init
    player->init();
}

void Control::deInit()
{
    delete player;
    delete pushMsg;
}

void Control::event(int device, int data, Interpreter *response)
{
    bool found = false;
    
    // Check first if device with data already exists
    for( int i=0; i < devices.size(); i++ )
    {
        if( devices.at(i)->getDevice() == device )
        {
            found = true;
            cout << "\nEvent device active: " << device;
            if( devices.at(i)->getData() != data )
            {
                cout << "\nEvent device update: " << data;
                devices.at(i)->update(data);
            }
            cout.flush();
            break;
        }
    }
    
    if( found == false )
    {
        if( ((DeviceType)device == DetBathroomWater) ||
            ((DeviceType)device == DetKitchenWater) ||
            ((DeviceType)device == DetToiletWater) )
        {
            cout << "\nEvent Water device: " << device << " with data: " << data;
            Control *ev = new WaterAlarm((DeviceType)device, data, response, pushMsg);
            devices.push_back(ev);
        }  
        else
        if( ((DeviceType)device == DetBathroomControl) ||
            ((DeviceType)device == DetKitchenControl))
        {
            cout << "\nEvent Remote device: " << device << " with data: " << data;
            Control *ev = new Remote((DeviceType)device, data, response, player);
            devices.push_back(ev);
        }
        else
        {
            cout << "\nEvent Unknown device: " << device << " with data: " << data;
        }
        cout.flush();
    }
}

void Control::command(int id, int subId, Interpreter *response )
{
    cout << "\nCommand id: " << id << " with subId: " << subId;
    cout.flush();
    Control *cmd = new Client((IdType)id, subId, response, player);
    devices.push_back(cmd);
}

void Control::process()
{
    for( int i=0; i < devices.size(); )
    {
        devices.at(i)->task();
        
        if( devices.at(i)->finished() == true )
        {
            Control *remove = devices[i];
            delete remove;
            remove = nullptr;
            devices.erase(devices.begin()+i);
            
            cout << "\nControl task finished " << i;
            cout.flush();
        }
        else
        {
            i++;
        }
    }
    
    return;
}

void Control::responseStatus()
{
    Player play;
    PlayerStatus radioStatus = play.getStatus();
    vector<string> streamsVec = play.getStations();
    string streamsStr;
    string responseStr;
    
    for( int i = 0; i < streamsVec.size(); i++ )
    {
        streamsStr += (streamsVec[i] + ";");
    }
            
    responseStr = "Station: " + to_string(radioStatus.iStream) 
                + " | Bathroom: " + to_string(radioStatus.byRoom[PlayerStatus::Bathroom]) 
                + " | Kitchen: "  + to_string(radioStatus.byRoom[PlayerStatus::Kitchen])
                + " | ;"
                + streamsStr
                + to_string(DetBathroomWater) + ": " + to_string(WaterAlarm::alarmStatus[DetBathroomWater-1]) + ", "
                + to_string(DetKitchenWater) + ": " + to_string(WaterAlarm::alarmStatus[DetKitchenWater-1]) + ", "
                + to_string(DetToiletWater) + ": " + to_string(WaterAlarm::alarmStatus[DetToiletWater-1]);
    cout << "\nResponse: " << responseStr;
    cout.flush();  
    response->response(responseStr); 
    
    return;
}

#if 0
void Control::responseDetectorStatus()
{
    string responseStr;
    responseStr = to_string(DetBathroomWater) + ": " + to_string(WaterAlarm::alarmStatus[DetBathroomWater-1]) + ", "
                + to_string(DetKitchenWater) + ": " + to_string(WaterAlarm::alarmStatus[DetKitchenWater-1]) + ", "
                + to_string(DetToiletWater) + ": " + to_string(WaterAlarm::alarmStatus[DetToiletWater-1]);
    cout << "\nResponse: " << responseStr;
    cout.flush();  
    response->response(responseStr);
    
    return;
}
#endif

WaterAlarm::WaterAlarm( DeviceType device, int value, Interpreter *pResp, PushMessage *pMsg )
    : device(device)
    , data(value)
    , start(true)
    , stop(false)  
    , expirecnt(0)
    , Control(pMsg, pResp)
{
}

WaterAlarm::~WaterAlarm()
{
}

void WaterAlarm::task()
{
    if( start == true )
    {
        gettimeofday(&tvAlarmStart, 0);
        
        cout << "\nWaterAlarm task started: " << expirecnt << " expired: " << pushMsg->getExpire();
        cout << "\nWaterAlarm task data: " << data;
        cout.flush();
        alarmStatus[device-1] = 1;
        pushMsg->sendMessageWater( (PushMessage::DetType)device, data );
        start = false;
        stop = false;
        
        if( response != nullptr )
        {
            responseStatus();
        }
    }
    else
    {
        gettimeofday(&tvAlarmEnd, 0);
        if( tvAlarmEnd.tv_sec >= (tvAlarmStart.tv_sec+pushMsg->getRetry()) )
        {
            start = true;
            expirecnt += pushMsg->getRetry();
            if( (expirecnt >= pushMsg->getExpire()) ||
                (alarmStatus[device-1] == 0) )
            {
                stop = true;
                start = false;
                alarmStatus[device-1] = 0;
                
                if( response != nullptr )
                {
                    responseStatus();
                }
        
                cout << "\nWaterAlarm task finished: " << device;
                cout.flush();
            }
        }
    }
}

bool WaterAlarm::finished()
{
    return stop;
}

Remote::Remote( DeviceType device, int data, Interpreter *pResp, Player *pPl )
    : device(device)
    , data(data)
    , start(true)
    , stop(false)
    , Control(pPl, pResp)
{
}

Remote::~Remote()
{
}

void Remote::task()
{
    if( start == true )
    {
        gettimeofday(&tvRemoteStart, 0);
        cout << "\nRemote task started: " << device << " data: " << data;
        
        int param = 0;
        if( (RemoteType)data == RemctrlOff )
        {
            PlayInRoom play;
            if( device == DetBathroomControl )
            {
                param = (int)PlayerStatus::RoomType::Bathroom;
            }
            else
            if( device == DetKitchenControl )
            {
                param = (int)PlayerStatus::RoomType::Kitchen;
            }
            play.command(param);
            
            if( response != nullptr )
            {
                responseStatus();
            }
        }
        else 
        if( (RemoteType)data == RemctrlNext )
        {	
            PlayStationNext play;	
            play.command(param);
            
            if( response != nullptr )
            {
                responseStatus();
            }            
        }
        else
        if( (RemoteType)data == RemctrlVolUp )
        {	
            PlayVolumeRel play;	
            param = 5;
            play.command(param);
            
            if( response != nullptr )
            {
                responseStatus();
            }
        }
        else
        if( (RemoteType)data == RemctrlVolDn )
        {	
            PlayVolumeRel play;		
            param = -5;
            play.command(param);
            
            if( response != nullptr )
            {
                responseStatus();
            }
        }
        else
        {
            cout << "\nNot supported task: " << data;
        }
        cout.flush();

        start = false;
        stop = false;
    }
    else
    {
        gettimeofday(&tvRemoteEnd, 0);
        if( (tvRemoteEnd.tv_usec >= tvRemoteStart.tv_usec) && 
            (tvRemoteEnd.tv_sec >= tvRemoteStart.tv_sec+1) )
        {
            stop = true;
            cout << "\nRemote task finished: " << device;
            cout.flush();
        }
    }
}

bool Remote::finished()
{
    return stop;
}


Client::Client( IdType id, int subId, Interpreter *pResp, Player *pPl )
    : id(id)
    , subId(subId)
    , Control(pPl, pResp)
{
}

Client::~Client()
{
    cout << "\nClient destructor";
    cout.flush();
}

void Client::task()
{
    cout << "\nClient task started: " << id << " with subId: " << subId;
    
    if( (IdType)id == ClientDetGetStatus )
    {
        if( response != nullptr )
        {
            response->response("Not supported");
        }
    }
    else 
    if( (IdType)id == ClientRadioStation )
    {
        PlayStationNo play;
        play.command(subId);
    
        if( response != nullptr )
        {
            responseStatus();
        }
    }
    else 
    if( (IdType)id == ClientRadioStationNext )
    {
        PlayStationNext play;
        play.command(subId);
    
        if( response != nullptr )
        {
            responseStatus();
        }
    }
    else 
    if( (IdType)id == ClientRadioGetStatus )
    {
        if( response != nullptr )
        {
            responseStatus();
        }
    }
    else
    if( (IdType)id == ClientRadioOff )
    {
        PlayStationsOff play;
        play.command(subId);
        
        if( response != nullptr )
        {
            responseStatus();
        }
    }
    else
    if( (IdType)id == ClientRoom )
    {
        PlayInRoom play;
        play.command(subId);
    
        if( response != nullptr )
        {
            responseStatus();
        }
    }
    else 
    if( (IdType)id == ClientRoomOn )
    {
        PlayInRoomOn play;
        play.command(subId);
    
        if( response != nullptr )
        {
            responseStatus();
        }
    }
    else 
    if( (IdType)id == ClientRoomOff )
    {
        PlayInRoomOff play;
        play.command(subId);
    
        if( response != nullptr )
        {
            responseStatus();
        }
    }
    else 
    if( (IdType)id == ClientVolumeAbs )
    {
        PlayVolumeAbs play;
        play.command(subId);
        
        if( response != nullptr )
        {
            responseStatus();
        }
    }
    else 
    if( (IdType)id == ClientVolumeRel )
    {
        PlayVolumeRel play;	

        if( subId > 0 )
        {	
            play.command(5);
        }
        else
        {	
            play.command(-5);
        }
        
        if( response != nullptr )
        {
            responseStatus();
        }
    }
    else 
    if( (IdType)id == ClientAlarmGetStatus )
    {
        if( response != nullptr )
        {
            responseStatus();
        }
    }
    else 
    if( (IdType)id == ClientResetAlarm )
    {
        for( int i = 0; i < DetAlarmMax; i++ )
        {
            WaterAlarm::alarmStatus[i] = 0;
        }
        if( response != nullptr )
        {
            responseStatus();
        }
    }
    else
    {
        cout << "\nNot supported task: " << id;
        if( response != nullptr )
        {
            response->response("Not supported");
        }
    }
    cout.flush();
}

