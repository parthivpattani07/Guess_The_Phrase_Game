#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include "ui.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/ioctl.h>
#endif


/* ---------- escape codes ---------- */
#define RESET      "\033[0m"
#define BOLD_OFF   "\033[22m"   /* bold off, keeps colours */
#define BOLD       "\033[1m"
#define REVERSE    "\033[7m"     /* swaps text and background colour */
#define NO_REVERSE "\033[27m"
#define CLEAR      "\033[2J\033[H"
#define BOX_WIDTH  46
#define BLOCK_WIDTH 60           /* width of the widest screen layout */

/* =====================================================================
 *  Centring + scaling output layer
 *  Everything below uses cprintf() instead of printf(). It:
 *    - adds left padding so the block sits in the middle of the terminal
 *    - optionally draws characters g_sx times wide and lines g_sy times tall
 * ===================================================================== */
static int  g_sx = 1;            /* horizontal scale (1 = normal, 2 = double) */
static int  g_sy = 1;            /* vertical scale */
static int  g_mid = 0;           /* 1 = we are in the middle of a line */
static char lineBuf[2048];
static int  lineLen = 0;

#ifdef _WIN32
/* Turn on colour / escape-code support in the Windows console (once). */
static void enableVT(void)
{
    static int done = 0;
    HANDLE h;
    DWORD mode = 0;

    if (done) return;
    done = 1;
    h = GetStdHandle(STD_OUTPUT_HANDLE);
    if (GetConsoleMode(h, &mode)) {
        SetConsoleMode(h, mode | 0x0004);   /* ENABLE_VIRTUAL_TERMINAL_PROCESSING */
    }
}

static int termCols(void)
{
    CONSOLE_SCREEN_BUFFER_INFO info;
    enableVT();
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info))
        return info.srWindow.Right - info.srWindow.Left + 1;
    return 80;
}

static int termRows(void)
{
    CONSOLE_SCREEN_BUFFER_INFO info;
    enableVT();
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info))
        return info.srWindow.Bottom - info.srWindow.Top + 1;
    return 24;
}
#else
static int termCols(void)
{
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0 && w.ws_col > 0) return w.ws_col;
    return 80;
}

static int termRows(void)
{
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0 && w.ws_row > 0) return w.ws_row;
    return 24;
}
#endif

static void printPad(void)
{
    int n = (termCols() - BLOCK_WIDTH) / 2;
    while (n-- > 0) putchar(' ');
}

static void vcenter(int rows);
static void beginScreen(int rows)
{
    fputs("\033[0m\033[2J\033[H", stdout);
    vcenter(rows);
}

/* push blank lines so content of 'rows' lines is vertically centred */
static void vcenter(int rows)
{
    /* keep 3 spare lines for the prompt and the Enter key so the screen never scrolls */
    int n = (termRows() - rows - 3) / 2;
    g_mid = 0;
    while (n-- > 0) putchar('\n');
}

static void flushLine(int newline)
{
    int i, k, r;
    int rep = newline ? g_sy : 1;

    for (r = 0; r < rep; r++) {
        if (!g_mid) printPad();
        for (i = 0; i < lineLen; i++) {
            if (lineBuf[i] == '\033') {          /* copy colour codes untouched */
                while (i < lineLen) {
                    putchar(lineBuf[i]);
                    if (isalpha((unsigned char)lineBuf[i])) break;
                    i++;
                }
            } else {
                for (k = 0; k < g_sx; k++) putchar(lineBuf[i]);
            }
        }
        if (newline) putchar('\n');
        g_mid = 0;
    }
    if (!newline) g_mid = 1;
    lineLen = 0;
}

static void cprintf(const char *fmt, ...)
{
    char buf[2048];
    va_list ap;
    int i;

    va_start(ap, fmt);
    vsnprintf(buf, sizeof buf, fmt, ap);
    va_end(ap);

    for (i = 0; buf[i] != '\0'; i++) {
        if (buf[i] == '\n') {
            flushLine(1);
        } else if (lineLen < (int)sizeof lineBuf - 1) {
            lineBuf[lineLen++] = buf[i];
        }
    }
}

static void flushPartial(void)
{
    if (lineLen > 0) flushLine(0);
    g_mid = 0;          /* the user's Enter moves to a new line, so pad again */
    fflush(stdout);
}

/* From here on printf means cprintf (centred output). */
#define printf cprintf

/* ---------- five colour schemes ---------- */
typedef struct {
    const char *bg;      /* background */
    const char *text;    /* main text  */
    const char *accent;  /* heading box and status lines */
    const char *red;
    const char *green;
} Colors;

static const Colors schemes[PALLATE_COUNT] = {
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
    return &schemes[palette % PALLATE_COUNT];
}

/* ---------- small helper: big heading in a box ---------- */
/* Letters are spaced out and bold so the title looks larger. */
static void printHeading(const char *title)
{
    char big[128];
    int i, j = 0;
    int len;
    int left, right;

    for (i = 0; title[i] != '\0' && j < (int)sizeof big - 3; i++) {
        big[j++] = title[i];
        if (title[i + 1] != '\0') big[j++] = ' ';
    }
    big[j] = '\0';
    len = j;

    left = (BOX_WIDTH - len) / 2;
    right = BOX_WIDTH - len - left;
    if (left < 0) { left = 0; right = 0; }

    printf(BOLD "+");
    for (i = 0; i < BOX_WIDTH; i++) printf("=");
    printf("+\n");
    printf("|%*s%*s|\n", BOX_WIDTH, "", 0, "");              /* roomy top */
    printf("|%*s%s%*s|\n", left, "", big, right, "");
    printf("|%*s%*s|\n", BOX_WIDTH, "", 0, "");              /* roomy bottom */
    printf("+");
    for (i = 0; i < BOX_WIDTH; i++) printf("=");
    printf("+\n" BOLD_OFF);
}

/* ================= general ================= */

void ui_init(void)
{
    /* Ask xterm-style terminals for a roomier window (others ignore this). */
    printf("");
    fputs("\033[8;40;110t", stdout);
    fflush(stdout);
}

void ui_clearScreen(void)
{
    fputs(RESET CLEAR, stdout);
}

void ui_showMessage(const char *text)
{
    printf(" %s\n", text);
    flushPartial();
}

void ui_showPrompt(const char *text)
{
    printf(" %s ", text);
    flushPartial();
}

void ui_showGoodbye(void)
{
    fputs(RESET CLEAR, stdout);
    vcenter(5);
    printf("\n " BOLD "Thanks for playing Guess the Phrase. Goodbye!" RESET "\n\n");
}

/* ================= menus and help ================= */

void ui_showWelcome(const char *playerName)
{
    beginScreen(16);
    printHeading("* Guess the Phrase *");
    printf("\n " BOLD "Player: %s" BOLD_OFF "\n\n", playerName);
    printf("   1. Play\n\n");
    printf("   2. Scoreboard\n\n");
    printf("   3. Help\n\n");
    printf("   4. Quit\n\n");
}

void ui_showHelp(void)
{
    beginScreen(21);
    printHeading("* Help *");
    printf("\n " BOLD "How to play" BOLD_OFF "\n");
    printf("  - Guess one letter, or type the whole phrase (letters only).\n");
    printf("  - Type ? to get the hint.\n");
    printf("  - You have %d tries.\n\n", MAX_TRIES);
    printf(" " BOLD "Scoring" BOLD_OFF "\n");
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

    beginScreen(count * 2 + 8);
    printHeading("* Choose a category *");
    printf("\n");
    for (i = 0; i < count; i++) {
        printf("   %d. %s\n\n", i + 1, names[i]);
    }
    printf("   %d. Random\n\n", count + 1);
}

void ui_showThemeMenu(void)
{
    beginScreen(12);
    printHeading("* Choose a theme *");
    printf("\n");
    printf("   1. Hangman\n\n");
    printf("   2. Sinking ship\n\n");
    printf("   3. Dying robot\n\n");
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

    /* draw the picture twice as wide and twice as tall */
    /* normal size: doubling just repeated characters */
    /* normal size: doubling just repeated characters */
    switch (theme) {
    case THEME_HANGMAN: drawHangman(wrong); break;
    case THEME_SHIP:    drawShip(wrong);    break;
    case THEME_ROBOT:   drawRobot(wrong);   break;
    default:            break;
    }
    g_sx = 1;
    g_sy = 1;
}

/* ================= the game screen ================= */

int ui_pickRandomPalette(void)
{
    return rand() % PALLATE_COUNT;   /* no srand here */
}

void ui_showGameBoard(const GameState *g)
{
    const Colors *c = getColors(g->palette);
    int i;

    /* background and text colour, then clear so the whole screen is coloured */
    fputs(c->bg, stdout);
    fputs(c->text, stdout);
    fputs(CLEAR, stdout);
    vcenter(23);

    printf("%s", c->accent);
    printHeading("* Guess the Phrase *");
    printf("%s", c->accent);
    printf("\n " BOLD "Category : %s     Score : %d" BOLD_OFF "%s%s\n",
           g->category, g->score, c->bg, c->accent);
    printf(" " BOLD "Tries left : %d / %d     Guessed : ", g->triesLeft, g->maxTries);
    for (i = 0; i < g->guessedCount; i++) {
        printf("%c ", g->guessed[i]);
    }
    printf(BOLD_OFF "%s%s\n\n", c->bg, c->text);

    ui_drawLifeStage(g->theme, g->triesLeft, g->maxTries);

    /* the phrase: wide letter spacing, bold, double height */
    
    printf("\n " BOLD);
    for (i = 0; g->masked[i] != '\0'; i++) {
        printf("%c   ", g->masked[i]);
    }
    printf(BOLD_OFF "%s%s\n", c->bg, c->text);
    g_sy = 1;
    printf("\n");

    if (g->hintUsed == 1) {
        printf(" " BOLD "Hint : %s" BOLD_OFF "\n", g->hint);
    } else {
        printf(" Type ? for a hint (small cost), or a letter, or the whole phrase\n");
    }

    /* Do not leave the terminal in this palette for the next screen. */
    /* keep the palette so the prompt below stays coloured */
    fflush(stdout);
}

/* ================= end screens ================= */

void ui_showWinScreen(const GameState *g)
{
    const Colors *c = getColors(g->palette);

    fputs(c->bg, stdout);
    fputs(c->green, stdout);
    fputs(CLEAR, stdout);
    vcenter(11);
    
    printf("\n  " BOLD REVERSE "   G A M E   O V E R   " NO_REVERSE "\n\n");
    printf("  " BOLD "Y O U   W O N" BOLD_OFF "%s%s\n\n", c->bg, c->green);
    g_sy = 1;
    printf("  " BOLD "Score : " REVERSE " %d " NO_REVERSE "\n\n", g->score);
    printf("  * Play again\n");
    printf("  * Quit\n\n");
    fflush(stdout);
}

void ui_showLoseScreen(const GameState *g)
{
    const Colors *c = getColors(g->palette);

    fputs(c->bg, stdout);
    fputs(c->red, stdout);
    fputs(CLEAR, stdout);
    vcenter(13);
    
    printf("\n  " BOLD REVERSE "   G A M E   O V E R   " NO_REVERSE "\n\n");
    printf("  " BOLD "Y O U   L O S T" BOLD_OFF "%s%s\n\n", c->bg, c->red);
    g_sy = 1;
    printf("  " BOLD "The phrase was: %s" BOLD_OFF "%s%s\n\n", g->phrase, c->bg, c->red);
    printf("  " BOLD "Score : " REVERSE " %d " NO_REVERSE "\n\n", g->score);
    printf("  * Play again\n");
    printf("  * Quit\n\n");
    fflush(stdout);
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

    beginScreen(shown * 2 + 9);
    printHeading("* Scoreboard *");
    printf("\n " BOLD "Top scores for %s" BOLD_OFF "\n\n", playerName);

    if (count <= 0 || entries == NULL) {
        printf(" No scores yet - play a game to get on the board!\n");
    } else {
        for (i = 0; i < shown; i++) {
            printf("   %d. %d\n\n", i + 1, entries[i].score);
        }
    }
    printf("\n");
}
