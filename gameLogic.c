#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "struct.h"
//#include "functions.h"


Board board[boardLength];
Player playerArray[4];

//to stop repeatedly typing struct 
//Posibility of Null pointarray dereferencing
//turn char methods into int not worth it


//functiondef

void PlayerInit(Player*,char*);
void boardInit();
int roll();
int coinFlip();
int troopToBoard(int ,Player *,char *[]);
int troopToBase(Troop *);
int spin(Troop *);
Troop *nextTroop(Player *);
int posCalc(int,int,int);
int troopReset(Troop *);
int Elimination(Troop *);
int calcBlockSpin(int);
void blockCreation(Troop *,int,Board *);
void updateBlockName(int,Board *);
void removeTroopCell(Troop *);
void blockDeletion(int);
int nextBlock(Troop *,int,int);
int movement(Troop *,int,char,char *[]);
int blockMovement(Block *, int,char *[]);
int canTroopToHome(Troop *,int);
void troopToHome(Troop *);
int approachDistance(int,int,int);
int isApproachPassed(Troop *,int,int,int);
void approachPassed(Troop *,int,int);
int isBlockApproachPassed(Block *,int,int);
int isGameOver(Player *);
int displayOptions(Player *,int *,Block *block[],int);
int firstPlayer(char *[]);
int randBot(int ,int *);
void printBoard();
void orderPrint(int,char *[]);
int game(Player *,int,int,Block *[]);
void blockBreak(int,int,Block *[]);
void genUniqueNum(int,int *);


/*
int main(){

    int elapsedRounds=0;
    int playerPosition = 1;
    int max = 0;
    int count =0;
    int rollVal;
    
    char *playerName[4] = {"Red","Green","Yellow","Blue"};
    //2341
    srand(2341);
    boardInit();

    int first = firstPlayer(playerName);
    orderPrint(first,playerName);
    
    while(isGameOver(playerArray)){
        int streak = 0;
        int playerIndex = count%boardPlayers;
        rollVal = roll();
        game(&playerArray[playerIndex],count,rollVal);
        /*
        do{
            streak++;
            if(streak == 3){
                printf("Three concecutive 6's\n");    
                break;
            }
            rollVal = roll();
            game(&playerArray[playerIndex],count,rollVal);
        } while (rollVal == maxRollVal);
        
        count++;
    }

}
*/

//Player
void PlayerInit(Player *player,char *playerName){
    static int count = 0;
    //this can be an issue if restarting the game

    player->name= playerName;
    player->troopsAtPlay=0;
    player->troopsAtHome=0;
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

            if(count > 0){
                //block creation
                if (opPlayer == player){
                    int pos = player->startingLocation;
                    blockCreation(troop,pos,&board[pos]);
                    calcBlockSpin(pos);
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
    troop->position = 0;
    troop->captures = 0;
    troop->approachPassed = 0;
    troop->where = 0;
    troop->rotation = clockwise;
}

int Elimination(Troop *troop){
    if(troop->where == 0){
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
        blockDeletion(pos);
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
    //board[position]
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

void removeTroopCell(Troop *troop){
        int pos = troop->position;
        int count = board[pos].troopCount;

        //if only one troop exists in cell;
        if(count == 1){
            board[pos].troop = NULL;
            board[pos].troopCount = 0;
            return;
        }

        //if more than one troop exist in cell;
        Block *block = board[pos].block;
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
            board[pos].troop = block->troopArr[index];
        }
        board[pos].troopCount--;
        //if removed cell removes the block into a single troop
        if(board[pos].troopCount == 1){
            blockDeletion(pos);
        }else{
            calcBlockSpin(pos);
            updateBlockName(pos,&board[pos]);
        }
}


void blockDeletion(int position){
    if(board[position].block != NULL){
        free(board[position].block);
        board[position].block = NULL;
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
int movement(Troop *troop,int rollVal,char rotation,char *elimName[]){

    //null check is for blocking partial movements from troops moving as a block
    if (!canTroopToHome(troop,rollVal)){
        troopToHome(troop);
        return 6;
    }
    
    if(elimName != NULL){
        int logVal = isApproachPassed(troop,troop->position,rollVal,rotation);
        if(!logVal){
            approachPassed(troop,rollVal,rotation);
            return 5;
        }else if (logVal == 1){
            troop->approachPassed += 1;
        }
        
    }

    if(!(troop->where == 1)){
        return 3;
    }

    int newPos=nextBlock(troop,rollVal,rotation);

    if(newPos == troop->position){
        elimName[1] = board[posCalc(newPos,1,rotation)].block->name;
        return 2;
    }

    int playerCount = board[newPos].troopCount;

    if(playerCount > 0){
        Player *opPlayer = board[newPos].troop->owner;

        if(opPlayer == troop->owner){
            removeTroopCell(troop);
            blockCreation(troop,newPos,&board[newPos]);
            calcBlockSpin(newPos);

            if(elimName != NULL){
                elimName[1] = board[newPos].block->name;
            }
            troop->position=newPos;
            return 0;

        }else if(playerCount == 1){
            removeTroopCell(troop);
            elimName[1] = board[newPos].troop->name;
            Elimination(board[newPos].troop);

            troop->position=newPos;
            troop->captures += 1;
            board[newPos].troopCount =1;
            board[newPos].troop = troop;
            return 4;

        }else{
            elimName[1] = board[newPos].troop->name;
            return 2;
        }
        
    }else if(playerCount == 0){
        removeTroopCell(troop);
        troop->position=newPos;
        board[newPos].troopCount =1;
        board[newPos].troop = troop;
        return 1;
    }
    
}
//return 0- block created or captured
//return 1- player moved
//return 2- player can't move/block infront or end.
//return 3- player is in base
//return 4- player eliminated someone
//return 5- troop moved into the home straight 
//return 6- troop finished and is at home now.

int blockMovement(Block *block, int rollVal,char *logArray[]){
    Troop *troop = block->troopArr[0];
    int oldPos = troop->position;
    int count = board[oldPos].troopCount;
    rollVal = rollVal/count;
    int chk = 0;
    int rotation = block->rotation;

    if(!rollVal){return 2;}
    if(troop->where == 2){

    }

    int logVal = isBlockApproachPassed(block,oldPos,rollVal);
    if(!logVal){
        for (short i = 0; i < count; i++){
            approachPassed(block->troopArr[i],rollVal,rotation);
        }
        return 5;
    }else if (logVal == 1){
        for (short i = 0; i < count; i++){
            block->troopArr[i]->approachPassed++;
        }
    }
    

    int pos = nextBlock(troop,rollVal,block->rotation);

    if(pos == oldPos){
        return 3;
    }
    if(board[pos].troopCount == board[oldPos].troopCount){
        logArray[1] = board[pos].block->name;
        Elimination(board[pos].troop);
        chk = 1;
        //eliminates the whole block
    }

    if(board[pos].troopCount != 0){
        return 4;
    }
    for(int i=0; i<count; i++){
        troop = board[oldPos].troop;  
        movement(troop,rollVal,rotation,NULL);
        if(chk){troop->captures++;};
    }
    return (chk);
}
//return 5 - block moved into the homestraight
//return 4 - Can't move due to opponent before first encountering block
//return 3 - can't move block in front
//return 2 - not enough roll to move block
//return 1 - eliminated;
//return 0 - move sucessful 
int canTroopToHome(Troop *troop,int rollVal){
    if(troop->where == 2 && rollVal == (homeRowLength -1) - troop->position){
        return 0;
    }
    return 1;
}

void troopToHome(Troop *troop){
    int pos = troop->position;
    Player *owner = troop->owner;
    int troopCount = owner->homeRow[pos].troopCount;

    troopReset(troop);
    troop->where = 3;
    owner->troopsAtHome++;
}    

int approachDistance(int approachPos, int oldPos, int rotation){
    int rotTemp = (rotation) ? -1:1;
    return (rotTemp*(approachPos - oldPos) + boardLength)%boardLength;
}

//make sure to use the divised rollval when using for blocks
int isApproachPassed(Troop *troop, int oldPos,int rollVal,int rotation){
    if(troop->where != 1){return 4;}

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

        Elimination(troop);
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
        if((troop->approachPassed - troop->rotation) <0  && troop->captures == 0){
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

//work on the displayOptions
//blocks needed to be checked if a move is possible or not right?
int displayOptions(Player *player,int *optionArray,Block *block[],int rollVal){

    int count = 0;
    int blockCount = 0;
    
    printf("%d. TAKEOUT: Troop\n",count);
    
    for (short i = 0; i < playerTroops; i++){
        //used to check if the next move is possible
        int newPos = nextBlock(&player->troopArr[i],rollVal,player->troopArr[i].rotation);
        //removed newPos == player->troopArr[i].position from check
        if(board[newPos].block != NULL){
            continue;
        }

        //if troop is at base and movable display posibility
        if(player->troopArr[i].where == 1){
            optionArray[i+1]=1;

            //displaying the move (remove for the prod)
            printf("%d. MOVE: Troop %s is at %d\n",
            i+1,player->troopArr[i].name,player->troopArr[i].position);
            count++;
        }
    }
    //troop to board
    if(rollVal==maxRollVal && (player->troopsAtHome+player->troopsAtPlay) < 4){
        optionArray[0] = 1;
        count++;
    }

    //skip turn is only allowed if no moves exist
    if(rollVal != maxRollVal && count == 0){
        optionArray[7]=1;
        count++;
    }
    //finding blocks
    for (short i = 0; i < playerTroops; i++){
        if (player->troopArr[i].where != 1){
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
    //display blocks
    for (short i = 0; i < blockCount; i++){
        optionArray[i+5]=1;
        printf("%d. MOVE: Block %s is at %d\n",
        i+5,block[i]->name,block[i]->troopArr[0]->position);
    }

    return count+blockCount;
    
}
//for printing all a simple function can be implemented


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
    int rollVal = roll();
    
    while (1){
        int optionArray[totalOptions]={0};
        Block *block[maxBlocks]={0};
 
        int optionAmount =displayOptions(&playerArray[playerIndex],optionArray,block,rollVal);;
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
        if(streak == 3 && rollVal ==6){
            printf("Three concecutive 6's\n");
            int count = 0;
            for (short i = 0; i < maxBlocks; i++){
                if(block[i] != NULL){
                    count++;
                }
            }
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

//not unit tested
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


    printf("%s rolls %d\n",currentPlayer->name,rollVal);
     

    //some of the cases do not happen do to the functions above are implemented
    //remove them accordingly
    if(option == 0){
        log = troopToBoard(rollVal,currentPlayer,logArray);
        int playersAtBase = playerTroops - (currentPlayer->troopsAtPlay+currentPlayer->troopsAtHome);
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

        log = movement(troop,rollVal,troop->rotation,logArray);

        int endingPos = troop->position;
        char *direction = (troop->rotation == clockwise) ? "clockwise":"counter clockwise";

        switch (log){
            case 0:
                printf("Color %s piece %s Created a Block %s on L%d\n",
                currentPlayer->name,logArray[0],logArray[1],endingPos);
                break;
            case 1:
                break;
            case 4:
                printf("Color %s piece %s Eliminated %s\n",
                currentPlayer->name,logArray[0],logArray[1]);
                logCode = 1;
                break;
            case 2:
                printf("Color %s piece %s is blocked from moving from L%d to L%d by %s piece\n",
                currentPlayer->name,logArray[0],startingPos,endingPos,logArray[1]);
                //goto invalidOption;
                break;
            case 3:
                printf("Color %s Player %s is in base or homestraight\n",currentPlayer->name,logArray[0]);
                //goto invalidOption;
                break;
        }
        printf("Color %s moves piece %s from location L%d to L%d by %d units in %s Direction. \n",
                currentPlayer->name,logArray[0],startingPos,endingPos,rollVal,direction);

    }else if(option == 5 || option == 6){
        Block *currentBlock = block[option-5];
        Troop *troop = currentBlock->troopArr[0];

        int startingPos = currentBlock->troopArr[0]->position;
        log = blockMovement(currentBlock,rollVal,logArray);
        int endingPos =troop->position;

        logArray[0]=board[endingPos].block->name;
        char *direction=(board[endingPos].block->rotation = clockwise) ? "clockwise":"counter clockwise";

        switch (log){
            case 0:
            
                printf("Color %s moves Block %s from L%d to L%d in %s direction.\n",
                currentPlayer->name,logArray[0],startingPos,endingPos,direction);  
                break;
            case 1:
                printf("Color %s Block %s eliminated Block %s and Moved from L%d to L%d in %s direction.\n",
                currentPlayer->name,logArray[0],logArray[1],startingPos,endingPos,direction);
                logCode = 1;
                break; 
            case 2:
                printf("Not enough roll to move block\n");
                //goto invalidOption;
                break;
            case 3:
                printf("Can't move block in front\n");
                //goto invalidOption;
                break;
            case 4:
                printf("Can't move due to opponent before first encountering block\n");
                //goto invalidOption;
                break;
        }
}else if(option == 7){
    printf("Color %s Turn skipped \n",currentPlayer->name);
}

return logCode;
}
