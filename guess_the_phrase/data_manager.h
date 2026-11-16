#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include "game_types.h"

/* start-up */
int data_init(void);

/* categories */
int data_getCategoryCount(void);
int data_getCategoryName(int index, char *outName);

/* phrases */
int data_fetchPhrase(int categoryIndex, char *outPhrase,
 char *outHint, char *outCategory);

/* scores */
int data_saveScore(const char *playerName, int score);
int data_loadPlayerScores(const char *playerName,
 ScoreEntry entries[], int maxEntries);
void data_sortScores(ScoreEntry entries[], int count);

#endif
