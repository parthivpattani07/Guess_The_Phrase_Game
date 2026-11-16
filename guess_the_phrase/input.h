#ifndef INPUT_H
#define INPUT_H

#include "game_types.h"

/* Low-level input helpers */
void input_flushBuffer(void);
int input_readLine(char *buffer, int size);

/* Input validation */
int input_isValidLetter(char ch);
int input_isValidPhrase(const char *text);

/* Guessed-letter management */
int input_hasAlreadyGuessed(const GameState *g, char letter);
void input_recordLetter(GameState *g, char letter);
void input_clearGuessed(GameState *g);

/* Main input functions */
GuessType input_readGuess(GameState *g, char *outText);
int input_readMenuChoice(int min, int max);
void input_readPlayerName(char *outName);
void input_waitForEnter(void);

#endif
