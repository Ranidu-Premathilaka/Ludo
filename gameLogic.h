#ifndef GAMELOGIC_H
#define GAMELOGIC_H
#include "struct.h"
#include <stdio.h>
#include <stdlib.h>

void PlayerInit(Player *,char *);
void boardInit();
int roll();
int coinFlip();
int canPieceToBoard(int rollVal,Player *player,Piece **piecePtr);
int pieceToBoard(int rollVal,Player *player,char *logArray[]);
int spin(Piece *piece);
Piece *nextPiece(Player *player);
int posCalc(int currentPos,int move,int direction);
int pieceReset(Piece *piece);
int Elimination(Piece *piece);
int lRotation(int position,int approachLocation);
int calcBlockSpin(int position);
void blockCreation(Piece *piece,int position,Board *boardCell);
void updateBlockName(int pos,Board *boardCell);
void removePieceCell(Piece *piece, Board *boardCell);
void blockDeletion(Board *boardCell);
int nextBlock(Piece *piece,int rollVal,int rotation);
int canMove(Piece *piece,int rollVal,int *newPosPtr);
int canDirectMove(Piece *piece,int newPos);
char *directMove(Piece *piece,int newPos,int log);
int movement(Piece *piece,int rollVal,char *logArray[]);
int canBlockMove(Block *block,int rollVal,int *newPosPtr);
int canDirectMoveBlock(Block *block,int newPos);
char *directBlockMove(Block *block,int newPos, int log);
int blockMovement(Block *block,int rollVal, char *logArray[]);
int hasPlayerWon(int playerIndex);
int canPieceToHome(Piece *piece,int rollVal);
void pieceToHome(Piece *piece);
void playerWinPlacement(Player *player);
int approachDistance(int approachPos, int oldPos, int rotation);
int isApproachPassed(Piece *piece, int oldPos,int rollVal,int rotation);
void approachPassed(Piece *piece,int rollVal,int rotation);
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
void genUniqueNum(int pieceCount, int *result);
void printEnd();

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