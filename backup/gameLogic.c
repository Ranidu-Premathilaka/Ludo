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
int troopToBoard(int ,Player *);
int troopToBase(Troop *);
int spin(Troop *);
Troop *nextTroop(Player *);
int movement(Troop *,int);
int posCalc(int,int,int);

//Player
void PlayerInit(Player *player,char *playerName){
    static int count = 0;

    player->name= playerName;
    player->troopsAtPlay=0;
    player->troopsAtHome=0;
    player->index = count;
    player->startingLocation = 13*count;
    player->approachLocation = posCalc(13*count,2,1);
    count++;
    
    for (short i = 0; i < playerTroops; i++){
        player->troopArr[i].name[0] = 'R';
        player->troopArr[i].name[1] = '0'+i;
        player->troopArr[i].name[2] = "\0";
        player->troopArr[i].position = 0;
        player->troopArr[i].notAtBase = 0;
        player->troopArr[i].owner = player;
        player->troopArr[i].rotation = clockwise;
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


int troopToBoard(int rollVal,Player *player){

    if(rollVal==6){
            Troop *troop = nextTroop(player);
        if (troop != NULL){
            
            troop->notAtBase=1;
            troop->position=player->approachLocation;
            (player->troopsAtPlay)++;
            board.boardArray[troop->position][player->index] += 1;

            return 0;
        }
        return 2;
    }
    return 1;
}
//return 0 - Completed
//return 1 - roll not 6
//return 2 - no troops currently at Base

int troopToBase(Troop *troop){
    if(troop->notAtBase){
        board.boardArray[troop->position][troop->owner->index] -= 1;
        troop->notAtBase=0;
        troop->position=0;
        (troop->owner->troopsAtPlay)--;

        return 0;
    }
    return 1;
}
//return 0 - completed
//return 1 - troop is not in play

int spin(Troop *troop){
    if(troop->notAtBase){
        troop->rotation = (coinFlip()) ? clockwise:counterClockwise;
    }
}

Troop *nextTroop(Player *player){
    int troopsNotAtBase = (player->troopsAtHome)+(player->troopsAtPlay);
    if(troopsNotAtBase != playerTroops){
        return &(player->troopArr[troopsNotAtBase]);
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

int movement(Troop *troop,int rollVal){
    int newPos = posCalc(troop->position,rollVal,troop->rotation);
    int opCount;
    int playerIndex;

    //find how many troops recide in the new space and which colour
    for(short i=0; i<playerTroops;i++){
        if(board.boardArray[newPos][i]){
            opCount=board.boardArray[newPos][i];
            playerIndex=i;
            break;
        }
    }
    if(playerIndex == troop->owner->index){
        //making a block
        troop->position=newPos;
        board.boardArray[newPos][playerIndex] += 1;
    }else if(opCount == 1){
        troop->position=newPos;
        board.boardArray[newPos][troop->owner->index] += 1;
        troopToBase();
    }


    
}