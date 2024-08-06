#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "struct.h"

#define counterClockwise 1
#define clockwise 0
#define endWinningSum 6 //1+2+3

//to stop repeatedly typing struct 
//Posibility of Null pointarraer dereferencing

Board board[boardLength];


// in main ? int roundsElapsed = 0;


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
int posCalc(int,int,int);
int troopReset(Troop *);
int Elimination(Troop *);
int calcBlockSpin(int);
void blockCreation(Troop *,int);
void removeTroopCell(Troop *);
void blockDeletion(int);
int nextBlock(Troop *,int,int);
int movement(Troop *,int);
int blockMovement(Block *, int);

int displayOptions(Player *);
int isGameOver(Player *);



//Player
void PlayerInit(Player *player,char *playerName){
    static int count = 0;

    player->name= playerName;
    player->troopsAtPlay=0;
    player->troopsAtHome=0;
    player->index = count;
    player->startingLocation = 13*count;
    player->approachLocation = posCalc(13*count,2,1);
    player->isWinner = 0;
    count++;
    
    for (short i = 0; i < playerTroops; i++){
        player->troopArr[i].name[0] = playerName[0];
        player->troopArr[i].name[1] = '0'+i;
        player->troopArr[i].name[2] = '\0';
        player->troopArr[i].position = 0;
        player->troopArr[i].notAtBase = 0;
        player->troopArr[i].captures = 0;
        player->troopArr[i].owner = player;
        player->troopArr[i].rotation = clockwise;

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



void playerCount(int cellNum, int *count,int *color){
    if(board[cellNum].troopCount){

    }
    *count = 0;
    *color = 0;
    
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
            if(board[player->approachLocation].troopCount > 0){
                //block creation
                blockCreation(troop,player->approachLocation);

                return 0;
            }else{
                board[player->approachLocation].troop = troop;
                board[player->approachLocation].troopCount = 1;

                return 1;
            }
        }
        return 2;
    }
    return 3;
}
//return 0 - Block Created
//return 1 - player in approach
//return 2 - All troops in board
//return 3 - Roll not a 6

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

int posCalc(int currentPos,int move,int direction){
    //if move is used to give direction too the return code can be simplified

    if(direction){
        //counterclockwise
        /*
        if((currentPos-move) < 0){
            return (currentPos-move) + boardLength;
        }else{
            return currentPos-move;
        }*/
        return (currentPos-move+boardLength)%boardLength;       
        
    }else{
        //clockwise
        /*
        if((currentPos+move)>=(boardLength)){
            return (currentPos+move) - boardLength;
        }else{
            return currentPos+move;
        }
        */
        return (currentPos+move)%boardLength;
    }
}

int troopReset(Troop *troop){
    troop->position = 0;
    troop->notAtBase = 0;
    troop->rotation = clockwise;
    board[troop->position].troopCount -= 1;
    (troop->owner->troopsAtPlay)--;
}

int Elimination(Troop *troop){
    if(troop->notAtBase == 0){
        return 0;
    }
    if(board[troop->position].troopCount ==1){
        troopReset(troop);
        board[troop->position].troop = NULL;

        return 1;
    }else{
        for(short i = 0; i<board[troop->position].troopCount; i++){
            troopReset(board[troop->position].block->troopArr[i]);
        }
        blockDeletion(troop->position);
        board[troop->position].troop = NULL;
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
void blockCreation(Troop *troop,int position){
    //the count can't be 0;
    int count = board[position].troopCount;
    if(count == 0 || count == 4){
        printf("Error: Full Block already exists or no troops in position\n");
        exit(1);
    }

    if(count == 1){
        Block *block = (Block *)malloc(sizeof(Block));
        if (block == NULL) {exit(1);}
        block->troopArr[0] = board[position].troop;    
        board[position].block = block;
    }

    board[position].block->troopArr[count] = troop;
    board[position].troopCount++;
    calcBlockSpin(position);
}


void removeTroopCell(Troop *troop){
        int pos = troop->position;
        int count = board[pos].troopCount;

        //if only one troop exists in cell;
        if(count == 1){
            board[pos].troop = NULL;
            return;
        }

        //if more than one troop exist in cell;
        Block *block = board[pos].block;
        int index;

        for(short i = 0; i<count; i++){
            if(block->troopArr[i]==troop){
                index = i;
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
int movement(Troop *troop,int rollVal){
    removeTroopCell(troop);
    int newPos=nextBlock(troop,rollVal,troop->rotation);

    if(newPos == troop->position){
        return 2;
    }

    int playerCount = board[newPos].troopCount;

    if(playerCount == 1){
        Player *opPlayer = board[newPos].troop->owner;

        if(opPlayer == troop->owner){
            blockCreation(troop,newPos);
        }else{
            Elimination(board[newPos].troop);
            troop->position=newPos;
            troop->captures += 1;
            board[newPos].troopCount =1;
            board[newPos].troop = troop;
            
        }
        return 0;
    }else if(playerCount == 0){
        troop->position=newPos;
        board[newPos].troopCount =1;
        board[newPos].troop = troop;
        return 1;
    }
    return 2;
}
//return 0- block created or captured
//return 1- player moved
//return 2- player can't move/block infront or end.

int blockMovement(Block *block, int rollVal){
    Troop *troop = block->troopArr[0];
    int count = board[troop->position].troopCount;
    rollVal = rollVal/count;
    int chk = 0;

    if(!rollVal){return 1;}

    int pos = nextBlock(troop,rollVal,block->rotation);

    if(pos == troop->position){
        return 2;
    }
    if(board[pos].troopCount == board[troop->position].troopCount){
        Elimination(board[pos].troop);
        chk = 1;
        //eliminates the whole block
    }

    if(board[pos].troopCount != 0){
        return 3;
    }
    for(int i=0; i<count; i++){
        removeTroopCell(block->troopArr[i]);
        movement(block->troopArr[i],rollVal);
        if(chk){block->troopArr[i]->captures++;};
    }
    return 0;
}
//return 3 - Can't move due to opponent before first encountering block
//return 2 - can't move block in front
//return 1 - not enough roll to move block
//return 0 - move sucessful 

int approachPassed(Troop *troop,int rollVal,int isBlock){

    int approachPos = troop->owner->approachLocation;
    int troopPos = troop->position;
    int newPos;
    int longestRotat = lRotation(troopPos,approachPos);
    if(isBlock){
        newPos = posCalc(troopPos,rollVal,board[troopPos].block->rotation);
    }else{
        newPos = posCalc(troopPos,rollVal,troop->rotation);
    }
    int newLongestRotat = lRotation(newPos, approachPos);
    if(longestRotat != newLongestRotat){
        //Approach passed
        Elimination(troop);

        


    }
    return 0;

}
//return 0- Approach was not passed
//return 1- 
int isGameOver(Player *playerArray){
    int count;
    for(short i = 0; i<boardPlayers; i++){
        count = playerArray[i].isWinner;
    }
    return (count == endWinningSum) ? 0:1;
}

int main(){

    int elapsedRounds=0;
    int playerPosition = 1;
    int firstPlayer;
    int max = 0;

    srand(time(NULL));
    boardInit();

    Player playerArray[4];
    char *playerName[4]     = {"Red","Green","Yellow","Blue"};

    for (short i = 0; i < boardPlayers; i++){
        int rollVal = roll();
        if(rollVal>max){
            max = rollVal;
            firstPlayer = i;
        }
        printf("%s rolls %d \n",playerName[i],rollVal);
    }

    printf("%s has the highest roll and will begin the game.\n",playerName[firstPlayer]);
    printf("The order of a single round is ");

    int range = boardPlayers+firstPlayer;
    for (short i = firstPlayer; i <range ; i++){
        int x = i % boardPlayers;
        PlayerInit(&playerArray[x],playerName[x]);

        if(i+1 == range){
            printf("and %s\n",playerArray[x].name);
        }else{
            printf("%s, ",playerArray[x].name);
        }
    }

    int count =0;
    while(isGameOver(playerArray)){
        Player *currentPlayer = &playerArray[count%boardPlayers];
        int option;
        int optionAmount;
        int log;
        int rollVal;

    reRoll:
        rollVal = roll();

/*
Action After the Roll of Dice : Many actions can happen when the pieces are moved,
as shown below.
• “Base” to the starting point “X”.   -- 0
• piece on the standard path can move cells specified by the dice value.
• piece may be blocked.
• piece might capture another piece.
• piece may land on a mystery cell and teleport to another cell.
*/
        optionAmount = displayOptions(currentPlayer);
        //placeholder
        option = 1;


        if(option == 0){
            log = troopToBoard(rollVal,currentPlayer);
            switch (log){
                case 0:
                    printf("Block Created\n");
                    break;
                case 1:
                    printf("Player in approach\n");
                    break;
                case 2:
                    printf("All troops are in play\n");
                    goto reRoll;
                    break;
                case 3:
                    printf("Roll not a 6 \n");
                    goto reRoll;
                    break;
            }
        }else if(option > 0 && option <= 4){
            log = movement(nextTroop(currentPlayer),rollVal);
            switch (log){
                case 0:
                    printf("Block Created or Captured\n");
                    break;
                case 1:
                    printf("Player Moved\n");
                    break;
                case 2:
                    printf("Player can't move/block infront or end\n");
                    goto reRoll;
                    break;
            }
        }else if(option == 5 || option == 6){

            // implement later
            log = blockMovement(board[currentPlayer->troopArr[0].position].block,rollVal);
            switch (log){
                case 0:
                    printf("Block Moved\n");
                    break;
                case 1:
                    printf("Not enough roll to move block\n");
                    goto reRoll;
                    break;
                case 2:
                    printf("Can't move block in front\n");
                    goto reRoll;
                    break;
                case 3:
                    printf("Can't move due to opponent before first encountering block\n");
                    goto reRoll;
                    break;
            }
    }
    count++;
}

int displayOptions(Player *player){
    //1.Take troop Out from base
    //2.Move standard board peices(2.1-R1,2.2-R2)
    //3.Move block
    int count=1;
    int posArray[playerTroops];
    
    for (short i = 0; i < playerTroops; i++){
        if(player->troopArr[i].notAtBase == 1){
            printf("%d. MOVE: Troop %s is at %d\n",count,player->troopArr[i].name,player->troopArr[i].position);
            count++;
        }
    }
    //display block 
    for (short i = 0; i < playerTroops; i++){
        posArray[i] = (player->troopArr[i].position);
    }
    for (short i = 0; i < playerTroops; i++){

    }

    
    printf("%d. TAKEOUT: Troop\n",count);
    
    
}
