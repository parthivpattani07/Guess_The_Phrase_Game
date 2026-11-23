#ifndef UI_H
#define UI_H

#include "game_types.h"

/* general */
void ui_clearScreen(void);
void ui_showMessage(const char *text);
void ui_showGoodbye(void);

/* menus and help */
void ui_showWelcome(const char *playerName);
void ui_showHelp(void);
void ui_showCategoryMenu(char names[][MAX_CATEGORY_LEN], int count);
void ui_showThemeMenu(void);

/* the game screen */
int  ui_pickRandomPalette(void);
void ui_showGameBoard(const GameState *g);
void ui_drawLifeStage(Theme theme, int triesLeft, int maxTries);

/* end screens */
void ui_showWinScreen(const GameState *g);
void ui_showLoseScreen(const GameState *g);
void ui_showScoreboard(const char *playerName,
                       const ScoreEntry entries[], int count);

#endif
