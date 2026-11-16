#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "game_types.h"

void logic_initRound(GameState *g, const char *phrase,
                     const char *hint, const char *category);
void logic_maskPhrase(const char *phrase, char *masked);

int logic_checkLetter(GameState *g, char letter);
int logic_checkPhrase(GameState *g, const char *guess);
int logic_useHint(GameState *g);

int logic_isWon(const GameState *g);
int logic_isLost(const GameState *g);

void logic_finalizeScore(GameState *g);

#endif
