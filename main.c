#include <time.h>
#include "gameLogic.h"
//1724843095 ubuntu compiled run is an anomalyn
int main(){

    int rollVal;
    
    char *playerName[4] = {"Yellow","Blue","Red","Green"};
    manualPlay(playerName);

    srand(time(NULL));
    boardInit();

    int first = firstPlayer(playerName);
    orderPrint(first,playerName);
    allPlayerInit(playerName);
    int count = first;

    while(isGameOver()){
        int index = count%boardPlayers;
        count++;

        if (index == first){
            createMysteryCell();
        } 

        if(hasPlayerWon(index)){continue;}
        playerTurn(index);
    }

    winnerPrint();
}
