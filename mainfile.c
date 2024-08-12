#include "gameLogic.c"

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
        playerTurn(count%boardPlayers);
        count++;
    }

}
