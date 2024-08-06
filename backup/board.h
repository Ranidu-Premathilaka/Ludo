#ifndef BOARD_H
#define BOARD_H

const int boardLength = 52;
const int boardPlayers = 4;
const int playerTroops = 4;

typedef struct {
    //int boardArray[boardLength][boardPlayers];
    //int boardArray[boardLength][boardPlayers];
    Troop *troopPtr[boardLength];
    int troopCount;
    int roundsElapsed;
}Board;

typedef struct {
    Player *owner;
    int position;
    char name[3];
    char notAtBase;
    char rotation;
    //int index?;
}Troop;

typedef struct {
    Troop troopArr[playerTroops];
    int troopsAtPlay;
    int troopsAtHome;
    int index;
    int startingLocation;
    int approachLocation;
    char *name;
}Player;

//typedef struct Board Board;
//typedef struct Player Player;
//typedef struct Troop Troop;

#endif 