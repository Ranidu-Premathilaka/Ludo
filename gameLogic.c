#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "struct.h"

#define counterClockwise 1
#define clockwise 0
#define endWinningSum 6 //1+2+3

//to stop repeatedly typing struct 
//Posibility of Null pointarray dereferencing

Board board[boardLength];
Player playerArray[4];

//functiondef
void PlayerInit(Player*,char*);
void boardInit();
int roll();
int coinFlip();
int troopToBoard(int ,Player *,char **);
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
int movement(Troop *,int,char **);
int blockMovement(Block *, int);
int displayOptions(Player *,int *,Block *block[],int);
int isGameOver(Player *);
int firstPlayer(char *[]);
void updateBlockName(int );
int randBot(int ,int *);
void printBoard();

int main(){

    int elapsedRounds=0;
    int playerPosition = 1;
    int max = 0;

    char *playerName[4] = {"Red","Green","Yellow","Blue"};
    int first = firstPlayer(&playerName[0]);

    //srand(time(NULL));
    srand(1);
    boardInit();

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

    int count =0;
    while(isGameOver(playerArray)){
        Player *currentPlayer = &playerArray[count%boardPlayers];
        int option;
        int log;
        int rollVal;

        int optionArray[totalOptions]={0};
        Block *block[maxBlocks] = {NULL,NULL};

        rollVal = roll();
        printf("%s rolls %d\n",currentPlayer->name,rollVal);

    invalidOption:
        int optionAmount = displayOptions(currentPlayer,optionArray,block,rollVal);

        for (int i = 0; i < totalOptions; i++)
        {
           printf("%d \t",optionArray[i]);
        }
        printf("%d",optionAmount);
        printf("\n");

        printBoard();
        option = randBot(optionAmount,optionArray);

//let functions handle exceptions
        char *logArray[2];
        //0- moved name
        //1- eliminated troop name

        if(option == 0){
            //troop To the board
            //changes moveed troop name to the actual name
            // if block all troop names
            log = troopToBoard(rollVal,currentPlayer,&logArray[0]);
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
            }

            printf("Color %s Moves %s to the starting point. \n",currentPlayer->name,logArray[0]);
            printf("Player now has %d/4 of pieces on board and  %d/4 number of pieces on base \n",
                    currentPlayer->troopsAtPlay,playersAtBase);

        }else if(option > 0 && option <= 4){
            // troop in board movement
            Troop *troop = &currentPlayer->troopArr[option-1];
            logArray[0]=troop->name;
            int startingPos = troop->position;
            log = movement(troop,rollVal,&logArray[1]);
            int endingPos = troop->position;
            char *direction = (troop->rotation == clockwise) ? "clockwise":"counter clockwise";
            switch (log){
                case 0:
                    printf("Block created on L%d\n",endingPos);
                    break;
                case 1:
                    break;
                case 4:
                    printf("Player Eliminated\n");
                    break;
                case 2:
                    printf("Color %s piece %s is blocked from moving from L%d to L%d by %s piece\n",
                    currentPlayer->name,logArray[0],startingPos,endingPos,logArray[1]);
                    //goto invalidOption;
                    break;
                case 3:
                    printf("Player still in base\n");
                    //goto invalidOption;
                    break;
            }
            printf("Color %s moves piece %s from location L%d to L%d by %d units in %s Direction. \n",
                    currentPlayer->name,logArray[0],startingPos,endingPos,rollVal,direction);

        }else if(option == 5 || option == 6){

            char *blockName; 
            char *startingPos; //access troop with player from index
            char *EliminationBlockName;
            log = blockMovement(block[option-5],rollVal);
            char *endingPos; //access troop with player from index
            char *direction; //access troop with player from index
            switch (log){
                case 0:
                    printf("Block Moved\n");
                    break;
                case 1:
                    printf("Not enough roll to move block\n");
                    //goto invalidOption;
                    break;
                case 2:
                    printf("Can't move block in front\n");
                    //goto invalidOption;
                    break;
                case 3:
                    printf("Can't move due to opponent before first encountering block\n");
                    //goto invalidOption;
                    break;
            }
    }else if(option == 7){
        printf("Turn skipped \n");
    }
    printf("%d \n",count);
    count++;
    }
}


//Player
void PlayerInit(Player *player,char *playerName){
    static int count = 0;
    //this can be an issue if restarting the game

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
        player->troopArr[i].name[1] = '1'+i;
        player->troopArr[i].name[2] = '\0';
        player->troopArr[i].position = 0;
        player->troopArr[i].where = 0;
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



//Non playerbased logic
int roll(){
    return (rand()%6 +1);
}

int coinFlip(){
    return (rand()%2);
}
//head - 1
//tail - 0

//Player based Logic


int troopToBoard(int rollVal,Player *player,char **logArray){

    if(rollVal==6){
            Troop *troop = nextTroop(player);
            *logArray = troop->name;
        if (troop != NULL){
            
            int count = board[player->startingLocation].troopCount;
            Player *opPlayer = NULL;
            if (count > 0){
                opPlayer =board[player->startingLocation].troop->owner;
            }
            
            
            if(opPlayer != player && count > 1){
                return 4;
            }
            
            troop->where=1;
            troop->position=player->startingLocation;
            spin(troop);
            (player->troopsAtPlay)++;

            if(count > 0){
                //block creation
                if (opPlayer == player){
                    blockCreation(troop,player->startingLocation);
                    return 0;
                }else{
                //already checked if count == 1;
                    Elimination(board[player->startingLocation].troop);
                    troop->captures += 1;
                    board[player->startingLocation].troopCount =1;
                    board[player->startingLocation].troop = troop;
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
//return 1 - player in approach
//return 2 - All troops in board
//return 3 - Roll not a 6
//return 4 - Blocked by opponent

int spin(Troop *troop){
    if(troop->where){
        troop->rotation = (coinFlip()) ? clockwise:counterClockwise;
    }
}


Troop *nextTroop(Player *player){
    int troopsNotAtBase = (player->troopsAtHome)+(player->troopsAtPlay);
    //invalid name!!!
    /*
    if(troopsNotAtBase != playerTroops){
        return &(player->troopArr[troopsNotAtBase]);
    }
    return NULL;
    */
    for (short i = 0; i <= troopsNotAtBase; i++){
        //implment to check out winners or troops in home already
        if(player->troopArr[i].where == 0){
            return &(player->troopArr[i]);
        }
    }
    

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
    board[troop->position].troopCount -= 1;
    (troop->owner->troopsAtPlay)--;
    troop->position = 0;
    troop->captures = 0;
    troop->where = 0;
    troop->rotation = clockwise;
}

int Elimination(Troop *troop){
    if(troop->where == 0){
        return 0;
    }
    int pos = troop->position;
    if(board[pos].troopCount ==1){
        troopReset(troop);
        board[pos].troop = NULL;

        return 1;
    }else{
        for(short i = 0; i<board[pos].troopCount; i++){
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
void blockCreation(Troop *troop,int position){
    //the count can't be 0;
    int count = board[position].troopCount;
    //debugging
    /*
    if(board[position].troop->owner != troop->owner){
        printf("Error: Troop owner and cell owner mismatch\n");
        exit(1);
    }*/
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
    updateBlockName(position);
}

void updateBlockName(int pos){
    int count = board[pos].troopCount; 
    Block *block = board[pos].block;
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
            updateBlockName(pos);
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
int movement(Troop *troop,int rollVal,char **elimName){
    if(!troop->where){
        return 3;
    }

    int newPos=nextBlock(troop,rollVal,troop->rotation);

    if(newPos == troop->position){
        return 2;
    }

    int playerCount = board[newPos].troopCount;

    if(playerCount > 0){
        Player *opPlayer = board[newPos].troop->owner;

        if(opPlayer == troop->owner){
            removeTroopCell(troop);
            blockCreation(troop,newPos);

            troop->position=newPos;
            return 0;

        }else if(playerCount == 1){
            removeTroopCell(troop);
            *elimName = board[newPos].troop->name;
            Elimination(board[newPos].troop);

            troop->position=newPos;
            troop->captures += 1;
            board[newPos].troopCount =1;
            board[newPos].troop = troop;
            return 4;

        }else{
            *elimName = board[newPos].troop->name;
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

int blockMovement(Block *block, int rollVal){
    Troop *troop = block->troopArr[0];
    int oldPos = troop->position;
    int count = board[oldPos].troopCount;
    rollVal = rollVal/count;
    int chk = 0;

    if(!rollVal){return 1;}

    int pos = nextBlock(troop,rollVal,block->rotation);

    if(pos == oldPos){
        return 2;
    }
    if(board[pos].troopCount == board[oldPos].troopCount){
        Elimination(board[pos].troop);
        chk = 1;
        //eliminates the whole block
    }

    if(board[pos].troopCount != 0){
        return 3;
    }
    for(int i=0; i<count; i++){
        troop = board[oldPos].troop;  
        movement(troop,rollVal,NULL);
        if(chk){troop->captures++;};
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
    int count=0;
    for(short i = 0; i<boardPlayers; i++){
        count += playerArray[i].isWinner;
    }
    return (count == endWinningSum) ? 0:1;
}


    //1.Take troop Out from base
    //2.Move standard board peices(2.1-R1,2.2-R2)
    //3.Move block
int displayOptions(Player *player,int *optionArray,Block *block[],int rollVal){

    int count = 0;
    int blockCount = 0;
    
    printf("%d. TAKEOUT: Troop\n",count);
    
    for (short i = 0; i < playerTroops; i++){
        //used to check if the next move is possible
        int newPos = nextBlock(&player->troopArr[i],rollVal,player->troopArr[i].rotation);
        if(newPos == player->troopArr[i].position || board[newPos].block != NULL){
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
    if(rollVal==6 && count != 4){
        optionArray[0] = 1;
        count++;
    }

    //skip turn is only allowed if no moves exist
    if(rollVal != 6 && (player->troopsAtPlay+player->troopsAtHome) == 0){
        optionArray[7]=1;
        count++;
    }
    //finding blocks
    for (short i = 0; i < playerTroops; i++){
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

int randBot(int optionAmount,int *optionArray){
    /*
    if(optionAmount==1 ){
        //skip turn done separately for efficency
        return 7;
    } */
    //processes when option amount is larger
    //same probability
    int randomVal = rand()%optionAmount;
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
