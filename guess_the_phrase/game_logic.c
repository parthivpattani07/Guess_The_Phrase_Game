#include "game_logic.h"

static int isLetter(char c)
{
    if (c >= 'a' && c <= 'z')
    {
        return 1;
    }
    if (c >= 'A' && c <= 'Z')
    {
        return 1;
    }
    return 0;
}

static char toLowerChar(char c)
{
    if (c >= 'A' && c <= 'Z')
    {
        return c + ('a' - 'A');
    }
    return c;
}

static void copyString(char *dest, const char *src, int maxSize)
{
    int i = 0;

    while (src[i] != '\0' && i < maxSize - 1)
    {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

static int lettersMatch(const char *a, const char *b)
{
    int i = 0;
    int j = 0;

    while (1)
    {
        while (a[i] != '\0' && !isLetter(a[i]))
        {
            i++;
        }
        while (b[j] != '\0' && !isLetter(b[j]))
        {
            j++;
        }
        if (a[i] == '\0' || b[j] == '\0')
        {
            break;
        }
        if (toLowerChar(a[i]) != toLowerChar(b[j]))
        {
            return 0;
        }
        i++;
        j++;
    }

    if (a[i] == '\0' && b[j] == '\0')
    {
        return 1;
    }
    return 0;
}

void logic_maskPhrase(const char *phrase, char *masked)
{
    int i = 0;

    while (phrase[i] != '\0')
    {
        if (isLetter(phrase[i]))
        {
            masked[i] = '_';
        }
        else
        {
            masked[i] = phrase[i];
        }
        i++;
    }
    masked[i] = '\0';
}

void logic_initRound(GameState *g, const char *phrase,
                     const char *hint, const char *category)
{
    copyString(g->phrase, phrase, MAX_PHRASE_LEN);
    copyString(g->hint, hint, MAX_HINT_LEN);
    copyString(g->category, category, MAX_CATEGORY_LEN);

    logic_maskPhrase(g->phrase, g->masked);

    g->triesLeft = MAX_TRIES;
    g->maxTries = MAX_TRIES;
    g->score = 0;
    g->hintUsed = 0;
}

int logic_checkLetter(GameState *g, char letter)
{
    int matches = 0;
    int i;
    char target = toLowerChar(letter);

    for (i = 0; g->phrase[i] != '\0'; i++)
    {
        if (toLowerChar(g->phrase[i]) == target && g->masked[i] == '_')
        {
            g->masked[i] = g->phrase[i];
            matches++;
        }
    }

    if (matches > 0)
    {
        g->score += matches * POINTS_PER_LETTER;
    }
    else
    {
        g->triesLeft -= 1;
        g->score -= PENALTY_WRONG_LETTER;
        if (g->score < 0)
        {
            g->score = 0;
        }
    }

    return matches;
}

int logic_checkPhrase(GameState *g, const char *guess)
{
    int hiddenCount = 0;
    int i;

    for (i = 0; g->masked[i] != '\0'; i++)
    {
        if (g->masked[i] == '_')
        {
            hiddenCount++;
        }
    }

    if (lettersMatch(guess, g->phrase))
    {
        copyString(g->masked, g->phrase, MAX_PHRASE_LEN);
        g->score += hiddenCount * POINTS_PER_HIDDEN_LETTER;
        return 1;
    }

    g->triesLeft -= 1;
    g->score -= PENALTY_WRONG_PHRASE;
    if (g->score < 0)
    {
        g->score = 0;
    }
    return 0;
}

int logic_useHint(GameState *g)
{
    if (g->hintUsed == 1)
    {
        return 0;
    }

    g->hintUsed = 1;
    g->score -= PENALTY_HINT;
    if (g->score < 0)
    {
        g->score = 0;
    }
    return 1;
}

int logic_isWon(const GameState *g)
{
    int i;

    for (i = 0; g->masked[i] != '\0'; i++)
    {
        if (g->masked[i] == '_')
        {
            return 0;
        }
    }
    return 1;
}

int logic_isLost(const GameState *g)
{
    if (g->triesLeft <= 0)
    {
        return 1;
    }
    return 0;
}

void logic_finalizeScore(GameState *g)
{
    g->score += BONUS_WIN;
    if (g->hintUsed == 0)
    {
        g->score += BONUS_NO_HINT;
    }
    g->score += g->triesLeft * BONUS_PER_TRY_LEFT;
}
