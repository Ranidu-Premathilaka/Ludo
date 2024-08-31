#include "gameLogic.h"

//GLOBALS (should be in separate file)
Board board[boardLength];
Player playerArray[4];
int globalMysteryCell = -1;
int winnerPlacement = 0;
int elapsedRounds=-1;

typedef int (*Bot)(Player *,int ,int *,Block *[]);
Bot botArray[] = {yellowBot,blueBot,redBot,greenBot};
//GLOBALS

void PlayerInit(Player *player,char *playerName){
    static int count = 0;
    //this can be an issue if restarting the game

    player->name= playerName;
    player->piecesAtPlay=0;
    player->piecesAtHome=0;
    player->piecesAtBase=playerPieces;
    player->index = count;
    player->startingLocation = 13*count + 2; //posCalc(13*count,2,counterClockwise) if more than 4 players hehe;
    player->approachLocation = 13*count;
    player->isWinner = 0;
    count++;
    
    for (short i = 0; i < playerPieces; i++){
        player->pieceArr[i].name[0] = playerName[0];
        player->pieceArr[i].name[1] = '1'+i;
        player->pieceArr[i].name[2] = '\0';
        player->pieceArr[i].position = 0;
        player->pieceArr[i].where = 0;
        player->pieceArr[i].captures = 0;
        player->pieceArr[i].owner = player;
        player->pieceArr[i].rotation = clockwise;
        player->pieceArr[i].approachPassed = 0;
        player->pieceArr[i].index = i;
        player->pieceArr[i].mysteryEffect[0] = 0;
        player->pieceArr[i].mysteryEffect[1] = 0;

    }
    for (short i = 0; i < homeRowLength; i++){
        player->homeRow[i].block = NULL;
        player->homeRow[i].piece = NULL;
        player->homeRow[i].pieceCount = 0;
        
    }
}

//board

void boardInit(){
    for (short i = 0; i <boardLength ; i++){
        board[i].block = NULL;
        board[i].piece = NULL;
        board[i].pieceCount = 0;
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
//return -2 - no pieces in base. (this is a serious error)
//return 4 - piece increments to it's own block
//return 5 - piece eliminates opponent piece
//return 6 - piece moves freely without elimination
//return -7 - piece is blocked by opponent 
int canPieceToBoard(int rollVal,Player *player,Piece **piecePtr){
    if(rollVal != maxRollVal){return -1;}
    if(player->piecesAtBase == 0){return -2;}

    Piece *piece = nextPiece(player);
    if(piecePtr != NULL){*piecePtr = piece;}

    return canDirectMove(piece,player->startingLocation);
}

int pieceToBoard(int rollVal,Player *player,char *logArray[]){
    Piece *piece;
    int log = canPieceToBoard(rollVal,player,&piece);

    switch (log){
    case -1:
        break;

    case -2:
        break;

    case -7:
        logArray[1] = directMove(piece,player->startingLocation,log);

    default:
        logArray[0] = piece->name;

        piece->where=1;
        spin(piece);
        (player->piecesAtPlay)++;
        (player->piecesAtBase)--;

        logArray[1] = directMove(piece,player->startingLocation,log);
        break;
    }

    return log;

}

int spin(Piece *piece){
    if(piece->where == 1){
        piece->rotation = (coinFlip()) ? clockwise:counterClockwise;
    }
}


Piece *nextPiece(Player *player){
    for (short i = 0; i < playerPieces; i++){
        //implment to check out winners or pieces in home already
        if(player->pieceArr[i].where == 0){
            return &(player->pieceArr[i]);
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

int pieceReset(Piece *piece){
    board[piece->position].pieceCount -= 1;
    (piece->owner->piecesAtPlay)--;
    (piece->owner->piecesAtBase)++;
    piece->position = 0;
    piece->captures = 0;
    piece->approachPassed = 0;
    piece->where = 0;
    piece->rotation = clockwise;
    piece->mysteryEffect[0] = 0;
    piece->mysteryEffect[1] = 0;
    
}

//could be optimized to only have a pos parameter
int Elimination(Piece *piece){
    if(piece->where != 1){
        return 0;
    }
    int pos = piece->position;
    int pieceCount = board[pos].pieceCount;

    if(pieceCount ==1){
        pieceReset(piece);
        board[pos].piece = NULL;

        return 1;
    }else{
        for(short i = 0; i<pieceCount; i++){
            pieceReset(board[pos].block->pieceArr[i]);
        }
        blockDeletion(&board[pos]);
        board[pos].piece = NULL;
        return 2;
    }
}
//return 0 - piece not in play
//return 1 - piece removed
//return 2 - Block removed


//can be used to get if the block needs to travel the longest path 
//if position has 1 piece the longest path rotation
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

    int count = board[position].pieceCount;
    int approachLocation = board[position].piece->owner->approachLocation;
    int longestRotation;

    longestRotation = lRotation(position,approachLocation);

    for(short i = 1; i<count; i++){
        Piece *piece1 = board[position].block->pieceArr[i-1];
        Piece *piece2 = board[position].block->pieceArr[i];
        if(piece1->rotation != piece2->rotation){
            return longestRotation;
        }
    }

    return board[position].piece->rotation;
}

//choose to dynammically alocate blocks due to the rarety of it and to save space
void blockCreation(Piece *piece,int position,Board *boardCell){


    int count = boardCell->pieceCount;

    if(count == 1){
        Block *block = (Block *)malloc(sizeof(Block));

        if (block == NULL) {
            printf("Couldn't assign memory to block\n");
            exit(1);
        }

        block->mysteryEffect[0] = 0;
        block->mysteryEffect[1] = 0;

        block->pieceArr[0] = boardCell->piece;    
        boardCell->block = block;
    }

    boardCell->block->pieceArr[count] = piece;
    boardCell->pieceCount++;
    updateBlockName(position,boardCell);
}

void updateBlockName(int pos,Board *boardCell){
    int count = boardCell->pieceCount; 
    Block *block = boardCell->block;
    int totalChar=0;
    for (short i = 0; i < count; i++){
        for (short j = 0; j < 2; j++){
            block->name[totalChar]=block->pieceArr[i]->name[j];
            totalChar++;
        }
        if(i != count-1){
            block->name[totalChar]=',';
            totalChar++;
        }
    }
    block->name[totalChar] = '\0';
}

void removePieceCell(Piece *piece,Board *boardCell){
        int pos = piece->position;
        int count = boardCell->pieceCount;

        //if only one piece exists in cell;
        if(count == 1){
            boardCell->piece = NULL;
            boardCell->pieceCount = 0;
            return;
        }

        //if more than one piece exist in cell;
        Block *block = boardCell->block;
        int index;

        for(short i = 0; i<count; i++){
            if(block->pieceArr[i]==piece){
                index = i;
                break;
            }
        }
        for(short i = index; i<count-1; i++){
            block->pieceArr[i] = block->pieceArr[i+1];
        }
        block->pieceArr[count-1] = NULL;
        if(index == 0){
            boardCell->piece = block->pieceArr[index];
        }
        boardCell->pieceCount--;
        //if removed cell removes the block into a single piece
        if(boardCell->pieceCount == 1){
            blockDeletion(boardCell);
        }else{
            if(piece->where == 1){
                boardCell->block->rotation = calcBlockSpin(pos);
            }
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


int nextBlock(Piece *piece,int rollVal,int rotation){

    for (short i = 1; i <rollVal; i++){
            int nextPos = posCalc(piece->position,i,rotation);
            if(board[nextPos].block != NULL && ((board[nextPos].piece->owner) != (piece->owner))){
                return posCalc(piece->position,i-1,rotation);
            }
        }
    //no block exists in between
    return posCalc(piece->position,rollVal,rotation);
}

//useing negative for unsuccessfull moves

//return -4 - mystery cell blocked
//return -2 - rollVal is 0
//return 1 - can Move piece to home
//return -1 - can't move piece
//return 2 - piece is in the home straight
//return -3 - piece is blocked by block/ no valid moves
//return 4 - piece increments to it's own block
//return 5 - piece eliminates opponent piece
//return 6 - piece moves freely without elimination
//return -7 - piece is blocked by opponent 
int canMove(Piece *piece,int rollVal,int *newPosPtr){

    if(piece->mysteryEffect[0] == 3){return -4;}

    if(!rollVal){return -2;}
    if(!canPieceToHome(piece,rollVal)){return 1;}

    if(piece->where != 1 ){return -1;}

    if(!isApproachPassed(piece,piece->position,rollVal,piece->rotation)){return 2;}

    int newPos=nextBlock(piece,rollVal,piece->rotation);
    if(newPosPtr != NULL){*newPosPtr = newPos;}

    if(newPos == piece->position){return -3;}

    return canDirectMove(piece,newPos);
}

//return 4 - piece increments to it's own block
//return 5 - piece eliminates opponent piece
//return 6 - piece moves freely without elimination
//return -7 - piece is blocked by opponent 
//return -8 - piece is already in position
int canDirectMove(Piece *piece,int newPos){

    if(piece->position == newPos){return -8;}

    if(board[newPos].pieceCount){

        if(board[newPos].piece->owner == piece->owner){return 4;}
        else if(board[newPos].pieceCount == 1){return 5;}
        else{return -7;}

    }else if(board[newPos].pieceCount == 0){return 6;}
}

//return 4 - piece increments to it's own block
//return 5 - piece eliminates opponent piece
//return 6 - piece moves freely without elimination
//return -7 - piece is blocked by opponent 
char *directMove(Piece *piece,int newPos,int log){
    char *ptr;
    switch (log){
    case 4:
        blockCreation(piece,newPos,&board[newPos]);
        board[newPos].block->rotation = calcBlockSpin(newPos);
        piece->position=newPos;
        ptr = board[newPos].block->name;
        break;

    case 5:
        ptr = board[newPos].piece->name;
        Elimination(board[newPos].piece);

        piece->position=newPos;
        piece->captures += 1;
        board[newPos].pieceCount =1;
        board[newPos].piece = piece;
        break;

    case 6:
    //return value is not declared
        piece->position=newPos;
        board[newPos].pieceCount =1;
        board[newPos].piece = piece;
        break;

    case -7:
        ptr = board[newPos].block->name;
        break;
    }

    return ptr;
}

int movement(Piece *piece,int rollVal,char *logArray[]){
    int newPos;
    int log = canMove(piece,rollVal,&newPos);
    switch (log){
    case -4:
        break;
    case -2:
        break;

    case 1:
        pieceToHome(piece);
        break;

    case -1:
        break;

    case 2:
        approachPassed(piece,rollVal,piece->rotation);
        break;

    case -3:
        logArray[1] = board[posCalc(piece->position,1,piece->rotation)].block->name;
        break;

    case -7:
        logArray[1] = directMove(piece,newPos,log);
        break;

    default:
        removePieceCell(piece,&board[piece->position]);
        logArray[1] = directMove(piece,newPos,log);
        break;
    }
    return log;
}


//return -4 - mysteryEffect can't move
//return -1 - not enough roll
//return 1 - can move Block to home
//return 2 - block can move into home straight
//return -3 - Block can't move due to opponent block in front
//return 4 - landed block same player thus creating a bigger block
//return 5 - opponent block but count is the same thus eliminates
//return -6 - opponent block but count is different thus can't move
//return 7 - can move block freely
int canBlockMove(Block *block,int rollVal,int *newPosPtr){
    Piece *piece = block->pieceArr[0];
    int oldPos = piece->position;
    int count = board[oldPos].pieceCount;

    rollVal = rollVal/count;

    if(block->mysteryEffect[0] == 3){return -4;}

    if(!rollVal){return -1;}

    if(!canPieceToHome(piece,rollVal)){return 1;}

    if(!isBlockApproachPassed(block,oldPos,rollVal)){return 2;}

    int newPos = nextBlock(piece,rollVal,block->rotation);
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

    if(block->pieceArr[0]->position == newPos){return -8;}

    if(board[newPos].pieceCount){
        if(board[newPos].piece->owner == block->pieceArr[0]->owner ){return 4;}
        if(board[newPos].pieceCount == board[block->pieceArr[0]->position].pieceCount){return 5;}
        return -6;
    }
    return 7;
}

//implement more readably and maybe implement with candirectmove
char *directBlockMove(Block *block,int newPos, int log){

    char *elimBlockName = board[newPos].block->name;
    if(log == -6 ){return elimBlockName;}
    if (log == 5){Elimination(board[newPos].piece);}

    int oldPos = block->pieceArr[0]->position;

    int mysteryEffectTemp[mysteryLog];
    if(block->mysteryEffect[0]){
        mysteryEffectTemp[0] = block->mysteryEffect[0];
        mysteryEffectTemp[1] = block->mysteryEffect[1];
    }

    if(log != 4){
        if (log == 5){board[oldPos].piece->captures++;}
        Piece *piece = board[oldPos].piece;
        removePieceCell(piece,&board[oldPos]);
        directMove(piece,newPos,6);
    }
    
    int count = board[oldPos].pieceCount;
    for (short i = 0; i < count; i++){
        Piece *piece = board[oldPos].piece;
        removePieceCell(piece,&board[oldPos]);
        directMove(piece,newPos,4);
        if (log == 5){piece->captures++;}
    }

    if(block->mysteryEffect[0]){
        board[newPos].block->mysteryEffect[0] = mysteryEffectTemp[0];
        board[newPos].block->mysteryEffect[1] = mysteryEffectTemp[1];
    }

    if (log == 5){return elimBlockName;}

}

int blockMovement(Block *block,int rollVal, char *logArray[]){
    int newPos;
    int log = canBlockMove(block,rollVal,&newPos);
    int oldPos = block->pieceArr[0]->position;
    int count = board[oldPos].pieceCount;
    
    rollVal = rollVal/count;

        switch (log){
        case -4:
            break;
        case -1:
            break;

        case 1:
            Board *homeRowBoard = &block->pieceArr[0]->owner->homeRow[oldPos];
            for (short i = 0; i < count; i++){
                pieceToHome(homeRowBoard->piece);
            }
            break;

        case 2:
        //for loop needed
            int rotation = block->rotation;
            for (short i = 0; i < count; i++){
                approachPassed(board[oldPos].piece,rollVal,rotation);
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

int canPieceToHome(Piece *piece,int rollVal){
    if(piece->where == 2 && rollVal == (homeRowLength -1) - piece->position){
        return 0;
    }
    return 1;
}

void pieceToHome(Piece *piece){
    int pos = piece->position;
    Player *owner = piece->owner;
    removePieceCell(piece,&owner->homeRow[pos]);
    
    piece->where = 3;
    owner->piecesAtHome++;
    owner->piecesAtPlay--;
    playerWinPlacement(owner);
}

void playerWinPlacement(Player *player){
    if(player->piecesAtHome == 4){
        printf("player %s wins!!!!",player->name);
        player->isWinner = ++winnerPlacement;
    }
}   

int approachDistance(int approachPos, int oldPos, int rotation){
    int rotTemp = (rotation) ? -1:1;
    return (rotTemp*(approachPos - oldPos) + boardLength)%boardLength;
}

//make sure to use the divised rollval when using for blocks
int isApproachPassed(Piece *piece, int oldPos,int rollVal,int rotation){
    int approachPos = piece->owner->approachLocation;

    int oldDist = approachDistance(approachPos,oldPos,rotation);
    rollVal = rollVal - (oldDist+1);
    //int newDist = (rotTemp*(approachPos - newPos) + boardLength)%boardLength;

    //try to use lrotation for computation like longest path rotation changes when passed?
    //break if the rollVal is not enough
    if (rollVal < 0){return 3;}
    //check the cell after the approach 
    int nextBlockPos = nextBlock(piece,oldDist,rotation);
    if((nextBlockPos == posCalc(oldPos,oldDist,rotation)) &&
       (board[nextBlockPos].block == NULL || board[nextBlockPos].piece->owner == piece->owner)){
        //piece->approachPassed += 1;
        if((piece->approachPassed - rotation) >= 0 && piece->captures){
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

void approachPassed(Piece *piece,int rollVal,int rotation){
    rollVal = rollVal -(approachDistance(piece->owner->approachLocation,piece->position,rotation)) -1;

        removePieceCell(piece,&board[piece->position]);
        rollVal = (rollVal > 4) ? 4:rollVal;
        // if the roll is above 4 then the player will be stopped at 4 to play 1 to finish

        Board *homeRow = &piece->owner->homeRow[rollVal];
        piece->where = 2;
        piece->position = rollVal;
        if(homeRow->pieceCount){
            blockCreation(piece,rollVal,homeRow);
        }else{
            homeRow->piece = piece;
            homeRow->pieceCount = 1;
        }
}
//return 1 - sucess
//return 0 - not enough captures or passing of aproaches

//only allowed if all pieces in it have eliminated
int isBlockApproachPassed(Block *block,int oldPos,int rollVal){

    if(isApproachPassed(block->pieceArr[0],oldPos,rollVal,block->rotation) >1){
        return 2;
    }

    int count = board[block->pieceArr[0]->position].pieceCount;
    for (short i = 0; i < count; i++){
        Piece *piece = block->pieceArr[i];
        if((piece->approachPassed - piece->rotation) <0  || piece->captures == 0){
            return 1;
        }
    }   
    return 0;
    
}
//return 0- The move is possible
//return 1- One piece in the block hasn't captured an opponent// 
//return 2- Not clear to pass meaning

int isGameOver(){
    int count=0;
    for(short i = 0; i<boardPlayers; i++){
        count += playerArray[i].isWinner;
    }
    return (count == endWinningSum) ? 0:1;
}

void winnerPrint(){
    int winnerArray[boardPlayers];
    for (short i = 0; i < boardPlayers; i++){
        winnerArray[playerArray[i].isWinner] = i;
    }
    printf("\nPLAYER %s WON!!!!!!! \n",playerArray[winnerArray[1]].name);
    printf("----------------------------------\n");
    printf("Player %s came in 1st Place\n",playerArray[winnerArray[1]].name);
    printf("Player %s is 2nd Place\n",playerArray[winnerArray[2]].name);
    printf("Player %s is 3rd Place\n",playerArray[winnerArray[3]].name);
    printf("Player %s came Last in 4th place\n",playerArray[winnerArray[0]].name);
    printf("----------------------------------\n");
}


    //1.Take piece Out from base
    //2.Move standard board peices(2.1-R1,2.2-R2)
    //3.Move block

int realRollVal(void *ptr,int isBlock,int rollVal){

    int mysteryVal;
    if(isBlock){
        Block *block = (Block *)ptr;
        mysteryVal =block->mysteryEffect[0];
    }else{
        Piece *piece = (Piece *)ptr;
        mysteryVal =piece->mysteryEffect[0];
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

    //find which pieces can make a valid move
    for (short i = 0; i < playerPieces; i++){
        Piece *piece = &player->pieceArr[i];
        int tmpRollVal = realRollVal(piece,0,rollVal);
        int log = canMove(piece,tmpRollVal,NULL);
        if(log > 0){
            optionArray[i+1] = log;
            optionCount++;
        } 
    }

    //piece to board
    if(canPieceToBoard(rollVal,player,NULL) > 0){
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

    for (short i = 0; i < playerPieces; i++){
        if (player->pieceArr[i].where == 0 || player->pieceArr[i].where == 3){
            continue;
        }
        
        int pos = player->pieceArr[i].position;
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

void allPlayerInit(char *playerName[]){
    for (int i = 0; i<boardPlayers; i++){
        PlayerInit(&playerArray[i],playerName[i]);
        printf("The %s player has four (04) pieces named ",playerArray[i].name);
        for(int j = 0; j < playerPieces - 1; j++){
            printf("%s, ",playerArray[i].pieceArr[j].name);
        }
        printf("and %s.\n",playerArray[i].pieceArr[playerPieces-1].name);
    }
}

void orderPrint(int first,char *playerName[]){
    printf("\n%s has the highest roll and will begin the game.\n",playerName[first]);
    printf("The order of a single round is ");
    int range = boardPlayers+first;
    for (short i = first; i <range ; i++){
        int x = i % boardPlayers;
        if(i+1 == range){
            printf("and %s\n",playerName[x]);
        }else{
            printf("%s, ",playerName[x]);
        }
    }
}


void playerTurn(int playerIndex){
    int option;
    int logCode=-1;
    int streak=1;
    Block *block[maxBlocks];

    printf("\n...................................................................\n\n");

    int rollVal = roll();
    printf("%s rolls %d\n",playerArray[playerIndex].name,rollVal);

    while (1){

        int optionArray[totalOptions]={0};
 
        int optionAmount =optionFinder(&playerArray[playerIndex],optionArray,block,rollVal);
        option = botArray[playerIndex](&playerArray[playerIndex],rollVal,optionArray,block);

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
    printEnd(playerIndex);
    manageEffect(&playerArray[playerIndex]);

}

//when a unique num is generated there can be a situation where the block isn't broken
//cause the pattern makes the pieces blocked but the break is actually possible if
//the pieces that move in the other is possible to move
void blockBreak(int playerIndex,int count,Block *block[]){
    int choice = rand()%count;
    printf("Player Choose to break block %s \n",block[choice]->name);

    int pos = block[choice]->pieceArr[0]->position;
    int pieceCount = board[pos].pieceCount;

    //maybe implement direction check too?
    int values[playerPieces]={0};
    genUniqueNum(pieceCount,values);

    //piece count 3
    //if we take that a piece has to move
    //where piece count is 4 this would be imposible to move without creating another block
    //thus let's take 0 as a possible move too 

    for (short i = 0; i < pieceCount; i++){
        Piece *piece = board[pos].piece;
        game(&playerArray[playerIndex],values[i],piece->index+1,NULL);
    }

}

//fix algorithm current one is bad?
void genUniqueNum(int pieceCount, int *result){
    int randomVal = rand();
    if(pieceCount == 2){
        char arrangements[3][2] = {"60","51","42"};
        int index = randomVal%3;
        int srt = randomVal%2;
        for (short i = 0; i < 2; i++){

            result[(srt + i)%2] = (int)arrangements[index][i] - '0';
        }
        

    }else if(pieceCount == 3){
        char arrangements[3][3] = {"510","420","321"};
        int index = randomVal%3;
        int srt = rand()%3;
        for (short i = 0; i < 3; i++){

            result[(srt + i)%3] = (int)arrangements[index][i] - '0';
        }


    }else if(pieceCount == 4){
        int srt = randomVal%4;

        for (short i = 0; i < 4; i++){
            result[(srt + i)%4]=i;
        }
    }
}


void printEnd(int playerIndex){
    Player *player =&playerArray[playerIndex];
    printf("===================================================================\n");
    printf("Location of pieces %s\n",player->name);
    printf("===================================================================\n");
    for(int i = 0; i < playerPieces; i++){
        printf("Piece[%s] ->",player->pieceArr[i].name);
        switch (player->pieceArr[i].where){
        case 0:
            printf("At Base\n");
            break;
        case 1:
            printf("L%d\n",player->pieceArr[i].position);
            break;
        case 2:
            printf("At Home Straight L%d\n",player->pieceArr[i].position);
            break;
        case 3:
            printf("At Home\n");
        }
    }
}


//logCode 0 - no elimination
//logCode 1 - elimination
int game(Player *currentPlayer,int rollVal,int option,Block *block[]){
    int log = 0;
    int logCode = 0;
    int streak = 0;
    int mysteryCheck = 0; //0 - no moves done , 1- pieceMoved, 2-Block moved
    char *logArray[2];  //0- moved name,1- eliminated piece name
    Piece *piece;
    Block *currentBlock;

    //some of the cases do not happen do to the functions above are implemented
    //remove them accordingly
    int endingPos;
    if(option == 0){
        log = pieceToBoard(rollVal,currentPlayer,logArray);
        int playersAtBase = currentPlayer->piecesAtBase;
        endingPos = currentPlayer->startingLocation;
        piece = board[endingPos].piece;

        switch (log){
            case -1:
                printf("Color %s Roll Not a 6.\n",currentPlayer->name);
                break;
            case -2:
                printf("Color %s All pieces are in play.\n",currentPlayer->name);
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
                currentPlayer->piecesAtPlay,playersAtBase);

    }else if(option > 0 && option <= 4){
        // piece in board movement
        piece = &currentPlayer->pieceArr[option-1];
        logArray[0]=piece->name;

        int startingPos = piece->position;
        rollVal = realRollVal(piece,0,rollVal);
        log = movement(piece,rollVal,logArray);
        endingPos = piece->position;

        if(log > 3){
            if(isApproachPassed(piece,startingPos,rollVal,piece->rotation) == 1){
                piece->approachPassed += 1;
                printf("Approach passed Not enough captures or rotations.\n");
            }
        }

        char *direction = (piece->rotation) ? "counter clockwise":"clockwise";

        printf("Color %s piece %s ",currentPlayer->name,logArray[0]);
        switch (log){
            case -4:
                printf("can't move due to mystery cell effect\n");
                break;
            case -2:
                printf("can't move cause the roll is equivelent to 0\n");
                break;
            case 1:
                printf("Moved into home.\n");
                printf("Player has %d/4 pieces at Home.\n",currentPlayer->piecesAtHome);
                break;
            case -1:
                printf("is not in Play.\n");
                //goto option again.
                break;
            case 2:
                printf("entered Home straight in Position: %d\n",piece->position);
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
        piece = currentBlock->pieceArr[0];

        int startingPos = currentBlock->pieceArr[0]->position;

        rollVal = realRollVal(currentBlock,1,rollVal);
        log = blockMovement(currentBlock,rollVal,logArray);
        endingPos =piece->position;

        if (log == 1 || log == 2){
            logArray[0]=currentPlayer->homeRow[endingPos].block->name;
        }else{
            logArray[0]=board[endingPos].block->name;
        }
        

        printf("Color %s Block %s ",currentPlayer->name,logArray[0]);
        switch (log){
            case -4:
                printf("can't move due to mystery cell effect\n");
                break;
            case -1:
                printf("not enough roll to move.\n");
                break;
            case 1:
                printf("Moved into home.\n");
                printf("Player has %d/4 pieces at Home.\n",currentPlayer->piecesAtHome);
                break;
            case 2:
                printf("Entered Home straight and at position %d\n",piece->position);
                 
                break;
            case -3:
                printf("Blocked by %s in front and can't move. \n",logArray[1]);
                //goto option again.
                break;

            case 4:
                printf("Created a bigger Block %s in L%d\n",logArray[0],piece->position);
                break;
            case 5:
                printf("eliminated Block \n");
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
                if(isApproachPassed(piece,startingPos,rollVal,piece->rotation) < 2){
                    int count = board[startingPos].pieceCount;
                    for (short i = 0; i < count; i++){
                        currentBlock->pieceArr[i]->approachPassed += 1;
                    }
                    printf("Approach passed Not enough captures or rotations on One or more pieces in block.\n");
                }

                char *direction=(board[endingPos].block->rotation) ? "counter clockwise":"clockwise";
                printf("Color %s Block %s ",currentPlayer->name,logArray[0]);
                printf("moves from L%d to L%d in %s direction.\n",startingPos,endingPos,direction); 
            }
}else if(option == 7){
    printf("Color %s Turn skipped \n",currentPlayer->name);
}
mysteryLand();

return logCode;
}

void mysteryLand(){  
    if(board[globalMysteryCell].pieceCount){
        char *name = board[globalMysteryCell].piece->owner->name;
        char *piece = (board[globalMysteryCell].pieceCount > 1) ? (board[globalMysteryCell].block->name) : (board[globalMysteryCell].piece->name);
        int endingPos = board[globalMysteryCell].piece->position;
        printf("Color %s landed %s on A mysteryCell!\n",name,piece);
        int choice = mysteryCellOptions(board[globalMysteryCell].piece->owner);
        mysteryCellEffect(choice,endingPos);
    }
}

void createMysteryCell(){
    elapsedRounds++;
    if ((elapsedRounds-roundsBeforeMystery)%roundsMysteryReset == 0){
        int mysteryCell;
        while(1){
            mysteryCell = rand()%boardLength;
            if(!board[mysteryCell].pieceCount && mysteryCell != globalMysteryCell){
                globalMysteryCell = mysteryCell;
                break;
            }
        }
    }
}

int mysteryCellOptions(Player *player){
        return roll();
}

void manageEffect(Player *player){
    for(short i = 0; i < playerPieces; i++){
        if (elapsedRounds - (player->pieceArr[i].mysteryEffect[1]) == effectDuration ){
            player->pieceArr[i].mysteryEffect[0] = 0;
            //player->pieceArr[i].mysteryEffect[1] = 0;
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
//or another piece lands on it and makes a block which the mystery effect is acted on both

    int newPos,log=0;
    int count = board[pos].pieceCount;
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
        newPos = board[pos].piece->owner->startingLocation;
        log = canForceMove(pos,newPos);
        if(log>0){
            printf("Piece%s teleported back to starting Position.\n",multiple);
            forceMove(pos,newPos,log);
        }
        break;

    case 6:
        printf("To Starting Position\n");
        newPos = board[pos].piece->owner->approachLocation;
        log = canForceMove(pos,newPos);
        if(log>0){
            printf("Price%s teleported to player approach.\n",multiple);
            forceMove(pos,newPos,log);
        }
        break;
    }
    if(log < 0){
        printf("Teleportation Not Possible. Initializing Re Teleportation.\n");
    }
    mysteryLand();

}

int effectOption1(int pos){
//bhawana (9th)
    int randEffect = coinFlip() + 1;
    if(board[pos].pieceCount > 1){
        board[pos].block->mysteryEffect[0] = randEffect;
        board[pos].block->mysteryEffect[1] = elapsedRounds;
    }else{
        board[pos].piece->mysteryEffect[0] = randEffect;
        board[pos].piece->mysteryEffect[1] = elapsedRounds;
    }
    return randEffect;
}
void effectOption2(int pos){
//koduwa (27th)
    if(board[pos].pieceCount > 1){
        board[pos].block->mysteryEffect[0] = 3;
        board[pos].block->mysteryEffect[1] = elapsedRounds;
    }else{
        board[pos].piece->mysteryEffect[0] = 3;
        board[pos].piece->mysteryEffect[1] = elapsedRounds;
    }
}
int effectOption3(int pos){
//Pita-kotuwa (46th)
    char *rotation;
    if(board[pos].pieceCount > 1){
        rotation = &board[pos].block->rotation;
    }else{
        rotation = &board[pos].piece->rotation;
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
    Elimination(board[pos].piece);
}

int canForceMove(int pos,int endingPos){
    if(board[pos].pieceCount > 1){
        return canDirectMoveBlock(board[pos].block,endingPos);
    }else{
        return canDirectMove(board[pos].piece,endingPos);
    }
}

int forceMove(int pos,int endingPos,int log){
//to starting position or approach 
    if(board[pos].pieceCount > 1){
        directBlockMove(board[pos].block,endingPos,log);
    }else{
        Piece *piece = board[pos].piece;
        removePieceCell(piece,&board[pos]);
        directMove(piece,endingPos,log);
    }
    return log;
}

int randBot(int *optionArray,int restriction){
    int randVal = rand()%totalOptions;
    int range = randVal + totalOptions;
    int option = -1;
    for (short i = randVal; i < range; i++){
        int x = i%totalOptions;
        if(option == -1 && optionArray[x]){option = x;}
        if(optionArray[x] == restriction || optionArray[x]==0){continue;}
        return x;
    }
    return option;
}

int redBot(Player *player,int rollVal,int *optionArray,Block *block[]){
    int option = 0;
    int minApproachDst = 100;
    
    //closest to eliminate
    for(short i = 1; i < 7; i++){
        if(optionArray[i] == 5){
            Piece *piece;
            int pos,rotation,tempRollVal;
            if(i>4){
                piece = block[i-5]->pieceArr[0];
                rotation = block[i-5]->rotation;
                tempRollVal = realRollVal(block[i-5],1,rollVal)/board[piece->position].pieceCount;
            }else{
                piece = &player->pieceArr[i-1];
                rotation = player->pieceArr[i-1].rotation;
                tempRollVal = realRollVal(&player->pieceArr[i-1],0,rollVal);
            }
            
            piece = board[nextBlock(piece,tempRollVal,rotation)].piece;
            rotation = (board[piece->position].pieceCount > 1) ? board[piece->position].block->rotation:piece->rotation;
            int approachDst = approachDistance(piece->owner->approachLocation,piece->position,rotation);

            if(minApproachDst > approachDst){
                option = i;
                minApproachDst = approachDst;
            }
        }

    }

    if(option){return option;}

    if(optionArray[0]){return 0;}

    return randBot(optionArray,4);
}


int greenBot(Player *player,int rollVal,int *optionArray,Block *block[]){
    char captureCount = 0;
    int option;
    for(short i = 1; i<5; i++){
        //piece make block
        if(optionArray[i] == 4){return i;}

        //piece to capture
        if(optionArray[i] == 5){
            if(!captureCount){
                option = i;
                captureCount++;
            }
            if(!player->pieceArr[i-1].captures){option = i;}
        }
    }
    if(optionArray[0]){return 0;}
    if(captureCount){return option;}

    for(short i = 0; i < maxBlocks; i++){
        if(optionArray[i+5]){return i+5;}
    }

    return randBot(optionArray,-1);

}

int yellowBot(Player *player,int rollVal,int *optionArray,Block *block[]){

    if(optionArray[0]){return 0;}

    int captureOption;
    int closestOption;
    char captureCount = 0;
    char closestCount = 0;
    int minApproachDist = 100;


    for (short i = 1; i < 5; i++){
        //best possible capture peice
        if(optionArray[i] == 5 && (!captureCount || !player->pieceArr[i-1].captures)){
            captureOption = i;
            captureCount++;
        }
        
        //can move into the homestaight
        if(optionArray[i] == 1 || optionArray[i] == 2){return i;}

        //move closest to the approach
        if(optionArray[i]){
            closestCount++;
            int tempDist = approachDistance(player->approachLocation,player->pieceArr[i-1].position,player->pieceArr[i-1].rotation);
            if(minApproachDist > tempDist){
                minApproachDist = tempDist;
                closestOption = i;
            }
        }
        
    }
    
    for(short i=0;i<maxBlocks; i++ ){
        //closest blocks are prioritized
        if(optionArray[i+5]){
            closestCount++;
            int tempDist = approachDistance(player->approachLocation,block[i]->pieceArr[0]->position,block[i]->rotation);
            if(minApproachDist > tempDist){
                minApproachDist = tempDist;
                closestOption = i+5;
            }
        }
        //capture blocks are prioritized
        if(optionArray[i+5] == 5){captureOption = i+5;}
    }

    if(captureCount){return captureOption;}
    if(closestCount){return closestOption;}

    randBot(optionArray,-1);
}

int blueBot(Player *player,int rollVal,int *optionArray,Block *block[]){
    static int prevIndex = 0;
    int range = prevIndex + totalOptions;
    for (short i = prevIndex+1; i <= range; i++){
        int x = i%totalOptions;
        if(optionArray[x]){
            if(x >0 && x<7){prevIndex = x;}
            return x;
        }
    }
}

int playerBot(Player *player,int rollVal,int *optionArray,Block *block[]){

    printOptions(player,optionArray,block);
    int option;
    while(1){
        scanf("%d",&option);
        if(option>=0 && option < 8 && optionArray[option]){
            return option;
        }
        printf("Invalid Option! Enter Again. \n");
        while (getchar() != '\n');
    }

}

void printOptions(Player *player,int *optionArray,Block *block[]){
    if(optionArray[0]){printf("\033[0;32m");}
    else{printf("\033[0;31m");}
    printf("0.Piece To Board\n");
    printf("\033[0m");

    for(short i = 1; i < 5; i++){
        if(optionArray[i]){printf("\033[0;32m");}
        else{printf("\033[0;31m");}
        printf("%d.Move %s\t",i,player->pieceArr[i-1].name);
        printf("\033[0m");
    }
    printf("\n");

    for(short i = 5; i < 7; i++){
        if(optionArray[i]){
            printf("\033[0;32m");
            printf("%d.Move %s\t",i,block[i-5]->name);
            printf("\033[0m");
        }
    }
    printf("\n");

    if(optionArray[7]){printf("\033[0;32m");}
    else{printf("\033[0;31m");}
    printf("7.Skip Turn\n");
    printf("\033[0m");

    printf("\n");
}

void manualPlay(char *playerName[]){
    printf("Do you want to play manually(y/n):");
    char option;
    while(1){
        scanf("%c",&option);
        if(option == 'y'){
            break;
        }else if(option == 'n'){
            printf("Bots will play the game\n");
            return;
        }
        printf("Invalid input Re enter:");
        while (getchar() != '\n');
    }

    printf("Pick Which color to play!\n");
    for(short i = 0; i < boardPlayers; i++){        
        printf("%d-%s\n",i+1,playerName[i]);
    }
    int colourOption;
    while(1){
        scanf("%d",&colourOption);
        if(colourOption>0 && colourOption <5){
            break;
        }
        printf("Invalid input Re enter:");
        while (getchar() != '\n');
    }

    botArray[colourOption-1] = playerBot;

}