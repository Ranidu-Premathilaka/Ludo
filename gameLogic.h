#ifndef GAMELOGIC_H
#define GAMELOGIC_H
#include "struct.h"
#include <stdio.h>
#include <stdlib.h>

void PlayerInit(Player *,char *);
void boardInit();
int roll();
int coinFlip();
int canTroopToBoard(int rollVal,Player *player,Troop **troopPtr);
int troopToBoard(int rollVal,Player *player,char *logArray[]);
int spin(Troop *troop);
Troop *nextTroop(Player *player);
int posCalc(int currentPos,int move,int direction);
int troopReset(Troop *troop);
int Elimination(Troop *troop);
int lRotation(int position,int approachLocation);
int calcBlockSpin(int position);
void blockCreation(Troop *troop,int position,Board *boardCell);
void updateBlockName(int pos,Board *boardCell);
void removeTroopCell(Troop *troop, Board *boardCell);
void blockDeletion(Board *boardCell);
int nextBlock(Troop *troop,int rollVal,int rotation);
int canMove(Troop *troop,int rollVal,int *newPosPtr);
int canDirectMove(Troop *troop,int newPos);
char *directMove(Troop *troop,int newPos,int log);
int movement(Troop *troop,int rollVal,char *logArray[]);
int canBlockMove(Block *block,int rollVal,int *newPosPtr);
int canDirectMoveBlock(Block *block,int newPos);
char *directBlockMove(Block *block,int newPos, int log);
int blockMovement(Block *block,int rollVal, char *logArray[]);
int hasPlayerWon(int playerIndex);
int canTroopToHome(Troop *troop,int rollVal);
void troopToHome(Troop *troop);
void playerWinPlacement(Player *player);
int approachDistance(int approachPos, int oldPos, int rotation);
int isApproachPassed(Troop *troop, int oldPos,int rollVal,int rotation);
void approachPassed(Troop *troop,int rollVal,int rotation);
int isBlockApproachPassed(Block *block,int oldPos,int rollVal);
int isGameOver();
void winnerPrint();
int realRollVal(void *ptr,int isBlock,int rollVal);
int optionFinder(Player *player,int *optionArray,Block *block[],int rollVal);
int blockFinder(Player *player,Block *block[]);
int firstPlayer(char *playerName[]);
void allPlayerInit(char *playerName[]);
void orderPrint(int first,char *playerName[]);
void playerTurn(int playerIndex);
void blockBreak(int playerIndex,int count,Block *block[]);
void genUniqueNum(int troopCount, int *result);
void printEnd(int playerIndex);

int game(Player *currentPlayer,int rollVal,int option,Block *block[]);

void mysteryLand();
void createMysteryCell();
int mysteryCellOptions(Player *player);
void manageEffect(Player *player);
void mysteryCellEffect(int option,int pos);

int effectOption1(int pos);
void effectOption2(int pos);
int effectOption3(int pos);
void effectOption4(int pos);

int canForceMove(int pos,int endingPos);
int forceMove(int pos,int endingPos,int log);

int randBot(int *optionArray,int restriction);
int redBot(Player *player,int rollVal,int *optionArray,Block *block[]);
int greenBot(Player *player,int rollVal,int *optionArray,Block *block[]);
int yellowBot(Player *player,int rollVal,int *optionArray,Block *block[]);
int blueBot(Player *player,int rollVal,int *optionArray,Block *block[]);
int playerBot(Player *player,int rollVal,int *optionArray,Block *block[]);
void printOptions(Player *player,int *optionArray,Block *block[]);
void manualPlay(char *playerName[]);
#endif