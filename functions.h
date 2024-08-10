#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include "struct.h"

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
int movement(Troop *,int,char,char *[]);
int blockMovement(Block *, int,char *[]);
int displayOptions(Player *,int *,Block *block[],int);
int isGameOver(Player *);
int firstPlayer(char *[]);
void updateBlockName(int );
int randBot(int ,int *);
void printBoard();
void orderPrint(int,char *[]);
void game(Player *,int,int);
int approachPassed(Troop *,int,int);

#endif