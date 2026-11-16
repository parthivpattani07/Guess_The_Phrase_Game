#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include "input.h"

/* FUNCTION 1: Flush leftover characters from the input buffer */
void input_flushBuffer(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
        /* Discard remaining characters. */
    }
}

/* FUNCTION 2: Read one complete line safely */
int input_readLine(char *buffer, int size) {
    size_t pos;
    if (buffer == NULL || size <= 0) {
        return 0;
    }
    if (fgets(buffer, size, stdin) == NULL) {
        return 0;
    }
    pos = strcspn(buffer, "\n" );
    if (buffer[pos] == '\n' ) {
        buffer[pos] = '\0' ;
    } else {
        /* No newline means the line may have exceeded the buffer. */
        input_flushBuffer();
    }
    return 1;
}

/* FUNCTION 3: Check whether a character is an alphabetic letter */
int input_isValidLetter(char ch) {
    return isalpha((unsigned char)ch) != 0;
}

/* FUNCTION 4: Check whether text contains only letters and spaces */
int input_isValidPhrase(const char *text) {
    int letterCount = 0;
    int i;
    if (text == NULL) {
        return 0;
    }
    for (i = 0; text[i] != '\0' ; i++) {
        if (isalpha((unsigned char)text[i])) {
            letterCount++;
        } else if (text[i] !=' ' ) {
            return 0;
        }
    }
    return letterCount > 0;
}

/* FUNCTION 5: Check whether a letter has already been guessed */
int input_hasAlreadyGuessed(const GameState *g, char letter) {
    int i;
    char lowerLetter;
    if (g == NULL || g->guessedCount < 0) {
        return 0;
    }
    lowerLetter = (char)tolower((unsigned char)letter);
    for (i = 0; i < g->guessedCount && i < MAX_GUESSED; i++) {
        if (g->guessed[i] == lowerLetter) {
            return 1;
        }
    }
    return 0;
}

/* FUNCTION 6: Store a new guessed letter in lowercase */
void input_recordLetter(GameState *g, char letter) {
    char lowerLetter;
    if (g == NULL || g->guessedCount < 0 || g->guessedCount >= MAX_GUESSED || !input_isValidLetter(letter) || input_hasAlreadyGuessed(g, letter)) {
        return;
    }
    lowerLetter = (char)tolower((unsigned char)letter);
    g->guessed[g->guessedCount] = lowerLetter;
    g->guessedCount++;
}

/* FUNCTION 7: Clear the guessed-letter list at the start of a round */
void input_clearGuessed(GameState *g) {
    if (g != NULL) {
        g->guessedCount = 0;
    }
}

/* FUNCTION 8: Read and validate a letter, phrase, or hint request */
GuessType input_readGuess(GameState *g, char *outText) {
    char buffer[MAX_PHRASE_LEN];
    char *start;
    char *end;
    size_t len;
    if (g == NULL || outText == NULL) {
        return (GuessType)0;
    }
    while (1) {
        printf( "\nEnter your guess (letter / phrase / ? for hint):" );
        if (!input_readLine(buffer, sizeof(buffer))) {
            outText[0] = '\0' ;
            return (GuessType)0;
        }
        /* Trim spaces from the beginning and end. */
        start = buffer;
        while (*start == ' ') {
            start++;
        }
        end = start + strlen(start);
        while (end > start && end[-1] == ' ') {
            end--;
        }
        *end = '\0' ;
        len = strlen(start);
        if (len == 0) {
            printf( "Please enter something.\n" );
            continue;
        }
        /* A question mark requests the hint. */
        if (strcmp(start, "?" ) == 0) {
            outText[0] = '\0' ;
            return GUESS_HINT;
        }
        /* A single character is treated as a letter guess. */
        if (len == 1) {
        }
        if (!input_isValidLetter(start[0])) {
            printf( "Please enter a letter.\n" );
            continue;
        }
        if (input_hasAlreadyGuessed(g, start[0])) {
            printf( "You already guessed %c. Try another letter.\n" , (char)toupper((unsigned char)start[0]));
            continue;
        }
        input_recordLetter(g, start[0]);
        outText[0] = start[0];
        outText[1] = '\0' ;
        return GUESS_LETTER;
        /* More than one character is treated as a phrase guess. */
        if (!input_isValidPhrase(start)) {
            printf("Phrases may only use letters and spaces.\n" );
            continue;
        }
        strcpy(outText, start);
        return GUESS_PHRASE;
    }
}

/* FUNCTION 9: Read a menu number strictly within the given range */
int input_readMenuChoice(int min, int max) {
    char buffer[64];
    char *end;
    long number;
    int i;
    if (min > max) {
        return min;
    }
    while (1) {
        printf( "Enter your choice (%d-%d): ", min, max);
        if (!input_readLine(buffer, sizeof(buffer))) {
            return min;
        }
        if (buffer[0] == '\0' ) {
            printf( "Please type a number.\n" );
            continue;
        }
        /* Accept digits only; signs and spaces are rejected. */
        for (i = 0; buffer[i] != '\0'; i++) {
            if (!isdigit((unsigned char)buffer[i])) {
                break;
            }
        }
        if (buffer[i] != '\0' ) {
            printf( "Please type a valid number.\n" );
            continue;
        }
        errno = 0;
        number = strtol(buffer, &end, 10);
        if (errno == ERANGE || *end != '\0' || number < INT_MIN || number > INT_MAX) {
            printf( "Invalid number. Try again.\n" );
            continue;
        }
        if (number >= min && number <= max) {
            return (int)number;
        }
        printf( "Please enter a number between %d and %d.\n", min, max);
    }
}

/* FUNCTION 10: Read a player name containing 1 to 15 letters */
void input_readPlayerName(char *outName) {
    char buffer[256];
    size_t i;
    size_t length;
    int valid;
    if (outName == NULL) {
        return;
    }
    while (1) {
        printf( "Enter your name (1-15 letters):" );
        if (!input_readLine(buffer, sizeof(buffer))) {
            outName[0] = '\0' ;
            return;
        }
        length = strlen(buffer);
        valid = (length >= 1 && length <= 15);
        for (i = 0; i < length; i++) {
            if (!isalpha((unsigned char)buffer[i])) {
                valid = 0;
                break;
            }
        }
        if (valid) {
            strcpy(outName, buffer);
            return;
        }
        printf( "Use letters only (1 to 15).\n" );
    }
}

/* FUNCTION 11: Wait for the player to press Enter */
void input_waitForEnter(void) {
    char buffer[10];
    printf( "\nPress Enter to continue..." );
    (void)input_readLine(buffer, sizeof(buffer));
}