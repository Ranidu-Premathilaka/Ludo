#ifndef STRUCT_H
#define STRUCT_H

#define boardLength 52
#define boardPlayers 4
#define playerTroops 4
#define homeRowLength 6

typedef struct Board Board;
typedef struct Player Player;
typedef struct Troop Troop;
typedef struct Block Block;

struct Board{
    Troop *troop;
    Block *block;
    int troopCount;
};

struct Block{
    Troop *troopArr[playerTroops];
    char rotation;
};


struct Troop{
    Player *owner;
    int position;
    int captures;
    char name[3];
    char notAtBase;
    char rotation;
    //int index;
};

struct Player{
    Troop troopArr[playerTroops];
    Board homeRow[homeRowLength];
    int troopsAtPlay;
    int troopsAtHome;
    int index;
    int startingLocation;
    int approachLocation;
    char *name;
    char isWinner;
};


#endif 