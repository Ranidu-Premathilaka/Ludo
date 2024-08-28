#include "gameLogic.c"
//doesn't need struct.h
int main(){

    int rollVal;
    
    char *playerName[4] = {"Yellow","Blue","Red","Green"};

    srand(1724843095);
    boardInit();

    int first = firstPlayer(playerName);
    orderPrint(first,playerName);
    allPlayerInit(playerName);
    int count = first;

    while(isGameOver(playerArray)){
        int index = count%boardPlayers;
        count++;

        if (index == first){
            elapsedRounds++;
            createMysteryCell();
        } 

        if(hasPlayerWon(index)){continue;}
        playerTurn(index);
    }

    winnerPrint();
}
