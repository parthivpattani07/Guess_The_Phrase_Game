#ifndef GAME_TYPES_H
#define GAME_TYPES_H

/* --------- sizes ----------- */
#define MAX_PHRASE_LEN      100
#define MAX_HINT_LEN        100
#define MAX_CATEGORY_LEN    30
#define MAX_CATEGORIES      10
#define MAX_NAME_LEN        16      /* 15 letters + the end marker */
#define MAX_GUESSED         26      /* one slot for each letter a-z */
#define MAX_SCORES          200
#define TOP_SCORES          5
#define MAX_PALETTE_COUNT   5

/* --------- game rules (the scoring table) ----------- */
#define MAX_TRIES                   7
#define POINTS_PER_LETTER           10
#define PENALTY_WRONG_LETTER        5
#define POINTS_PER_HIDDEN_LETTER    15
#define PENALTY_WRONG_PHRASE        15
#define PENALTY_HINT                20
#define BONUS_WIN                   50
#define BONUS_NO_HINT               50
#define BONUS_PER_TRY_LEFT          10

/* --------- files ----------- */
#define CATEGORIES_FILE     "data/categories.txt"
#define SCORES_FILE         "data/scores.txt"
#define CATEGORY_FOLDER     "data/"

/* --------- named choices ----------- */
typedef enum { THEME_HANGMAN = 1, THEME_SHIP = 2, THEME_ROBOT = 3 } Theme;
typedef enum { GUESS_LETTER = 1, GUESS_PHRASE = 2, GUESS_HINT = 3 } GuessType;

/* --------- one line of the scoreboard ----------- */
typedef struct {
    char name[MAX_NAME_LEN];
    int score;
} ScoreEntry;

/* --------- everything about the round being played ----------- */
typedef struct {
    char phrase[MAX_PHRASE_LEN];         /* the secret phrase                */
    char hint[MAX_HINT_LEN];             /* its hint                         */
    char category[MAX_CATEGORY_LEN];     /* e.g. sports                      */
    char masked[MAX_PHRASE_LEN];         /* phrase with underscores          */
    int  triesLeft;                      /* tries remaining                  */
    int  maxTries;                       /* always MAX_TRIES                 */
    char guessed[MAX_GUESSED];           /* letters already tried            */
    int  guessedCount;                   /* how many are stored              */
    int  score;                          /* score of this round              */
    int  hintUsed;                       /* 0 = no, 1 = yes                  */
    Theme theme;                         /* hangman / ship / robot           */
    int  palette;                        /* colour scheme 0..4               */
    char playerName[MAX_NAME_LEN];       /* who is playing                   */
} GameState;

#endif