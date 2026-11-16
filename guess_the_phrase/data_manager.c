/*
 * Module 1 - Data & Phrase Management  (prefix: data_)
 *
 * Supplies data to the game: categories, a random phrase with its hint,
 * and saving / loading of scores.
 * This module never prints to the screen and never plays the game.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "data_manager.h"

#define LINE_LEN 300     /* longest line we read from any file */
#define PATH_LEN 200     /* longest file name we build         */


/* ------------------------------------------------------------------ */
/* Private helpers (static = only visible inside this file)           */
/* ------------------------------------------------------------------ */

/* Removes the newline (\n or \r\n) and any spaces at the end of a line. */
static void cleanLine(char *line)
{
    size_t n = strcspn(line, "\r\n");   /* position of the newline */
    line[n] = '\0';
    while (n > 0 && line[n - 1] == ' ') {
        n--;
        line[n] = '\0';
    }
}

/* Reads the next NON-EMPTY line from fp into line (already cleaned).
 * Returns 1 if a line was read, 0 at the end of the file. */
static int nextLine(FILE *fp, char *line, int size)
{
    while (fgets(line, size, fp) != NULL) {
        cleanLine(line);
        if (line[0] != '\0') {
            return 1;
        }
    }
    return 0;
}

/* Copies src into dest but never writes more than size characters
 * (including the end marker), so a long text can never overflow dest. */
static void copyText(char *dest, int size, const char *src)
{
    strncpy(dest, src, size - 1);
    dest[size - 1] = '\0';
}

/* Compares two texts ignoring upper / lower case. Returns 1 if equal. */
static int sameIgnoreCase(const char *a, const char *b)
{
    while (*a != '\0' && *b != '\0') {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) {
            return 0;
        }
        a++;
        b++;
    }
    return *a == *b;   /* both must end together */
}


/* ------------------------------------------------------------------ */
/* Start-up                                                           */
/* ------------------------------------------------------------------ */

/* Call ONCE when the program starts.
 * Seeds the random generator and checks categories.txt is usable.
 * Returns 1 if OK, 0 if categories.txt cannot be opened (or is empty). */
int data_init(void)
{
    srand((unsigned int)time(NULL));   /* seed ONLY ONCE */

    if (data_getCategoryCount() == 0) {
        return 0;
    }
    return 1;
}


/* ------------------------------------------------------------------ */
/* Categories                                                         */
/* ------------------------------------------------------------------ */

/* Counts the categories in categories.txt. Returns 0 if file is missing. */
int data_getCategoryCount(void)
{
    int count = 0;
    char line[LINE_LEN];

    FILE *fp = fopen(CATEGORIES_FILE, "r");
    if (fp == NULL) {
        return 0;
    }

    while (nextLine(fp, line, LINE_LEN)) {
        count++;
    }

    fclose(fp);
    return count;
}

/* Copies the name of category number index (0 = first) into outName.
 * outName must hold at least MAX_CATEGORY_LEN characters.
 * Returns 1 if OK, 0 if index is invalid. */
int data_getCategoryName(int index, char *outName)
{
    char line[LINE_LEN];
    int i = 0;

    if (index < 0 || outName == NULL) {
        return 0;
    }

    FILE *fp = fopen(CATEGORIES_FILE, "r");
    if (fp == NULL) {
        return 0;
    }

    while (nextLine(fp, line, LINE_LEN)) {
        if (i == index) {
            copyText(outName, MAX_CATEGORY_LEN, line);
            fclose(fp);
            return 1;
        }
        i++;
    }

    fclose(fp);
    return 0;   /* index was bigger than the number of categories */
}


/* ------------------------------------------------------------------ */
/* Phrases                                                            */
/* ------------------------------------------------------------------ */

/* Gives the game a random phrase, its hint and its category name.
 *   categoryIndex : 0,1,2...  or  -1 = pick a random category first
 *   outPhrase     : room for MAX_PHRASE_LEN characters
 *   outHint       : room for MAX_HINT_LEN characters
 *   outCategory   : room for MAX_CATEGORY_LEN characters
 * Returns 1 if OK, 0 on error. */
int data_fetchPhrase(int categoryIndex, char *outPhrase,
                     char *outHint, char *outCategory)
{
    char categoryName[MAX_CATEGORY_LEN];
    char path[PATH_LEN];
    char line[LINE_LEN];
    int categoryCount;
    int lineCount = 0;
    int target;
    int i = 0;

    if (outPhrase == NULL || outHint == NULL || outCategory == NULL) {
        return 0;
    }

    categoryCount = data_getCategoryCount();
    if (categoryCount == 0) {
        return 0;
    }

    /* -1 means "choose a random category first" */
    if (categoryIndex == -1) {
        categoryIndex = rand() % categoryCount;
    }

    if (!data_getCategoryName(categoryIndex, categoryName)) {
        return 0;
    }
    copyText(outCategory, MAX_CATEGORY_LEN, categoryName);

    /* build the file name:  data/  +  movie  +  .txt */
    snprintf(path, PATH_LEN, "%s%s.txt", CATEGORY_FOLDER, categoryName);

    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        return 0;
    }

    /* pass 1: count the usable lines (lines that contain a '|') */
    while (nextLine(fp, line, LINE_LEN)) {
        if (strchr(line, '|') != NULL) {
            lineCount++;
        }
    }

    if (lineCount == 0) {
        fclose(fp);
        return 0;
    }

    /* choose a random line number */
    target = rand() % lineCount;

    /* pass 2: go back to the start and walk to that line */
    rewind(fp);
    while (nextLine(fp, line, LINE_LEN)) {
        char *bar = strchr(line, '|');
        if (bar == NULL) {
            continue;                 /* not a phrase line, ignore it */
        }

        if (i == target) {
            *bar = '\0';              /* cut the line: phrase | hint */
            cleanLine(line);          /* remove spaces at end of phrase */

            char *hint = bar + 1;
            while (*hint == ' ') {    /* skip spaces at start of hint */
                hint++;
            }

            copyText(outPhrase, MAX_PHRASE_LEN, line);
            copyText(outHint, MAX_HINT_LEN, hint);

            fclose(fp);
            return 1;
        }
        i++;
    }

    fclose(fp);
    return 0;
}


/* ------------------------------------------------------------------ */
/* Scores                                                             */
/* ------------------------------------------------------------------ */

/* Adds one line  name|score  to the end of scores.txt.
 * Returns 1 if OK, 0 on error. */
int data_saveScore(const char *playerName, int score)
{
    if (playerName == NULL) {
        return 0;
    }

    FILE *fp = fopen(SCORES_FILE, "a");   /* "a" = add to the end */
    if (fp == NULL) {
        return 0;
    }

    fprintf(fp, "%s|%d\n", playerName, score);

    fclose(fp);
    return 1;
}

/* Loads ONLY the scores of playerName (ignoring upper/lower case).
 * Returns how many were loaded (0 if the file does not exist). */
int data_loadPlayerScores(const char *playerName,
                          ScoreEntry entries[], int maxEntries)
{
    char line[LINE_LEN];
    int n = 0;

    if (playerName == NULL || entries == NULL || maxEntries <= 0) {
        return 0;
    }

    FILE *fp = fopen(SCORES_FILE, "r");
    if (fp == NULL) {
        return 0;
    }

    while (n < maxEntries && nextLine(fp, line, LINE_LEN)) {
        char *bar = strchr(line, '|');
        if (bar == NULL) {
            continue;                 /* broken line, ignore it */
        }

        *bar = '\0';                  /* line = name part, bar+1 = score */

        if (sameIgnoreCase(line, playerName)) {
            copyText(entries[n].name, MAX_NAME_LEN, line);
            entries[n].score = atoi(bar + 1);
            n++;
        }
    }

    fclose(fp);
    return n;
}

/* Sorts the scores from the highest to the lowest (bubble sort). */
void data_sortScores(ScoreEntry entries[], int count)
{
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - 1 - i; j++) {
            if (entries[j].score < entries[j + 1].score) {
                ScoreEntry temp = entries[j];       /* swap */
                entries[j] = entries[j + 1];
                entries[j + 1] = temp;
            }
        }
    }
}
