#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "struct.h"
//#include "functions.h"


Board board[boardLength];
Player playerArray[4];
int globalMysteryCell = -1;
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
char *directMove(Troop *,int ,int );
int movement(Troop *,int ,char *[]);

int canBlockMove(Block *,int ,int *);
int canDirectMoveBlock(Block *,int );
char *directBlockMove(Block *,int , int );
int blockMovement(Block *,int , char *[]);

int canTroopToHome(Troop *,int);
void troopToHome(Troop *);
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
void blockBreak(int,int,Block *[]);
void genUniqueNum(int,int *);
void printEnd(int );


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

int troopToBoard(int rollVal,Player *player,char *logArray[]){

    if(rollVal==maxRollVal){
            Troop *troop = nextTroop(player);
        if (troop != NULL){
            logArray[0] = troop->name;

            int count = board[player->startingLocation].troopCount;
            Player *opPlayer = NULL;
            if (count > 0){
                opPlayer =board[player->startingLocation].troop->owner;
            }
            
            
            if(opPlayer != player && count > 1){
                logArray[1] = board[player->startingLocation].block->name;
                return 4;
            }
            
            troop->where=1;
            troop->position=player->startingLocation;
            spin(troop);
            (player->troopsAtPlay)++;
            (player->troopsAtBase)--;

            if(count > 0){
                //block creation
                if (opPlayer == player){
                    int pos = player->startingLocation;
                    blockCreation(troop,pos,&board[pos]);
                    board[pos].block->rotation = calcBlockSpin(pos);
                    return 0;
                }else{
                //already checked if count == 1;
                    logArray[1] = board[player->startingLocation].troop->name;
                    Elimination(board[player->startingLocation].troop);
                    troop->captures += 1;
                    board[player->startingLocation].troopCount =1;
                    board[player->startingLocation].troop = troop;
                    return 5;
                }
            }else{
                board[player->startingLocation].troop = troop;
                board[player->startingLocation].troopCount = 1;

                return 1;
            }
        }
        return 2;
    }
    return 3;
}
//return 0 - Block Created
//return 1 - successfully moved
//return 2 - All troops in board
//return 3 - Roll not a 6
//return 4 - Blocked by opponent
//return 5 - Eliminated opponent

int spin(Troop *troop){
    if(troop->where == 1){
        troop->rotation = (coinFlip()) ? clockwise:counterClockwise;
    }
}


Troop *nextTroop(Player *player){
    for (short i = 0; i <= playerTroops; i++){
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
    //the count can't be 0;

    int count = boardCell->troopCount;

    if(count == 1){
        Block *block = (Block *)malloc(sizeof(Block));

        if (block == NULL) {
            printf("Couldn't assign memory to block\n");
            exit(1);
        }
        block->troopArr[0] = board[position].troop;    
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
    if(!rollVal){return -2;}
    //if (!canTroopToHome(troop,rollVal)){return 1;}

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
int canDirectMove(Troop *troop,int newPos){
    if(board[newPos].troopCount == 1){

        if(board[newPos].troop->owner == troop->owner){return 4;}
        else{return 5;}

    }else if(board[newPos].troopCount == 0){return 6;}
    return -7;
}

//return 4 - troop increments to it's own block
//return 5 - troop eliminates opponent troop
//return 6 - troop moves freely without elimination
//return -7 - troop is blocked by opponent 
char *directMove(Troop *troop,int newPos,int log){
    char *ptr;
    switch (log){
    case 4:
        removeTroopCell(troop,&board[troop->position]);
        blockCreation(troop,newPos,&board[newPos]);
        board[newPos].block->rotation = calcBlockSpin(newPos);
        troop->position=newPos;
        ptr = board[newPos].block->name;
        break;

    case 5:
        removeTroopCell(troop,&board[troop->position]);
        ptr = board[newPos].troop->name;
        Elimination(board[newPos].troop);

        troop->position=newPos;
        troop->captures += 1;
        board[newPos].troopCount =1;
        board[newPos].troop = troop;
        break;

    case 6:
    //return value is not declared
        removeTroopCell(troop,&board[troop->position]);
        troop->position=newPos;
        board[newPos].troopCount =1;
        board[newPos].troop = troop;
        break;

    case -7:
        ptr = board[newPos].block->name;
        break;
    }

    return ptr;
}

int movement(Troop *troop,int rollVal,char *logArray[]){
    int newPos;
    int log = canMove(troop,rollVal,&newPos);
    switch (log){
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

    default:
        logArray[1] = directMove(troop,newPos,log);
        break;
    }
    return log;
}



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
int canDirectMoveBlock(Block *block,int newPos){
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
    if(log != 4){
        if (log == 5){board[oldPos].troop->captures++;}
        directMove(board[oldPos].troop,newPos,6);
    }
    
    int count = board[oldPos].troopCount;
    for (short i = 0; i < count; i++){
        Troop *troop = board[oldPos].troop;
        directMove(troop,newPos,4);
        if (log == 5){troop->captures++;}
    }

    if (log == 5){return elimBlockName;}

}

int blockMovement(Block *block,int rollVal, char *logArray[]){
    int newPos;
    int log = canBlockMove(block,rollVal,&newPos);
    int oldPos = block->troopArr[0]->position;
    int count = board[oldPos].troopCount;
    
    rollVal = rollVal/count;

        switch (log){
        case -1:
            break;

        case 1:
            Board *board = &block->troopArr[0]->owner->homeRow[oldPos];
            for (short i = 0; i < count; i++){
                troopToHome(board->troop);
            }
            break;

        case 2:
        //for loop needed
            for (short i = 0; i < count; i++){
                approachPassed(board[oldPos].troop,rollVal,block->rotation);
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
            blockCreation(troop,rollVal,&homeRow[rollVal]);
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

int optionFinder(Player *player,int *optionArray,Block *block[],int rollVal){
    int optionCount = 0;

    //find which troops can make a valid move
    for (short i = 0; i < playerTroops; i++){
        Troop *troop = &player->troopArr[i];
        int log = canMove(troop,rollVal,NULL);
        if(log > 0){
            optionArray[i+1] = log;
            optionCount++;
        } 
    }

    //troop to board
    if(rollVal==maxRollVal && player->troopsAtBase > 0){
        optionArray[0] = 1;
        optionCount++;
    }

    //finding blocks
    int blockCount=blockFinder(player,block);

    for (short i = 0; i < blockCount; i++){
        int log = canBlockMove(block[i],rollVal,NULL);
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
    int log;
    int logCode = 0;
    int streak = 0;
    char *logArray[2];  //0- moved name,1- eliminated troop name

    //some of the cases do not happen do to the functions above are implemented
    //remove them accordingly
    int endingPos;
    if(option == 0){
        log = troopToBoard(rollVal,currentPlayer,logArray);
        int playersAtBase = currentPlayer->troopsAtBase;
        endingPos = currentPlayer->startingLocation;
        switch (log){
            case 0:
                printf("Block created on starting point. \n");
                break;
            case 1:
                break;
            case 2:
                printf("Color %s All troops are in play.\n",currentPlayer->name);
                //goto invalidOption;
                break;
            case 3:
                printf("Color %s Roll Not a 6.\n",currentPlayer->name);
                //goto invalidOption;
                break;
            case 4:
                printf("blocked by opponent %s\n",logArray[1]);
                //goto invalidOption;
                break;
            case 5:
                printf("%s eliminated %s\n",logArray[0],logArray[1]);
                break;
        
        }

        printf("Color %s Moves %s to the starting point. \n",currentPlayer->name,logArray[0]);
        printf("Player now has %d/4 of pieces on board and  %d/4 number of pieces on base \n",
                currentPlayer->troopsAtPlay,playersAtBase);

    }else if(option > 0 && option <= 4){
        // troop in board movement
        Troop *troop = &currentPlayer->troopArr[option-1];
        logArray[0]=troop->name;

        int startingPos = troop->position;
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
            case 0:
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
        Block *currentBlock = block[option-5];
        Troop *troop = currentBlock->troopArr[0];

        int startingPos = currentBlock->troopArr[0]->position;
        log = blockMovement(currentBlock,rollVal,logArray);
        endingPos =troop->position;

        if(log > 3){
            if(isApproachPassed(troop,startingPos,rollVal,troop->rotation) < 2){
                int count = board[startingPos].troopCount;
                for (short i = 0; i < count; i++){
                    currentBlock->troopArr[i]->approachPassed += 1;
                }
                printf("Approach passed Not enough captures or rotations on One or more troops in block.\n");
            }
        }

        logArray[0]=board[endingPos].block->name;
        char *direction=(board[endingPos].block->rotation) ? "counter clockwise":"clockwise";

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
                printf("Color %s Block %s ",currentPlayer->name,logArray[0]);
                printf("moves from L%d to L%d in %s direction.\n",startingPos,endingPos,direction); 
            }
}else if(option == 7){
    printf("Color %s Turn skipped \n",currentPlayer->name);
}
/*
if(endingPos == globalMysteryCell){
    printf("Color <colour> landed <pieceName> on A mysteryCell!\n");
    int choice = mysteryCellOptions();
    mysteryCellEffect(choice);
}
*/
return logCode;
}
/*
void createMysteryCell(int elapsedRounds){
    if ((elapsedRounds-2)%4 == 0){
        int mysteryCell;
        while(1){
            mysteryCell = rand()%boardLength;
            if(!board[mysteryCell].troopCount){
                globalMysteryCell = mysteryCell;
            }
        }
    }
}

//use inline keyword for optimizations?
int mysteryCellOptions(){
    return roll();
}

void mysteryCellEffect(int option){
//using function pointers would be good to reduce code
//but switches are optimized to jump in most compilers
//thus for readablity since it's only 6 cases switches
//are chosen
    
    switch (option)
    {
    case 1:
        effectOption1();
        break;

    case 2:
        effectOption2();
        break;

    case 3:
        effectOption3();
        break;

    case 4:
        effectOption4();
        printf("Piece sent back to Base.\n");
        break;

    case 5:
        int log = effectOption5();

        break;

    case 6:
        effectOption6();
        break;

    default:
        printf("invalid option for mystery Cell effect closing program.\n");
        exit(1);
        break;
    }
}

void effectOption1(){
//bhawana (9th)
    int currentPos = globalMysteryCell;


}
void effectOption2(){
//koduwa (27th)
    int currentPos = globalMysteryCell;

}
void effectOption3(){
//Pita-kotuwa (46th)

}
void effectOption4(){
//To base
    Elimination(board[globalMysteryCell].troop);
}
int effectOption5(){
    
    int currentPos = globalMysteryCell;
    int troopCount = board[currentPos].troopCount;
    Player *player = board[currentPos].troop->owner;
    int newPos = player->startingLocation;

    char *logArray[2];
    for (short i = 0; i < troopCount; i++){
        directMove(board[currentPos].troop,newPos,logArray);
    }
    
//To starting position

    int currentPos = globalMysteryCell;
    int troopCount = board[currentPos].troopCount;
    Player *player = board[currentPos].troop->owner;
    int newPos = player->startingLocation;

    Player *opPlayer = board[newPos].troop->owner;
    int opTroopCount = board[newPos].troopCount;
    
    //check if move possible
    if (opPlayer != NULL){
        if(opPlayer != player){
            if (troopCount == opTroopCount){
                Elimination(board[newPos].troop);
            }else{
                return 1;
            }   
        }
    }

    Troop *currentTroop = board[currentPos].troop;
    removeTroopCell(currentTroop);
    currentTroop->position = newPos;
    board[newPos].troopCount = 1;

    for (short i = 0; i < troopCount; i++){
        
        removeTroopCell(currentTroop);
        blockCreation(currentTroop,newPos,&board[newPos]);
        calcBlockSpin(newPos);
        troop->position=newPos;
    }

}
void effectOption6(){
//Approach of the piece color

}
*/