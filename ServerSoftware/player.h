#ifndef PLAYER_H
#define PLAYER_H

using namespace std;

class PlayerStatus
{
public:
    enum RoomType { Kitchen = 0, Bathroom, Size };
    
    PlayerStatus();

public:
    string strStation;
    unsigned char byLoadPlayList;
    unsigned char byRoom[Size];
    unsigned char byPlay;
    int iStream;
};


class Player
{
public:
    Player();
    virtual ~Player();

public:
    void init();
    vector<string> getStations() { return stations; }
    PlayerStatus getStatus() { return status; }
    virtual void command(int param) { return; }
    
protected:
    void readStations();
    void loadPlaylist();
    void playStream( int iNumber );
    void stopPlaying();
    void playControl();
    void playStationName( int iNumber );

protected: //accessible from derived class
    static PlayerStatus status;
    static vector<string> stations;
};

  
class PlayStationNo : public Player
{
public:
    void command(int param);
};

class PlayStationNext : public Player
{
public:
    void command(int param);
};

class PlayStationsOff : public Player
{
public:
    void command(int param);
};

class PlayInRoom : public Player
{
public:
    void command(int param);
};

class PlayInRoomOn : public Player
{
public:
    void command(int param);
};

class PlayInRoomOff : public Player
{
public:
    void command(int param);
};

class PlayVolumeRel : public Player
{
public:
    void command(int param);
};

class PlayVolumeAbs : public Player
{
public:
    void command(int param);
};




#endif
