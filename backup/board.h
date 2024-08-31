#ifndef BOARD_H
#define BOARD_H

const int boardLength = 52;
const int boardPlayers = 4;
const int playerPieces = 4;

typedef struct {
    //int boardArray[boardLength][boardPlayers];
    //int boardArray[boardLength][boardPlayers];
    Piece *piecePtr[boardLength];
    int pieceCount;
    int roundsElapsed;
}Board;

typedef struct {
    Player *owner;
    int position;
    char name[3];
    char notAtBase;
    char rotation;
    //int index?;
}Piece;

typedef struct {
    Piece pieceArr[playerPieces];
    int piecesAtPlay;
    int piecesAtHome;
    int index;
    int startingLocation;
    int approachLocation;
    char *name;
}Player;

//typedef struct Board Board;
//typedef struct Player Player;
//typedef struct Piece Piece;

#endif 