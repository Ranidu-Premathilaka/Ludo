#include "gameLogic.c"

int main(){

    int elapsedRounds=0;
    int count =0;
    int rollVal;
    
    char *playerName[4] = {"Yellow","Blue","Red","Green"};
    //2341
    srand(2341);
    boardInit();

    int first = firstPlayer(playerName);
    orderPrint(first,playerName);
    
    while(isGameOver(playerArray)){
        count++;
        playerTurn(count%boardPlayers);
        printf("-----------%d-----------\n",count);
        if (count%boardPlayers == 0){
            elapsedRounds++;
            //createMysteryCell(elapsedRounds);
        }     
    }
}
