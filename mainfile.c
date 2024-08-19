#include "gameLogic.c"

int main(){

    int elapsedRounds=-1;
    int count =0;
    int rollVal;
    
    char *playerName[4] = {"Yellow","Blue","Red","Green"};
    //2341
    srand(time(NULL));
    boardInit();

    int first = firstPlayer(playerName);
    orderPrint(first,playerName);
    
    while(isGameOver(playerArray)){
        int index = count%boardPlayers;
        count++;

        if (index == 0){
            elapsedRounds++;
            createMysteryCell(elapsedRounds);
        } 

        if(hasPlayerWon(index)){continue;}
    
        playerTurn(index);
        printf("-----------%d-----------\n",count); 
    }
}
