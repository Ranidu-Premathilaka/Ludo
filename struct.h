#ifndef STRUCT_H
#define STRUCT_H
#include "const.h"

typedef struct Board Board;
typedef struct Player Player;
typedef struct Piece Piece;
typedef struct Block Block;


struct Board{
    Piece *piece;
    Block *block;
    int pieceCount;
};

struct Block{
    Piece *pieceArr[playerPieces];
    int mysteryEffect[mysteryLog];
    char name[maxBlockName];
    char rotation;
};
//second element is roundNum; 
//mysteryEffect 0 - no effect
//mysteryEffect 1 - energized
//mysteryEffect 2 - sick
//mysteryEffect 3 - can't move

struct Piece{
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
    Piece pieceArr[playerPieces];
    Board homeRow[homeRowLength];
    int piecesAtPlay;
    int piecesAtHome;
    int piecesAtBase;
    int index;
    int startingLocation;
    int approachLocation;
    char *name;
    char isWinner;
};


#endif 