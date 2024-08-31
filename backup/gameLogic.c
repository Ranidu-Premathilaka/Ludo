#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "struct.h"

#define counterClockwise 1
#define clockwise 0

//to stop repeatedly typing struct 
//Posibility of Null pointer dereferencing

Board board;
Player arr[4];


//functiondef
void PlayerInit(Player*,char*);
void boardInit();
void playerCount(int , int *,int *);
int roll();
int coinFlip();
int pieceToBoard(int ,Player *);
int pieceToBase(Piece *);
int spin(Piece *);
Piece *nextPiece(Player *);
int movement(Piece *,int);
int posCalc(int,int,int);

//Player
void PlayerInit(Player *player,char *playerName){
    static int count = 0;

    player->name= playerName;
    player->piecesAtPlay=0;
    player->piecesAtHome=0;
    player->index = count;
    player->startingLocation = 13*count;
    player->approachLocation = posCalc(13*count,2,1);
    count++;
    
    for (short i = 0; i < playerPieces; i++){
        player->pieceArr[i].name[0] = 'R';
        player->pieceArr[i].name[1] = '0'+i;
        player->pieceArr[i].name[2] = "\0";
        player->pieceArr[i].position = 0;
        player->pieceArr[i].notAtBase = 0;
        player->pieceArr[i].owner = player;
        player->pieceArr[i].rotation = clockwise;
    }
    
}

//board

void boardInit(){
    for (short i = 0; i <boardLength ; i++){
        for(short j = 0; j <boardPlayers; j++){
            board.boardArray[i][j]=0;
        } 
    }

    board.roundsElapsed=0;
}

void playerCount(int cellNum, int *count,int *color){
    *count = 0;
    *color = 0;
    for (short i = 0; i < boardPlayers; i++){
        if(board.boardArray[cellNum][i]){
            *count = board.boardArray[cellNum][i];
            *color = i;
        }
    }
}


//Non playerbased logic
//srand(time(NULL));
int roll(){
    return (rand()%6 +1);
}

int coinFlip(){
    return (rand()%2);
}
//head - 1
//tail - 0

//Player based Logic


int pieceToBoard(int rollVal,Player *player){

    if(rollVal==6){
            Piece *piece = nextPiece(player);
        if (piece != NULL){
            
            piece->notAtBase=1;
            piece->position=player->approachLocation;
            (player->piecesAtPlay)++;
            board.boardArray[piece->position][player->index] += 1;

            return 0;
        }
        return 2;
    }
    return 1;
}
//return 0 - Completed
//return 1 - roll not 6
//return 2 - no pieces currently at Base

int pieceToBase(Piece *piece){
    if(piece->notAtBase){
        board.boardArray[piece->position][piece->owner->index] -= 1;
        piece->notAtBase=0;
        piece->position=0;
        (piece->owner->piecesAtPlay)--;

        return 0;
    }
    return 1;
}
//return 0 - completed
//return 1 - piece is not in play

int spin(Piece *piece){
    if(piece->notAtBase){
        piece->rotation = (coinFlip()) ? clockwise:counterClockwise;
    }
}

Piece *nextPiece(Player *player){
    int piecesNotAtBase = (player->piecesAtHome)+(player->piecesAtPlay);
    if(piecesNotAtBase != playerPieces){
        return &(player->pieceArr[piecesNotAtBase]);
    }
    return NULL;

}

/*
int posCalc(int currentPos,int move){
    if(move>=0){
        if((currentPos+move)>=(boardLength)){
            return (currentPos+move) - boardLength;
        }
    }else{
        if((currentPos+move)<0){
            return (currentPos+move) +boardLength;
        }
    }
    return currentPos+move;
}
 */

int posCalc(int currentPos,int move,int direction){
    //if move is used to give direction too the return code can be simplified.
    if(direction){
        //counterclockwise
        if((currentPos-move) < 0){
            return (currentPos-move) + boardLength;
        }else{
            return currentPos-move;
        }
        
    }else{
        //clockwise
        if((currentPos+move)>=(boardLength)){
            return (currentPos+move) - boardLength;
        }else{
            return currentPos+move;
        }
    }
}

int movement(Piece *piece,int rollVal){
    int newPos = posCalc(piece->position,rollVal,piece->rotation);
    int opCount;
    int playerIndex;

    //find how many pieces recide in the new space and which colour
    for(short i=0; i<playerPieces;i++){
        if(board.boardArray[newPos][i]){
            opCount=board.boardArray[newPos][i];
            playerIndex=i;
            break;
        }
    }
    if(playerIndex == piece->owner->index){
        //making a block
        piece->position=newPos;
        board.boardArray[newPos][playerIndex] += 1;
    }else if(opCount == 1){
        piece->position=newPos;
        board.boardArray[newPos][piece->owner->index] += 1;
        pieceToBase();
    }


    
}