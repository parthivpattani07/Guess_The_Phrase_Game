#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui.h"

/* ---------- escape codes ---------- */
#define RESET      "\033[0m"
#define REVERSE    "\033[7m"     /* swaps text and background colour */
#define NO_REVERSE "\033[27m"
#define CLEAR      "\033[2J\033[H"
#define BOX_WIDTH  30

/* ---------- five colour schemes ---------- */
typedef struct {
    const char *bg;      /* background */
    const char *text;    /* main text  */
    const char *accent;  /* heading box and status lines */
    const char *red;
    const char *green;
} Colors;

static const Colors schemes[MAX_PALETTE_COUNT] = {
    /* 0: Hangman on cyan     bg #C9F2F4 text #A8126B red #C1121F green #1B7F3B */
    { "\033[48;2;201;242;244m", "\033[38;2;168;18;107m", "\033[38;2;20;110;125m",
      "\033[38;2;193;18;31m",   "\033[38;2;27;127;59m" },
    /* 1: Sinking ship on beige   bg #F6ECDC text #9C1E7A red #B3261E green #2E7D32 */
    { "\033[48;2;246;236;220m", "\033[38;2;156;30;122m", "\033[38;2;30;110;140m",
      "\033[38;2;179;38;30m",   "\033[38;2;46;125;50m" },
    /* 2: Night purple with bright pink text */
    { "\033[48;2;43;27;77m",    "\033[38;2;255;106;213m", "\033[38;2;76;201;240m",
      "\033[38;2;255;107;107m", "\033[38;2;107;255;149m" },
    /* 3: Deep teal with warm gold text */
    { "\033[48;2;16;54;60m",    "\033[38;2;255;214;102m", "\033[38;2;102;224;207m",
      "\033[38;2;255;120;120m", "\033[38;2;143;230;160m" },
    /* 4: Dark slate with bright lime text */
    { "\033[48;2;35;43;54m",    "\033[38;2;205;255;112m", "\033[38;2;120;190;255m",
      "\033[38;2;255;130;145m", "\033[38;2;120;245;170m" }
};

static const Colors *getColors(int palette)
{
    if (palette < 0) palette = 0;
    return &schemes[palette % MAX_PALETTE_COUNT];
}

/* ---------- small helper: heading in a box ---------- */
static void printHeading(const char *title)
{
    int i;
    int len = (int)strlen(title);
    int left = (BOX_WIDTH - len) / 2;
    int right = BOX_WIDTH - len - left;

    printf("+");
    for (i = 0; i < BOX_WIDTH; i++) printf("=");
    printf("+\n");
    printf("|%*s%s%*s|\n", left, "", title, right, "");
    printf("+");
    for (i = 0; i < BOX_WIDTH; i++) printf("=");
    printf("+\n");
}

/* ================= general ================= */

void ui_clearScreen(void)
{
    printf(RESET CLEAR);
}

void ui_showMessage(const char *text)
{
    printf(" %s\n", text);
}

void ui_showPrompt(const char *text)
{
    printf(" %s ", text);
    fflush(stdout);
}

void ui_showGoodbye(void)
{
    printf(RESET "\n Thanks for playing Guess the Phrase. Goodbye!\n\n");
}

/* ================= menus and help ================= */

void ui_showWelcome(const char *playerName)
{
    printHeading("* Guess the Phrase *");
    printf("\n Player: %s\n\n", playerName);
    printf(" 1. Play\n");
    printf(" 2. Scoreboard\n");
    printf(" 3. Help\n");
    printf(" 4. Quit\n\n");
}

void ui_showHelp(void)
{
    printHeading("* Help *");
    printf("\n How to play\n");
    printf("  - Guess one letter, or type the whole phrase (letters only).\n");
    printf("  - Type ? to get the hint.\n");
    printf("  - You have %d tries.\n\n", MAX_TRIES);
    printf(" Scoring\n");
    printf("  Correct letter ............ +%d per letter\n", POINTS_PER_LETTER);
    printf("  Wrong letter .............. -%d\n", PENALTY_WRONG_LETTER);
    printf("  Correct phrase ............ +%d per hidden letter\n", POINTS_PER_HIDDEN_LETTER);
    printf("  Wrong phrase .............. -%d\n", PENALTY_WRONG_PHRASE);
    printf("  Asking for the hint ....... -%d\n", PENALTY_HINT);
    printf("  Winning bonus ............. +%d\n", BONUS_WIN);
    printf("  No hint bonus ............. +%d\n", BONUS_NO_HINT);
    printf("  Each try left ............. +%d\n\n", BONUS_PER_TRY_LEFT);
}

void ui_showCategoryMenu(char names[][MAX_CATEGORY_LEN], int count)
{
    int i;

    printHeading("* Choose a category *");
    printf("\n");
    for (i = 0; i < count; i++) {
        printf(" %d. %s\n", i + 1, names[i]);
    }
    printf(" %d. Random\n\n", count + 1);
}

void ui_showThemeMenu(void)
{
    printHeading("* Choose a theme *");
    printf("\n");
    printf(" 1. Hangman\n");
    printf(" 2. Sinking ship\n");
    printf(" 3. Dying robot\n\n");
}

/* ================= life pictures ================= */

/* Hangman: 8 pictures (0 to 7 wrong guesses) */
static void drawHangman(int wrong)
{
    switch (wrong) {
    case 0:
        printf("\n\n\n\n\n\n"
               "  =========\n");
        break;
    case 1:
        printf("\n\n\n\n\n"
               "        |\n"
               "  =========\n");
        break;
    case 2:
        printf("\n"
               "        |\n        |\n        |\n        |\n        |\n"
               "  =========\n");
        break;
    case 3:
        printf("    +---+\n"
               "        |\n        |\n        |\n        |\n        |\n"
               "  =========\n");
        break;
    case 4:
        printf("    +---+\n"
               "    |   |\n"
               "    O   |\n"
               "        |\n        |\n        |\n"
               "  =========\n");
        break;
    case 5:
        printf("    +---+\n"
               "    |   |\n"
               "    O   |\n"
               "    |   |\n"
               "        |\n        |\n"
               "  =========\n");
        break;
    case 6:
        printf("    +---+\n"
               "    |   |\n"
               "    O   |\n"
               "   /|\\  |\n"
               "        |\n        |\n"
               "  =========\n");
        break;
    case 7:
        printf("    +---+\n"
               "    |   |\n"
               "    O   |\n"
               "   /|\\  |\n"
               "   / \\  |\n"
               "        |\n"
               "  =========\n");
        break;
    default:
        break;
    }
}

/* Ship: 8 pictures (0 to 7 wrong guesses) */
#define SHIP_SAIL    "      |\\\n      | \\\n      |__\\\n"
#define SHIP_TORN    "      |\\\n      |\n      |__\n"
#define SHIP_DECK    "   ___|___\\___\n"
#define SHIP_HULL    "   \\ o o o o /\n    \\_______/\n"
#define SHIP_WAVES   "  ~-~-~-~-~-~-~-~-\n"

static void drawShip(int wrong)
{
    switch (wrong) {
    case 0:   /* full and floating, flag on top */
        printf("      >\n" SHIP_SAIL SHIP_DECK SHIP_HULL SHIP_WAVES);
        break;
    case 1:   /* flag is gone */
        printf("\n" SHIP_SAIL SHIP_DECK SHIP_HULL SHIP_WAVES);
        break;
    case 2:   /* sail is torn */
        printf("\n" SHIP_TORN SHIP_DECK SHIP_HULL SHIP_WAVES);
        break;
    case 3:   /* water reaches the deck */
        printf("\n" SHIP_TORN SHIP_WAVES SHIP_HULL SHIP_WAVES);
        break;
    case 4:   /* ship tilts */
        printf("\n\n"
               "         /\n"
               "        /\n"
               SHIP_WAVES
               "     \\ o o o o \\\n"
               "      \\_______\\\n"
               SHIP_WAVES);
        break;
    case 5:   /* half under water */
        printf("\n\n\n"
               "      |\n"
               "      |\n"
               SHIP_WAVES
               "    \\_____/\n"
               SHIP_WAVES);
        break;
    case 6:   /* only the mast shows */
        printf("\n\n\n\n"
               "      |\n"
               SHIP_WAVES SHIP_WAVES SHIP_WAVES);
        break;
    case 7:   /* sunk, only waves */
        printf("\n\n\n"
               "  ~-~-o-~-~-0-~-~-o-~\n"
               SHIP_WAVES SHIP_WAVES SHIP_WAVES SHIP_WAVES);
        break;
    default:
        break;
    }
}

/* Robot: 8 pictures (0 to 7 wrong guesses) */
static void drawRobot(int wrong)
{
    switch (wrong) {
    case 0:   /* happy face */
        printf("         *\n"
               "    .---------.\n"
               "    | ^     ^ |\n"
               "    |  \\___/  |\n"
               "    '---------'\n");
        break;
    case 1:   /* smile goes flat */
        printf("         *\n"
               "    .---------.\n"
               "    | ^     ^ |\n"
               "    |  _____  |\n"
               "    '---------'\n");
        break;
    case 2:   /* one eye turns to @ */
        printf("         *\n"
               "    .---------.\n"
               "    | @     ^ |\n"
               "    |  _____  |\n"
               "    '---------'\n");
        break;
    case 3:   /* both eyes @ @ */
        printf("         *\n"
               "    .---------.\n"
               "    | @     @ |\n"
               "    |  _____  |\n"
               "    '---------'\n");
        break;
    case 4:   /* mouth wobbles */
        printf("         *\n"
               "    .---------.\n"
               "    | @     @ |\n"
               "    |  ~~~~~  |\n"
               "    '---------'\n");
        break;
    case 5:   /* antenna droops */
        printf("        \\_\n"
               "    .---------.\n"
               "    | @     @ |\n"
               "    |  ~~~~~  |\n"
               "    '---------'\n");
        break;
    case 6:   /* sparks, face cracks */
        printf("       ~ ~ ~\n"
               "    .----/----.\n"
               "    | @  /  @ |\n"
               "    |  ~~/~~  |\n"
               "    '---------'\n");
        break;
    case 7:   /* dead face X X */
        printf("       ~ ~ ~\n"
               "    .---------.\n"
               "    | X     X |\n"
               "    |  XXXXX  |\n"
               "    '---------'\n");
        break;
    default:
        break;
    }
}

void ui_drawLifeStage(Theme theme, int triesLeft, int maxTries)
{
    int wrong;

    /* Avoid invalid stage numbers if the caller passes unexpected values. */
    if (maxTries <= 0) {
        return;
    }

    wrong = maxTries - triesLeft;
    if (wrong < 0) wrong = 0;
    if (wrong > 7) wrong = 7;

    switch (theme) {
    case THEME_HANGMAN: drawHangman(wrong); break;
    case THEME_SHIP:    drawShip(wrong);    break;
    case THEME_ROBOT:   drawRobot(wrong);   break;
    default:            break;
    }
}

/* ================= the game screen ================= */

int ui_pickRandomPalette(void)
{
    return rand() % MAX_PALETTE_COUNT;   /* no srand here */
}

void ui_showGameBoard(const GameState *g)
{
    const Colors *c = getColors(g->palette);
    int i;

    /* background and text colour, then clear so the whole screen is coloured */
    printf("%s%s" CLEAR, c->bg, c->text);

    printf("%s", c->accent);
    printHeading("* Guess the Phrase *");
    printf("\n Category : %s     Score : %d\n", g->category, g->score);
    printf(" Tries left : %d / %d     Guessed : ", g->triesLeft, g->maxTries);
    for (i = 0; i < g->guessedCount; i++) {
        printf("%c ", g->guessed[i]);
    }
    printf("\n\n%s", c->text);

    ui_drawLifeStage(g->theme, g->triesLeft, g->maxTries);

    printf("\n ");
    for (i = 0; g->masked[i] != '\0'; i++) {
        printf("%c ", g->masked[i]);
    }
    printf("\n\n");

    if (g->hintUsed == 1) {
        printf(" Hint : %s\n", g->hint);
    } else {
        printf(" Type ? for a hint (small cost), or type a letter or the whole phrase\n");
    }

    /* Do not leave the terminal in this palette for the next screen. */
    printf(RESET);
}

/* ================= end screens ================= */

void ui_showWinScreen(const GameState *g)
{
    const Colors *c = getColors(g->palette);

    printf("%s%s" CLEAR, c->bg, c->green);
    printf("\n  " REVERSE " GAME OVER " NO_REVERSE "\n\n");
    printf("  You won\n\n");
    printf("  Score : " REVERSE " %d " NO_REVERSE "\n\n", g->score);
    printf("  * Play again\n");
    printf("  * Quit\n\n");
    printf(RESET);
}

void ui_showLoseScreen(const GameState *g)
{
    const Colors *c = getColors(g->palette);

    printf("%s%s" CLEAR, c->bg, c->red);
    printf("\n  " REVERSE " GAME OVER " NO_REVERSE "\n\n");
    printf("  You lost\n\n");
    printf("  The phrase was: %s\n\n", g->phrase);
    printf("  Score : " REVERSE " %d " NO_REVERSE "\n\n", g->score);
    printf("  * Play again\n");
    printf("  * Quit\n\n");
    printf(RESET);
}

/* ================= scoreboard ================= */

void ui_showScoreboard(const char *playerName,
                       const ScoreEntry entries[], int count)
{
    int i;
    int shown = count;

    if (shown > TOP_SCORES) {
        shown = TOP_SCORES;
    }

    printHeading("* Scoreboard *");
    printf("\n Top scores for %s\n\n", playerName);

    if (count <= 0 || entries == NULL) {
        printf(" No scores yet - play a game to get on the board!\n");
    } else {
        for (i = 0; i < shown; i++) {
            printf(" %d. %d\n", i + 1, entries[i].score);
        }
    }
    printf("\n");
}
