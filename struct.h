#ifndef STRUCT_H
#define STRUCT_H
#include "const.h"

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
    int mysteryEffect[mysteryLog];
    char name[maxBlockName];
    char rotation;
};
//second element is roundNum; 
//mysteryEffect 0 - no effect
//mysteryEffect 1 - energized
//mysteryEffect 2 - sick
//mysteryEffect 3 - can't move

struct Troop{
    Player *owner;
    int position;
    int captures;
    int approachPassed;
    int index; //only used in break block can be redundent
    int mysteryEffect[mysteryLog];
    char name[3];
    char where;// 1 = not at base/in play, 0 = at base, 2- in homerow,3 - in home
    char rotation;
};

struct Player{
    Troop troopArr[playerTroops];
    Board homeRow[homeRowLength];
    int troopsAtPlay;
    int troopsAtHome;
    int troopsAtBase;
    int index;
    int startingLocation;
    int approachLocation;
    char *name;
    char isWinner;
};


#endif 