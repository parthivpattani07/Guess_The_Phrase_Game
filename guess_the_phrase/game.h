#ifndef GAME_H
#define GAME_H


#include "game_types.h"
/*shows on screen before start*/
void game_runMenu(GameState *g);
void game_showScoreboard(const GameState *g);
void game_showHelp();

/*choose before round */
int game_chooseCategory();
Theme game_chooseTheme();

/* in single round */
int game_playRound(GameState *g);
int game_endRound(GameState *g);

#endif
