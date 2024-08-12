#ifndef STRUCT_H
#define STRUCT_H

#define boardLength 52
#define boardPlayers 4
#define playerTroops 4
#define homeRowLength 6
#define approachLength 6
#define maxBlocks 2
#define maxRollVal 6
#define totalOptions 8
#define maxBlockName 11

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
    char name[maxBlockName];
    Troop *troopArr[playerTroops];
    char rotation;
};


struct Troop{
    Player *owner;
    int position;
    int captures;
    char name[3];
    char where;
    // 0 = not at base, 1 = at base, 2- in homerow
    char rotation;
    int index;
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