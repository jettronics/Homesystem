
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "player.h"

PlayerStatus Player::status;
vector<string> Player::stations;

PlayerStatus::PlayerStatus()
    : strStation("----")
    , byLoadPlayList(1)
    , byPlay(0)
    , iStream(0)
{
    for( int i=0; i<Size; i++ )
    {
        byRoom[i] = 0;
    }
}

Player::Player()
{
    
}

Player::~Player()
{

}


void Player::init()
{
    cout << "\nConfigure and setting Player GPIOs";
    cout.flush();
    /*
    pinMode( GPIO_PIN_23, OUTPUT );
    pinMode( GPIO_PIN_24, OUTPUT );
    digitalWrite( GPIO_PIN_23, LOW );
    digitalWrite( GPIO_PIN_24, LOW );
    */
    
    // Export the desired pin by writing to /sys/class/gpio/export
    int fd;
    fd = open("/sys/class/gpio/export", O_WRONLY);
    write(fd, "24", 2);
    close(fd);
    fd = open("/sys/class/gpio/export", O_WRONLY);
    write(fd, "23", 2);
    close(fd);

    // Set the pin to be an output by writing "out" to /sys/class/gpio/gpioxx/direction
    fd = open("/sys/class/gpio/gpio24/direction", O_WRONLY);
    write(fd, "out", 3);
    close(fd);
    fd = open("/sys/class/gpio/gpio23/direction", O_WRONLY);
    write(fd, "out", 3);
    close(fd);

    // Set the pin to a level
    fd = open("/sys/class/gpio/gpio24/value", O_WRONLY);
    write(fd, "0", 1);
    close(fd);
    fd = open("/sys/class/gpio/gpio23/value", O_WRONLY);
    write(fd, "0", 1);
    close(fd);

    readStations();
}


void Player::readStations()
{
	string line;
    cout  << "\nRead Stations" << line;
    
    stations.clear();
    stations.push_back("----");
	
	ifstream filestream("/home/pi/Programme/Homesystem/stations.txt", ifstream::in);
	if (filestream.is_open())
	{
		while (getline(filestream, line))
		{
            cout  << "\nRead line: " << line;
            stations.push_back(line);
		}
        // remove last line
        cout  << "\nRemove last line: " << line;
        stations.pop_back();
        filestream.close();
	}
    cout  << "\nStations: " << stations.size() << line << endl;
    cout.flush();
    return;
}
    
void Player::loadPlaylist( void )
{
    string strPlayerMessage;
    
    readStations();
    
    strPlayerMessage = "mpc clear";
    system( strPlayerMessage.data() );
    strPlayerMessage = "mpc load internetradio";
    system( strPlayerMessage.data() );
}

void Player::playStream( int iNumber )
{
    string strPlayerMessage;
    
    cout << "\nStream: " << iNumber << endl;
    
    status.iStream = iNumber;
    status.strStation = stations[iNumber];
    strPlayerMessage = "mpc play " + to_string(iNumber);
    system( strPlayerMessage.data() );
}

void Player::stopPlaying( void )
{
    string strPlayerMessage;
    
    status.strStation = "----";
    status.iStream = 0;
    strPlayerMessage = "mpc stop";
    system( strPlayerMessage.data() );
}

void Player::playControl()
{
    if( status.byRoom[PlayerStatus::RoomType::Bathroom] == 0 )
    {
        cout << "\nBathroom GPIO off" << endl;
        
        //digitalWrite( GPIO_PIN_23, LOW );
        int fd = open("/sys/class/gpio/gpio23/value", O_WRONLY);
        write(fd, "0", 1);
        close(fd);
    }
    else
    {
        cout << "\nBathroom GPIO on" << endl;
        
        //digitalWrite( GPIO_PIN_23, HIGH );
        int fd = open("/sys/class/gpio/gpio23/value", O_WRONLY);
        write(fd, "1", 1);
        close(fd);
    }
    
    if( status.byRoom[PlayerStatus::RoomType::Kitchen] == 0 )
    {
        cout << "\nKitchen GPIO off" << endl;
        
        //digitalWrite( GPIO_PIN_24, LOW );
        int fd = open("/sys/class/gpio/gpio24/value", O_WRONLY);
        write(fd, "0", 1);
        close(fd);
    }
    else
    {
        cout << "\nKitchen GPIO on" << endl;
        
        //digitalWrite( GPIO_PIN_24, HIGH );
        int fd = open("/sys/class/gpio/gpio24/value", O_WRONLY);
        write(fd, "1", 1);
        close(fd);
    }
    cout.flush();
    
    if( (status.byRoom[PlayerStatus::RoomType::Bathroom] == 0) && 
        (status.byRoom[PlayerStatus::RoomType::Kitchen] == 0) )
    {
        status.byPlay = 0;
        status.byLoadPlayList = 1;
        stopPlaying();
    }
}

void PlayStationNo::command(int param)
{
    cout << "\nPlayStationNo: " << param << endl;
    
    if( status.byLoadPlayList == 1 )
    {
        loadPlaylist();
        status.byLoadPlayList = 0;
    }
    playStream(param);
    status.byPlay = 1;
    
    cout.flush();
    
    return;
}

void PlayStationNext::command(int param)
{
    cout << "\nPlayStationNext: " << param << endl;
    
    if( status.byLoadPlayList == 1 )
    {
        loadPlaylist();
        status.byLoadPlayList = 0;
    }
    status.iStream++;
    if( status.iStream <= 0 )
    {
        status.iStream = 1;
    }
    else
    if( status.iStream >= stations.size() )
    {
        status.iStream = 1;
    }          
    playStream(status.iStream);
    status.byPlay = 1;
    
    cout.flush();    
    
    return;
}

void PlayStationsOff::command(int param)
{
    cout << "\nPlayStationsOff: " << param << endl;
    
    status.byRoom[PlayerStatus::RoomType::Bathroom] = 0;
    status.byRoom[PlayerStatus::RoomType::Kitchen] = 0;
    playControl();
    
    cout.flush();     
    
    return;
}

void PlayInRoom::command(int param)
{
    cout << "\nPlayInRoom: " << param << endl;
    
    if( status.byRoom[param] == 0 )
    {
        status.byRoom[param] = 1;
        if( status.byPlay == 0 )
        {
            if( status.byLoadPlayList == 1 )
            {
                loadPlaylist();
                status.byLoadPlayList = 0;
            }
            if( status.iStream == 0 )
            {
                status.iStream = 1;
            }
            playStream(status.iStream);
            status.byPlay = 1;
        }
    }
    else
    {
        status.byRoom[param] = 0;
    }   
    
    playControl();

    cout.flush(); 
    
    return;
}

void PlayInRoomOn::command(int param)
{
    cout << "\nPlayInRoomOn: " << param << endl;
    
    if( status.byRoom[param] == 0 )
    {
        status.byRoom[param] = 1;
        if( status.byPlay == 0 )
        {
            if( status.byLoadPlayList == 1 )
            {
                loadPlaylist();
                status.byLoadPlayList = 0;
            }
            if( status.iStream == 0 )
            {
                status.iStream = 1;
            }
            playStream(status.iStream);
            status.byPlay = 1;
        }
        playControl();
    }
    
    cout.flush(); 
    
    return;
}

void PlayInRoomOff::command(int param)
{
    cout << "\nPlayInRoomOff: " << param << endl;
    
    if( status.byRoom[param] == 1 )
    {
        status.byRoom[param] = 0;
        playControl();
    }   
    
    cout.flush(); 
        
    return;
}


void PlayVolumeRel::command(int param)
{
    string strPlayerMessage;
    
    cout << "\nPlayVolumeRel: " << param << endl;
    
    if( param >= 0 )
    {
        strPlayerMessage = "mpc volume +" + to_string(param); 
    }
    else
    {
        strPlayerMessage = "mpc volume " + to_string(param);
    }
       
    system( strPlayerMessage.data() );
    
    cout.flush(); 
    
    return;
}

void PlayVolumeAbs::command(int param)
{
    string strPlayerMessage;
    
    if( param < 100 )
    {
        cout << "\nPlayVolumeAbs: " << param << endl;
        
        strPlayerMessage = "mpc volume " + to_string(param); 
           
        system( strPlayerMessage.data() );
        
        cout.flush(); 
    }
    return;    
}
