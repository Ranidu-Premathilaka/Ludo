#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "struct.h"
//#include "functions.h"


Board board[boardLength];
Player playerArray[4];
int globalMysteryCell = -1;
int winnerPlacement = 0;
int elapsedRounds=-1;
//Block *blockArr[boardPlayers][maxBlocks];

//to stop repeatedly typing struct 
//Posibility of Null pointarray dereferencing
//turn char methods into int not worth it


//functiondef


void PlayerInit(Player*,char*);
void boardInit();
int roll();
int coinFlip();
int troopToBoard(int ,Player *,char *[]);
int spin(Troop *);
Troop *nextTroop(Player *);
int posCalc(int,int,int);
int troopReset(Troop *);
int Elimination(Troop *);
int lRotation(int position,int approachLocation);
int calcBlockSpin(int);
void blockCreation(Troop *,int,Board *);
void updateBlockName(int,Board *);
void removeTroopCell(Troop *,Board *);
void blockDeletion(Board *);
int nextBlock(Troop *,int,int);

int canMove(Troop *,int ,int *);
int canDirectMove(Troop *,int );
char *directMove(Troop *,int ,int,int);
int movement(Troop *,int ,char *[]);

int canBlockMove(Block *,int ,int *);
int canDirectMoveBlock(Block *,int );
char *directBlockMove(Block *,int , int );
int blockMovement(Block *,int , char *[]);

int canTroopToHome(Troop *,int);
int hasPlayerWon(int);
void troopToHome(Troop *);
void playerWinPlacement(Player *);
int approachDistance(int,int,int);
int isApproachPassed(Troop *,int,int,int);
void approachPassed(Troop *,int,int);
int isBlockApproachPassed(Block *,int,int);
int isGameOver(Player *);
int optionFinder(Player *,int *,Block *[],int );
int blockFinder(Player *,Block *[]);
int firstPlayer(char *[]);
int randBot(int ,int *);
void printBoard();
void orderPrint(int,char *[]);
void playerTurn(int );
int game(Player *,int,int,Block *[]);
void createMysteryCell();
void blockBreak(int, int, Block *[]);
void genUniqueNum(int,int *);
void printEnd(int );
int mysteryCellOptions();
void manageEffect(Player *);
void mysteryCellEffect(int ,int );
void mysteryLand(int, int ,char *,char *);
int effectOption1(int );
void effectOption2(int );
int effectOption3(int );
void effectOption4(int );
int canForceMove(int ,int );
int forceMove(int ,int,int );
//Player
void PlayerInit(Player *player,char *playerName){
    static int count = 0;
    //this can be an issue if restarting the game

    player->name= playerName;
    player->troopsAtPlay=0;
    player->troopsAtHome=0;
    player->troopsAtBase=playerTroops;
    player->index = count;
    player->startingLocation = 13*count + 2; //posCalc(13*count,2,counterClockwise) if more than 4 players hehe;
    player->approachLocation = 13*count;
    player->isWinner = 0;
    count++;
    
    for (short i = 0; i < playerTroops; i++){
        player->troopArr[i].name[0] = playerName[0];
        player->troopArr[i].name[1] = '1'+i;
        player->troopArr[i].name[2] = '\0';
        player->troopArr[i].position = 0;
        player->troopArr[i].where = 0;
        player->troopArr[i].captures = 0;
        player->troopArr[i].owner = player;
        player->troopArr[i].rotation = clockwise;
        player->troopArr[i].approachPassed = 0;
        player->troopArr[i].index = i;
        player->troopArr[i].mysteryEffect[0] = 0;
        player->troopArr[i].mysteryEffect[1] = 0;

    }
    for (short i = 0; i < homeRowLength; i++){
        player->homeRow[i].block = NULL;
        player->homeRow[i].troop = NULL;
        player->homeRow[i].troopCount = 0;
        
    }
}

//board

void boardInit(){
    for (short i = 0; i <boardLength ; i++){
        board[i].block = NULL;
        board[i].troop = NULL;
        board[i].troopCount = 0;
    }
}



//Non playerbased logic
int roll(){
    return (rand()%maxRollVal +1);
}

int coinFlip(){
    return (rand()%2);
}
//head - 1
//tail - 0

//Player based Logic

//return -1 - roll not enough
//return -2 - no troops in base. (this is a serious error)
//return 4 - troop increments to it's own block
//return 5 - troop eliminates opponent troop
//return 6 - troop moves freely without elimination
//return -7 - troop is blocked by opponent 
int canTroopToBoard(int rollVal,Player *player,Troop **troopPtr){
    if(rollVal != maxRollVal){return -1;}
    if(player->troopsAtBase == 0){return -2;}

    Troop *troop = nextTroop(player);
    if(troopPtr != NULL){*troopPtr = troop;}

    return canDirectMove(troop,player->startingLocation);
}

int troopToBoard(int rollVal,Player *player,char *logArray[]){
    Troop *troop;
    int log = canTroopToBoard(rollVal,player,&troop);

    switch (log){
    case -1:
        break;

    case -2:
        break;

    case -7:
        logArray[1] = directMove(troop,player->startingLocation,log,1);

    default:
        logArray[0] = troop->name;

        troop->where=1;
        spin(troop);
        (player->troopsAtPlay)++;
        (player->troopsAtBase)--;

        logArray[1] = directMove(troop,player->startingLocation,log,1);
        break;
    }

    return log;

}

int spin(Troop *troop){
    if(troop->where == 1){
        troop->rotation = (coinFlip()) ? clockwise:counterClockwise;
    }
}


Troop *nextTroop(Player *player){
    for (short i = 0; i < playerTroops; i++){
        //implment to check out winners or troops in home already
        if(player->troopArr[i].where == 0){
            return &(player->troopArr[i]);
        }
    }
    return NULL;
    
}

int posCalc(int currentPos,int move,int direction){
    //if move is used to give direction too the return code can be simplified

    if(direction){
        return (currentPos-move+boardLength)%boardLength;       
        
    }else{
        return (currentPos+move)%boardLength;
    }
}

int troopReset(Troop *troop){
    board[troop->position].troopCount -= 1;
    (troop->owner->troopsAtPlay)--;
    (troop->owner->troopsAtBase)++;
    troop->position = 0;
    troop->captures = 0;
    troop->approachPassed = 0;
    troop->where = 0;
    troop->rotation = clockwise;
    troop->mysteryEffect[0] = 0;
    troop->mysteryEffect[1] = 0;
    
}

//could be optimized to only have a pos parameter
int Elimination(Troop *troop){
    if(troop->where != 1){
        return 0;
    }
    int pos = troop->position;
    int troopCount = board[pos].troopCount;

    if(troopCount ==1){
        troopReset(troop);
        board[pos].troop = NULL;

        return 1;
    }else{
        for(short i = 0; i<troopCount; i++){
            troopReset(board[pos].block->troopArr[i]);
        }
        blockDeletion(&board[pos]);
        board[pos].troop = NULL;
        return 2;
    }
}
//return 0 - troop not in play
//return 1 - troop removed
//return 2 - Block removed


//can be used to get if the block needs to travel the longest path 
//if position has 1 troop the longest path rotation
    //Think the algorthm can be more optimized for now should be okay.
    //Longest rotation
int lRotation(int position,int approachLocation){
    if(approachLocation>position){
        return ((approachLocation-position)>boardLength/2) ? (clockwise):(counterClockwise);
    }else{
        return ((position - approachLocation)>boardLength/2) ? (counterClockwise):(clockwise);
    }
}

//could be optimized to do it instantly without returns 
int calcBlockSpin(int position){

    int count = board[position].troopCount;
    int approachLocation = board[position].troop->owner->approachLocation;
    int longestRotation;

    longestRotation = lRotation(position,approachLocation);

    for(short i = 1; i<count; i++){
        Troop *troop1 = board[position].block->troopArr[i-1];
        Troop *troop2 = board[position].block->troopArr[i];
        if(troop1->rotation != troop2->rotation){
            return longestRotation;
        }
    }

    return board[position].troop->rotation;
}

//choose to dynammically alocate blocks due to the rarety of it and to save space
void blockCreation(Troop *troop,int position,Board *boardCell){


    int count = boardCell->troopCount;

    if(count == 1){
        Block *block = (Block *)malloc(sizeof(Block));

        if (block == NULL) {
            printf("Couldn't assign memory to block\n");
            exit(1);
        }

        block->mysteryEffect[0] = 0;
        block->mysteryEffect[1] = 0;

        block->troopArr[0] = boardCell->troop;    
        boardCell->block = block;
    }

    boardCell->block->troopArr[count] = troop;
    boardCell->troopCount++;
    updateBlockName(position,boardCell);
}

void updateBlockName(int pos,Board *boardCell){
    int count = boardCell->troopCount; 
    Block *block = boardCell->block;
    int totalChar=0;
    for (short i = 0; i < count; i++){
        for (short j = 0; j < 2; j++){
            block->name[totalChar]=block->troopArr[i]->name[j];
            totalChar++;
        }
        if(i != count-1){
            block->name[totalChar]=',';
            totalChar++;
        }
    }
    block->name[totalChar] = '\0';
}

void removeTroopCell(Troop *troop,Board *boardCell){
        int pos = troop->position;
        int count = boardCell->troopCount;

        //if only one troop exists in cell;
        if(count == 1){
            boardCell->troop = NULL;
            boardCell->troopCount = 0;
            return;
        }

        //if more than one troop exist in cell;
        Block *block = boardCell->block;
        int index;

        for(short i = 0; i<count; i++){
            if(block->troopArr[i]==troop){
                index = i;
                break;
            }
        }
        for(short i = index; i<count-1; i++){
            block->troopArr[i] = block->troopArr[i+1];
        }
        block->troopArr[count-1] = NULL;
        if(index == 0){
            boardCell->troop = block->troopArr[index];
        }
        boardCell->troopCount--;
        //if removed cell removes the block into a single troop
        if(boardCell->troopCount == 1){
            blockDeletion(boardCell);
        }else{
            boardCell->block->rotation = calcBlockSpin(pos);
            updateBlockName(pos,boardCell);
        }
}


void blockDeletion(Board *boardCell){
    if(boardCell->block != NULL){
        free(boardCell->block);
        boardCell->block = NULL;
        //return 0;
    }
    //return 1;
}
//return 1 -no Block in this position
//return 0 - Block removed


int nextBlock(Troop *troop,int rollVal,int rotation){

    for (short i = 1; i <rollVal; i++){
            int nextPos = posCalc(troop->position,i,rotation);
            if(board[nextPos].block != NULL && ((board[nextPos].troop->owner) != (troop->owner))){
                return posCalc(troop->position,i-1,rotation);
            }
        }
    //no block exists in between
    return posCalc(troop->position,rollVal,rotation);
}

//use negative for unsuccessfull moves

//return -4 - mystery cell blocked
//return -2 - rollVal is 0
//return 1 - can Move troop to home
//return -1 - can't move troop
//return 2 - troop is in the home straight
//return -3 - troop is blocked by block/ no valid moves
//return 4 - troop increments to it's own block
//return 5 - troop eliminates opponent troop
//return 6 - troop moves freely without elimination
//return -7 - troop is blocked by opponent 
int canMove(Troop *troop,int rollVal,int *newPosPtr){

    if(troop->mysteryEffect[0] == 3){return -4;}

    if(!rollVal){return -2;}
    //if(!canTroopToHome(troop,rollVal)){return 1;}

    if(troop->where != 1 ){return -1;}

    //if(!isApproachPassed(troop,troop->position,rollVal,troop->rotation)){return 2;}

    int newPos=nextBlock(troop,rollVal,troop->rotation);
    if(newPosPtr != NULL){*newPosPtr = newPos;}

    if(newPos == troop->position){return -3;}

    return canDirectMove(troop,newPos);
}

//return 4 - troop increments to it's own block
//return 5 - troop eliminates opponent troop
//return 6 - troop moves freely without elimination
//return -7 - troop is blocked by opponent 
//return -8 - troop is already in position
int canDirectMove(Troop *troop,int newPos){

    if(troop->position == newPos){return -8;}

    if(board[newPos].troopCount){

        if(board[newPos].troop->owner == troop->owner){return 4;}
        else if(board[newPos].troopCount == 1){return 5;}
        else{return -7;}

    }else if(board[newPos].troopCount == 0){return 6;}
}

//return 4 - troop increments to it's own block
//return 5 - troop eliminates opponent troop
//return 6 - troop moves freely without elimination
//return -7 - troop is blocked by opponent 
char *directMove(Troop *troop,int newPos,int log,int mysteryCheck){
    char *ptr;
    switch (log){
    case 4:
        blockCreation(troop,newPos,&board[newPos]);
        board[newPos].block->rotation = calcBlockSpin(newPos);
        troop->position=newPos;
        ptr = board[newPos].block->name;
        break;

    case 5:
        ptr = board[newPos].troop->name;
        Elimination(board[newPos].troop);

        troop->position=newPos;
        troop->captures += 1;
        board[newPos].troopCount =1;
        board[newPos].troop = troop;
        break;

    case 6:
    //return value is not declared
        troop->position=newPos;
        board[newPos].troopCount =1;
        board[newPos].troop = troop;
        break;

    case -7:
        ptr = board[newPos].block->name;
        break;
    }
    if(mysteryCheck){
        mysteryLand(log,newPos,troop->owner->name,troop->name);
    }
    return ptr;
}

int movement(Troop *troop,int rollVal,char *logArray[]){
    int newPos;
    int log = canMove(troop,rollVal,&newPos);
    switch (log){
    case -4:
        break;
    case -2:
        break;

    case 1:
        troopToHome(troop);
        break;

    case -1:
        break;

    case 2:
        approachPassed(troop,rollVal,troop->rotation);
        break;

    case -3:
        logArray[1] = board[posCalc(troop->position,1,troop->rotation)].block->name;
        break;

    case -7:
        logArray[1] = directMove(troop,newPos,log,1);

    default:
        removeTroopCell(troop,&board[troop->position]);
        logArray[1] = directMove(troop,newPos,log,1);
        break;
    }
    return log;
}


//return -2 - mysteryEffect can't move
//return -1 - not enough roll
//return 1 - can move Block to home
//return 2 - block can move into home straight
//return -3 - Block can't move due to opponent block in front
//return 4 - landed block same player thus creating a bigger block
//return 5 - opponent block but count is the same thus eliminates
//return -6 - opponent block but count is different thus can't move
//return 7 - can move block freely
int canBlockMove(Block *block,int rollVal,int *newPosPtr){
    Troop *troop = block->troopArr[0];
    int oldPos = troop->position;
    int count = board[oldPos].troopCount;

    rollVal = rollVal/count;

    if(block->mysteryEffect[0] == 3){return -2;}

    if(!rollVal){return -1;}

    //if(!canTroopToHome(troop,rollVal)){return 1;}

    //if(!isBlockApproachPassed(block,oldPos,rollVal)){return 2;}

    int newPos = nextBlock(troop,rollVal,block->rotation);
    if(newPosPtr != NULL){*newPosPtr = newPos;}

    if(newPos == oldPos){return -3;}

    return canDirectMoveBlock(block,newPos);
}

//return 4 - landed block same player thus creating a bigger block
//return 5 - opponent block but count is the same thus eliminates
//return -6 - opponent block but count is different thus can't move
//return 7 - can move block freely
//return -8 - block is already in new Position
int canDirectMoveBlock(Block *block,int newPos){

    if(block->troopArr[0]->position == newPos){return -8;}

    if(board[newPos].troopCount){
        if(board[newPos].troop->owner == block->troopArr[0]->owner ){return 4;}
        if(board[newPos].troopCount == board[block->troopArr[0]->position].troopCount){return 5;}
        return -6;
    }
    return 7;
}

//implement more readably and maybe implement with candirectmove
char *directBlockMove(Block *block,int newPos, int log){

    char *elimBlockName = board[newPos].block->name;
    if(log == -6 ){return elimBlockName;}
    if (log == 5){Elimination(board[newPos].troop);}

    int oldPos = block->troopArr[0]->position;

    int mysteryEffectTemp[mysteryLog];
    if(block->mysteryEffect[0]){
        mysteryEffectTemp[0] = block->mysteryEffect[0];
        mysteryEffectTemp[1] = block->mysteryEffect[1];
    }

    if(log != 4){
        if (log == 5){board[oldPos].troop->captures++;}
        Troop *troop = board[oldPos].troop;
        removeTroopCell(troop,&board[oldPos]);
        directMove(troop,newPos,6,0);
    }
    
    int count = board[oldPos].troopCount;
    for (short i = 0; i < count; i++){
        Troop *troop = board[oldPos].troop;
        removeTroopCell(troop,&board[oldPos]);
        directMove(troop,newPos,4,0);
        if (log == 5){troop->captures++;}
    }

    if(block->mysteryEffect[0]){
        board[newPos].block->mysteryEffect[0] = mysteryEffectTemp[0];
        board[newPos].block->mysteryEffect[1] = mysteryEffectTemp[1];
    }

    if (log == 5){return elimBlockName;}

    mysteryLand(log,newPos,board[newPos].troop->owner->name,board[newPos].block->name);

}

int blockMovement(Block *block,int rollVal, char *logArray[]){
    int newPos;
    int log = canBlockMove(block,rollVal,&newPos);
    int oldPos = block->troopArr[0]->position;
    int count = board[oldPos].troopCount;
    
    rollVal = rollVal/count;

        switch (log){
        case -2:
            break;
        case -1:
            break;

        case 1:
            Board *homeRowBoard = &block->troopArr[0]->owner->homeRow[oldPos];
            for (short i = 0; i < count; i++){
                troopToHome(homeRowBoard->troop);
            }
            break;

        case 2:
        //for loop needed
            int rotation = block->rotation;
            for (short i = 0; i < count; i++){
                approachPassed(board[oldPos].troop,rollVal,rotation);
            }

            break;

        case -3:
            logArray[1] = board[posCalc(oldPos,1,block->rotation)].block->name;
            break;

        default:
            logArray[1] = directBlockMove(block,newPos,log);
            break;
        }
    return log;
}

int hasPlayerWon(int playerIndex){
    if (playerArray[playerIndex].isWinner){return 1;}
    else {return 0;}
}

int canTroopToHome(Troop *troop,int rollVal){
    if(troop->where == 2 && rollVal == (homeRowLength -1) - troop->position){
        return 0;
    }
    return 1;
}

void troopToHome(Troop *troop){
    int pos = troop->position;
    Player *owner = troop->owner;
    removeTroopCell(troop,&owner->homeRow[pos]);
    
    troop->where = 3;
    owner->troopsAtHome++;
    owner->troopsAtPlay--;
    playerWinPlacement(owner);
}

void playerWinPlacement(Player *player){
    if(player->troopsAtHome == 4){
        player->isWinner = ++winnerPlacement;
    }
}   

int approachDistance(int approachPos, int oldPos, int rotation){
    int rotTemp = (rotation) ? -1:1;
    return (rotTemp*(approachPos - oldPos) + boardLength)%boardLength;
}

//make sure to use the divised rollval when using for blocks
int isApproachPassed(Troop *troop, int oldPos,int rollVal,int rotation){
    int approachPos = troop->owner->approachLocation;

    int oldDist = approachDistance(approachPos,oldPos,rotation);
    rollVal = rollVal - (oldDist+1);
    //int newDist = (rotTemp*(approachPos - newPos) + boardLength)%boardLength;

    //try to use lrotation for computation like longest path rotation changes when passed?
    //break if the rollVal is not enough
    if (rollVal < 0){return 3;}
    //check the cell after the approach 
    int nextBlockPos = nextBlock(troop,oldDist,rotation);
    if((nextBlockPos == posCalc(oldPos,oldDist,rotation)) &&
       (board[nextBlockPos].block == NULL || board[nextBlockPos].troop->owner == troop->owner)){
        //troop->approachPassed += 1;
        if((troop->approachPassed - rotation) >= 0 && troop->captures){
            return 0;
        }
        return 1;
    }else{
        return 2;
    }
}
//return 0 - when passed
//return 1 - not enough captures or approach passes
//return 2 - block in between the path of the approach even though roll was enough
//return 3 - rollValue not enough to pass approach
//return 4 - not at play

void approachPassed(Troop *troop,int rollVal,int rotation){
    rollVal = rollVal -(approachDistance(troop->owner->approachLocation,troop->position,rotation)) -1;

        removeTroopCell(troop,&board[troop->position]);
        rollVal = (rollVal > 4) ? 4:rollVal;
        // if the roll is above 4 then the player will be stopped at 4 to play 1 to finish

        Board *homeRow = &troop->owner->homeRow[rollVal];
        troop->where = 2;
        troop->position = rollVal;
        if(homeRow->troopCount){
            blockCreation(troop,rollVal,homeRow);
        }else{
            homeRow->troop = troop;
            homeRow->troopCount = 1;
        }
}
//return 1 - sucess
//return 0 - not enough captures or passing of aproaches

//only allowed if all troops in it have eliminated
int isBlockApproachPassed(Block *block,int oldPos,int rollVal){

    if(isApproachPassed(block->troopArr[0],oldPos,rollVal,block->rotation) >1){
        return 2;
    }

    int count = board[block->troopArr[0]->position].troopCount;
    for (short i = 0; i < count; i++){
        Troop *troop = block->troopArr[i];
        if((troop->approachPassed - troop->rotation) <0  || troop->captures == 0){
            return 1;
        }
    }   
    return 0;
    
}
//return 0- The move is possible
//return 1- One troop in the block hasn't captured an opponent// 
//return 2- Not clear to pass meaning

int isGameOver(Player *playerArray){
    int count=0;
    for(short i = 0; i<boardPlayers; i++){
        count += playerArray[i].isWinner;
    }
    return (count == endWinningSum) ? 0:1;
}


    //1.Take troop Out from base
    //2.Move standard board peices(2.1-R1,2.2-R2)
    //3.Move block

int realRollVal(void *ptr,int isBlock,int rollVal){

    int mysteryVal;
    if(isBlock){
        Block *block = (Block *)ptr;
        mysteryVal =block->mysteryEffect[0];
    }else{
        Troop *troop = (Troop *)ptr;
        mysteryVal =troop->mysteryEffect[0];
    }

    switch(mysteryVal){
        case 1:
            return rollVal*2;
            break;
        case 2:
            return rollVal/2;
            break;
        default:
            return rollVal;
            break;
    }
}

int optionFinder(Player *player,int *optionArray,Block *block[],int rollVal){
    int optionCount = 0;

    //find which troops can make a valid move
    for (short i = 0; i < playerTroops; i++){
        Troop *troop = &player->troopArr[i];
        int tmpRollVal = realRollVal(troop,0,rollVal);
        int log = canMove(troop,tmpRollVal,NULL);
        if(log > 0){
            optionArray[i+1] = log;
            optionCount++;
        } 
    }

    //troop to board
    if(canTroopToBoard(rollVal,player,NULL) > 0){
        optionArray[0] = 1;
        optionCount++;
    }

    //finding blocks
    int blockCount=blockFinder(player,block);

    for (short i = 0; i < blockCount; i++){
        int tmpRollVal = realRollVal(block[i],1,rollVal);
        int log = canBlockMove(block[i],tmpRollVal,NULL);
        if(log > 0){
            optionArray[i+5] = log;
            optionCount++;
        }
    }
    //skip turn is only allowed if no moves exist
    if(optionCount== 0){
        optionArray[7]=1;
        optionCount++;
    }

    return optionCount;
}

int blockFinder(Player *player,Block *block[]){
    int blockCount = 0;

    for(short i = 0; i < maxBlocks; i++){
        block[i] = NULL;
    }

    for (short i = 0; i < playerTroops; i++){
        if (player->troopArr[i].where == 0 || player->troopArr[i].where == 3){
            continue;
        }
        
        int pos = player->troopArr[i].position;
        Block *blockptr = board[pos].block;
        int chk = 1;

        if(blockptr != NULL){
            for(short j = 0; j <blockCount; j++){
                if(block[j] == blockptr){
                    chk = 0;
                    break;
                }
            }
            if(chk){
                block[blockCount] = blockptr;
                blockCount++;
            }
        }
    }
    return blockCount;
}

int firstPlayer(char *playerName[]){
    int firstPlayer = 0;
    int max = 0;

    for (short i = 0; i < boardPlayers; i++){
        int rollVal = roll();
        if(rollVal>max){
            max = rollVal;
            firstPlayer = i;
        }
        printf("%s rolls %d \n",playerName[i],rollVal);
    }
    return firstPlayer;

}

void orderPrint(int first,char *playerName[]){
    printf("%s has the highest roll and will begin the game.\n",playerName[first]);
    printf("The order of a single round is ");
    int range = boardPlayers+first;
    for (short i = first; i <range ; i++){
        int x = i % boardPlayers;
        int cnt = i-first;
        PlayerInit(&playerArray[cnt],playerName[x]);

        if(i+1 == range){
            printf("and %s\n",playerArray[cnt].name);
        }else{
            printf("%s, ",playerArray[cnt].name);
        }
    }
}

void displayOptionArray(int optionArray[]){
    for (short i = 0; i < 8; i++){
        printf("%d\t",optionArray[i]);
    }
    printf("\n");
}

int randBot(int optionAmount,int *optionArray){
    /*
    if(optionAmount==1 ){
        //skip turn done separately for efficency
        return 7;
    } */
    //processes when option amount is larger
    //same probability
    int randomVal = rand()%optionAmount;

    //choose what block to break
    if(optionArray == NULL){
        return randomVal;
    }

    int count = 0;
    for (int i = 0; i < totalOptions; i++){
        if(optionArray[i]){
            if(count == randomVal){return i;}
            count++;
        }
    }

}

void printBoard(){
    for (short i = 0; i < boardLength; i++){
        printf("%2d",board[i].troopCount);
        if(i%13 == 12){
            printf("\n");
        }
    }
}

void playerTurn(int playerIndex){
    int option;
    int logCode=-1;
    int streak=1;
    Block *block[maxBlocks];

    int rollVal = roll();
    printf("%s rolls %d\n",playerArray[playerIndex].name,rollVal);

    while (1){

        int optionArray[totalOptions]={0};
 
        int optionAmount =optionFinder(&playerArray[playerIndex],optionArray,block,rollVal);
        displayOptionArray(optionArray);
        option = randBot(optionAmount,optionArray);


        logCode = game(&playerArray[playerIndex],rollVal,option,block);

        if(rollVal == maxRollVal){
            streak++;
        }else if(logCode == 1){
            streak = 1;
        }else{
            break;
        }
        
        rollVal = roll();
        printf("%s rolls %d\n",playerArray[playerIndex].name,rollVal);
        if(streak == 3 && rollVal ==6){
            printf("Three concecutive 6's\n");
            int count = blockFinder(&playerArray[playerIndex],block);
            if(count){
                printf("Player has %d blocks \n",count);
                blockBreak(playerIndex,count,block);
            }else{
                printf("Player has no blocks. Skipping turn\n");
            }
            break;
        }

    }

    manageEffect(&playerArray[playerIndex]);
}

//when a unique num is generated there can be a situation where the block isn't broken
//cause the pattern makes the pieces blocked but the break is actually possible if
//the pieces that move in the other is possible to move
void blockBreak(int playerIndex,int count,Block *block[]){
    int choice = randBot(count,NULL);
    printf("Player Choose to break block %s \n",block[choice]->name);

    int pos = block[choice]->troopArr[0]->position;
    int troopCount = board[pos].troopCount;

    //maybe implement direction check too?
    int values[playerTroops]={0};
    genUniqueNum(troopCount,values);

    //troop count 3
    //if we take that a piece has to move
    //where troop count is 4 this would be imposible to move without creating another block
    //thus let's take 0 as a possible move too 

    for (short i = 0; i < troopCount; i++){
        Troop *troop = board[pos].troop;
        game(&playerArray[playerIndex],values[i],troop->index+1,NULL);
    }

}

//fix algorithm current one is bad?
void genUniqueNum(int troopCount, int *result){
    int randomVal = rand();
    if(troopCount == 2){
        char arrangements[3][2] = {"60","51","42"};
        int index = randomVal%3;
        int srt = randomVal%2;
        for (short i = 0; i < 2; i++){

            result[(srt + i)%2] = (int)arrangements[index][i] - '0';
        }
        

    }else if(troopCount == 3){
        char arrangements[3][3] = {"510","420","321"};
        int index = randomVal%3;
        int srt = rand()%3;
        for (short i = 0; i < 3; i++){

            result[(srt + i)%3] = (int)arrangements[index][i] - '0';
        }


    }else if(troopCount == 4){
        int srt = randomVal%4;

        for (short i = 0; i < 4; i++){
            result[(srt + i)%4]=i;
        }
    }
}

void printEnd(int playerIndex){
    //print round results
}


//logCode 0 - no elimination
//logCode 1 - elimination
int game(Player *currentPlayer,int rollVal,int option,Block *block[]){
    int log = 0;
    int logCode = 0;
    int streak = 0;
    int mysteryCheck = 0; //0 - no moves done , 1- troopMoved, 2-Block moved
    char *logArray[2];  //0- moved name,1- eliminated troop name
    Troop *troop;
    Block *currentBlock;

    //some of the cases do not happen do to the functions above are implemented
    //remove them accordingly
    int endingPos;
    if(option == 0){
        log = troopToBoard(rollVal,currentPlayer,logArray);
        int playersAtBase = currentPlayer->troopsAtBase;
        endingPos = currentPlayer->startingLocation;
        troop = board[endingPos].troop;

        switch (log){
            case -1:
                printf("Color %s Roll Not a 6.\n",currentPlayer->name);
                break;
            case -2:
                printf("Color %s All troops are in play.\n",currentPlayer->name);
                break;
            case -7:
                printf("blocked by opponent %s\n",logArray[1]);
                break;
            case 4:
                printf("Block created on starting point L%d. \n",endingPos);
                break;
            case 5:
                printf("%s eliminated %s\n",logArray[0],logArray[1]);
                break;
            case 6:
                break;
        
        }

        printf("Color %s Moves %s to the starting point. \n",currentPlayer->name,logArray[0]);
        printf("Player now has %d/4 of pieces on board and  %d/4 number of pieces on base \n",
                currentPlayer->troopsAtPlay,playersAtBase);

    }else if(option > 0 && option <= 4){
        // troop in board movement
        troop = &currentPlayer->troopArr[option-1];
        logArray[0]=troop->name;

        int startingPos = troop->position;
        rollVal = realRollVal(troop,0,rollVal);
        log = movement(troop,rollVal,logArray);
        endingPos = troop->position;

        if(log > 3){
            if(isApproachPassed(troop,startingPos,rollVal,troop->rotation) == 1){
                troop->approachPassed += 1;
                printf("Approach passed Not enough captures or rotations.\n");
            }
        }

        char *direction = (troop->rotation) ? "counter clockwise":"clockwise";

        printf("Color %s piece %s ",currentPlayer->name,logArray[0]);
        switch (log){
            case -2:
                printf("can't move cause the roll is equivelent to 0\n");
                break;
            case 1:
                printf("Moved into home.\n");
                printf("Player has %d/4 pieces at Home.\n",currentPlayer->troopsAtHome);
                break;
            case -1:
                printf("is not in Play.\n");
                //goto option again.
                break;
            case 2:
                printf("entered Home straight in Position: %d\n",troop->position);
                break;
            case -3:
                printf("Blocked by %s in front and can't move. \n",logArray[1]);
                //goto option again.
                break;
            case 4:
                printf("Created a Block %s on L%d\n",logArray[1],endingPos);
                break;
            case 5:
                printf("Eliminated %s\n",logArray[1]);
                logCode = 1;
                break;
            case 6:
                printf("can Move without Blocks\n");
                break;
            
            case -7:
                printf("is blocked from moving from L%d to L%d by %s piece\n",startingPos,endingPos,logArray[1]);
                //goto option again.
                break;
            
        }
        if(log >3){
                printf("Color %s piece %s ",currentPlayer->name,logArray[0]);
                printf("Moves from location L%d to L%d by %d units in %s Direction. \n",startingPos,endingPos,rollVal,direction);
            }

    }else if(option == 5 || option == 6){
        currentBlock = block[option-5];
        troop = currentBlock->troopArr[0];

        int startingPos = currentBlock->troopArr[0]->position;

        rollVal = realRollVal(currentBlock,1,rollVal);
        log = blockMovement(currentBlock,rollVal,logArray);
        endingPos =troop->position;

        if (log == 1 || log == 2){
            logArray[0]=currentPlayer->homeRow[endingPos].block->name;
        }else{
            logArray[0]=board[endingPos].block->name;
        }
        

        printf("Color %s Block %s ",currentPlayer->name,logArray[0]);
        switch (log){
            case -1:
                printf("not enough roll to move.\n");
                break;
            case 1:
                printf("Moved into home.\n");
                printf("Player has %d/4 pieces at Home.\n",currentPlayer->troopsAtHome);
                break;
            case 2:
                printf("Entered Home straight and at position %d\n",troop->position);
                 
                break;
            case -3:
                printf("Blocked by %s in front and can't move. \n",logArray[1]);
                //goto option again.
                break;

            case 4:
                printf("Created a bigger Block %s in L%d\n",logArray[0],troop->position);
                break;
            case 5:
                printf("eliminated Block %s\n",logArray[1]);
                logCode = 1;
                break;
            case -6:
                printf("Can't move block by Block %s.\n",logArray[1]);
                //goto invalidOption;
                break;
            case 7:
                printf("can Move Block freely\n");
                break;
        }

        if(log >3){
                if(isApproachPassed(troop,startingPos,rollVal,troop->rotation) < 2){
                    int count = board[startingPos].troopCount;
                    for (short i = 0; i < count; i++){
                        currentBlock->troopArr[i]->approachPassed += 1;
                    }
                    printf("Approach passed Not enough captures or rotations on One or more troops in block.\n");
                }

                char *direction=(board[endingPos].block->rotation) ? "counter clockwise":"clockwise";
                printf("Color %s Block %s ",currentPlayer->name,logArray[0]);
                printf("moves from L%d to L%d in %s direction.\n",startingPos,endingPos,direction); 
            }
}else if(option == 7){
    printf("Color %s Turn skipped \n",currentPlayer->name);
}

//mysteryLand(log,endingPos,currentPlayer->name,logArray[0]);
return logCode;
}

void mysteryLand(int log, int endingPos,char *name,char *troop){
    if(log > 3 && endingPos == globalMysteryCell){
        printf("Color %s landed %s on A mysteryCell!\n",name,troop);
        int choice = mysteryCellOptions(board[endingPos].troop->owner);
        mysteryCellEffect(choice,endingPos);
    }
}

void mysteryLand(){
    
    if(board[globalMysteryCell].troopCount){
        char *name = board[globalMysteryCell].troop->owner->name;
        char *troop = (board[globalMysteryCell].troopCount > 1) ? (board[globalMysteryCell].block->name) : board[globalMysteryCell].troop->name;
        int endingPos = board[globalMysteryCell].troop->position;
        printf("Color %s landed %s on A mysteryCell!\n",name,troop);
        int choice = mysteryCellOptions(board[globalMysteryCell].troop->owner);
        mysteryCellEffect(choice,endingPos);
    }
}

void createMysteryCell(){
    if ((elapsedRounds-roundsBeforeMystery)%roundsMysteryReset == 0){
        int mysteryCell;
        while(1){
            mysteryCell = rand()%boardLength;
            if(!board[mysteryCell].troopCount && mysteryCell != globalMysteryCell){
                globalMysteryCell = mysteryCell;
                break;
            }
        }
    }
}

int mysteryCellOptions(Player *player){
        return roll();
}
//use inline keyword for optimizations?
/*
int mysteryCellOptions(Player *player){
    int newPos[6] = {bhawana,kotuwa,pitaKotuwa,-1,player->startingLocation,player->approachLocation};
    while(1){
        int choice = roll();
        if(globalMysteryCell != newPos[choice-1]){
            return choice;
        }
        printf("Teleported back in the mystery cell\n");
    }
}
*/
void manageEffect(Player *player){
    for(short i = 0; i < playerTroops; i++){
        if (elapsedRounds - (player->troopArr[i].mysteryEffect[1]) == effectDuration ){
            player->troopArr[i].mysteryEffect[0] = 0;
            //player->troopArr[i].mysteryEffect[1] = 0;
        } 
    }

    Block *block[maxBlocks];
    int blockCount = blockFinder(player,block);

    for(short i = 0; i < blockCount; i++){
        if (elapsedRounds - (block[i]->mysteryEffect[1]) == effectDuration ){
            block[i]->mysteryEffect[0] = 0;
            //block[i]->mysteryEffect[1] = 0;   
        } 
    }
}

void mysteryCellEffect(int option,int pos){
//using function pointers would be good to reduce code
//but switches are optimized to jump in most compilers
//thus for readablity since it's only 6 cases switches
//are chosen

//if the move isn't possible they stay in mysterycell untill moved 
//or another troop lands on it and makes a block which the mystery effect is acted on both

    int newPos,log=0;
    int count = board[pos].troopCount;
    char *multiple = (count>1) ? "s are":" is";
    
    printf("Player randomly selected Option: ");
    switch (option){
    case 1:
        printf("Bhawana\n");
        newPos = bhawana;
        log = canForceMove(pos,newPos);
        if(log >0){
            printf("piece%s teleported into bhawana \n",multiple);

            char *effect = (effectOption1(pos) == 1) ? "energized":"sick";
            printf("Pieces got %s\n",effect);
            forceMove(pos,newPos,log);
        }
        break;

    case 2:
        printf("Kotuwa\n");
        newPos = kotuwa;
        log = canForceMove(pos,newPos);
        if(log >0){
            printf("piece%s teleported into kotuwa \n",multiple);
            effectOption2(pos);
            forceMove(pos,newPos,log);
        }
        break;

    case 3:
        printf("Pita Kotuwa\n");
        newPos = pitaKotuwa;
        log = canForceMove(pos,newPos);
        if(log >0){
            printf("piece%s teleported into pita Kotuwa \n",multiple);

            if(effectOption3(pos)){
                forceMove(pos,newPos,log);
            }
        }
        break;

    case 4:
        printf("To Base\n");
        printf("Piece%s teleported back to Base.\n",multiple);
        effectOption4(pos);
        break;

    case 5:
        printf("To Starting Position\n");
        newPos = board[pos].troop->owner->startingLocation;
        log = canForceMove(pos,newPos);
        if(log>0){
            printf("Piece%s teleported back to starting Position s.\n",multiple);
            forceMove(pos,newPos,log);
        }
        break;

    case 6:
        printf("To Starting Position\n");
        newPos = board[pos].troop->owner->approachLocation;
        log = canForceMove(pos,newPos);
        if(log>0){
            printf("Price%s teleported to player approach.\n",multiple);
            forceMove(pos,newPos,log);
        }
        break;
    }
    if(log < 0){
        printf("Teleportation Not Possible. Mystery Cell Move ignored.\n");
    }

}

int effectOption1(int pos){
//bhawana (9th)
    int randEffect = coinFlip() + 1;
    if(board[pos].troopCount > 1){
        board[pos].block->mysteryEffect[0] = randEffect;
        board[pos].block->mysteryEffect[1] = elapsedRounds;
    }else{
        board[pos].troop->mysteryEffect[0] = randEffect;
        board[pos].troop->mysteryEffect[1] = elapsedRounds;
    }
    return randEffect;
}
void effectOption2(int pos){
//koduwa (27th)
    if(board[pos].troopCount > 1){
        board[pos].block->mysteryEffect[0] = 3;
        board[pos].block->mysteryEffect[1] = elapsedRounds;
    }else{
        board[pos].troop->mysteryEffect[0] = 3;
        board[pos].troop->mysteryEffect[1] = elapsedRounds;
    }
}
int effectOption3(int pos){
//Pita-kotuwa (46th)
    char *rotation;
    if(board[pos].troopCount > 1){
        rotation = &board[pos].block->rotation;
    }else{
        rotation = &board[pos].troop->rotation;
    }

    if(*rotation){
        int log = canForceMove(pos,kotuwa);
        if(log >0){
            effectOption2(pos);
            forceMove(pos,kotuwa,log);
            return 0;
        }
    }else{
        *rotation = counterClockwise;
        return 1;
    }
}
void effectOption4(int pos){
//To base
    Elimination(board[pos].troop);
}

int canForceMove(int pos,int endingPos){
    if(board[pos].troopCount > 1){
        return canDirectMoveBlock(board[pos].block,endingPos);
    }else{
        return canDirectMove(board[pos].troop,endingPos);
    }
}

int forceMove(int pos,int endingPos,int log){
//to starting position or approach 
    if(board[pos].troopCount > 1){
        directBlockMove(board[pos].block,endingPos,log);
    }else{
        Troop *troop = board[pos].troop;
        removeTroopCell(troop,&board[pos]);
        directMove(troop,endingPos,log,1);
    }
    return log;
}

