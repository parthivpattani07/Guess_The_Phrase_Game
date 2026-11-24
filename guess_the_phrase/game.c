#include<stdio.h>
#include<string.h>
#include "game.h"
#include "game_logic.h"
#include "data_manager.h"
#include "ui.h"
#include "input.h"

/* Value returned by game_chooseCategory when the player typed ! (go back to the menu) */
#define QUIT_TO_MENU -2


/* runs the menu shows welcone screen and allows user to choose if they want to play , see the scorecard , get help to play the game or quit the game .
  modules called: 3,4 */
void game_runMenu(GameState *g)
{
    int running=1;
    while (running !=0)
    {
        ui_clearScreen();
        ui_showWelcome(g->playerName);
        
        int choice= input_readMenuChoice(1, 4);
        switch (choice) 
        {
            case 1:
            while (game_playRound(g) == 1) 
            {
                /*repeats automatically*/
            }
                break;
            case 2:
                game_showScoreboard(g);
                break;

            case 3:
                game_showHelp();
                break;

            case 4:
                running = 0;
                    break;
            
        }
    }
    

}
/*shows the top five scores of the current user 
modules called : 1,3,4 */
void game_showScoreboard(const GameState *g)
{
    ScoreEntry list[MAX_SCORES];
    int n = data_loadPlayerScores(g->playerName, list, MAX_SCORES);
    data_sortScores(list,n);
    ui_clearScreen();
    ui_showScoreboard(g->playerName, list, n);
    input_waitForEnter();
}
/* explains how to play the game and how the points are counted .
modules called :3,4 */
void  game_showHelp()
{
    ui_clearScreen();
    ui_showHelp();
    input_waitForEnter();
}
/* allows user to choose the category of the phrase.
modules called: 3,4*/
int game_chooseCategory()
{
    char names[MAX_CATEGORIES][MAX_CATEGORY_LEN];
    int count = data_getCategoryCount();
    for (int i=0;i<count;i++)
    {
        data_getCategoryName(i, names[i]);
        
    }
    ui_showCategoryMenu(names, count);
    printf("(type ! to go back to the main menu)\n");
    
    int choice = input_readMenuChoice(1, count +1);
    if (choice == 0)                 /* player typed ! */
    {
        return QUIT_TO_MENU;
    }
    if (choice == count + 1)
    {
        return -1;
    }
    else
    {
        return choice-1;
    }
}
/* allows the user to choose the theme.
modules called : 3,4 */
Theme game_chooseTheme()
{
    ui_showThemeMenu();
    printf("(type ! to go back to the main menu)\n");

    return (Theme)input_readMenuChoice(1, 3);   /* 0 means the player typed ! */
}
/* starts round after 'play' is selected in game_run_menu .
modules called : 2,3 */
int game_playRound(GameState *g)
{
    #define MAX_MESSAGE_LEN 256
    char message[MAX_MESSAGE_LEN];
    
    Theme theme = game_chooseTheme();
    if ((int)theme == 0)             /* player typed ! : back to the main menu */
    {
        return 0;
    }
    
    int category_index = game_chooseCategory();
    if (category_index == QUIT_TO_MENU)   /* player typed ! : back to the main menu */
    {
        return 0;
    }
    
    char phrase[MAX_PHRASE_LEN];
    char hint[MAX_HINT_LEN];
    char category[MAX_CATEGORY_LEN];

    if (data_fetchPhrase(category_index, phrase, hint, category) == 0) 
    {
        strcpy(message, "Phrase not found.");
        printf("%s\n", message);
        return 0;
    }
    logic_initRound(g, phrase, hint, category);
    g->palette = ui_pickRandomPalette();
    g->theme = theme;
    input_clearGuessed(g);
    message[0] = '\0';
    
    while(!logic_isWon(g) && !logic_isLost(g))
    {
        ui_clearScreen();
        ui_showGameBoard(g);
        char guess[MAX_PHRASE_LEN];
        
        
        GuessType type = input_readGuess(g, guess);
        switch (type) {
            case GUESS_LETTER: 
            {
                int matches = logic_checkLetter(g, guess[0]);
                if (matches > 0) 
                {
                    strcpy(message, "Correct!");
                } 
                else 
                {
                    strcpy(message, "Wrong letter");
                }
                break;
            }
            case GUESS_PHRASE: 
            {
                if (logic_checkPhrase(g, guess) == 1) 
                {
                    strcpy(message, "Correct!");
                }
                else 
                {
                    strcpy(message, "Wrong phrase");
                }
                break;
            }
            case GUESS_HINT: 
            {
                if (logic_useHint(g) == 1)
                 {
                    strcpy(message, "Hint used: -20 points");
                } else 
                {
                    strcpy(message, "Hint already shown");
                }
                break;
            }
            case GUESS_QUIT:
            {
                return 0;            /* leave the round, back to the main menu */
            }
            default:
                break;

        }
        printf("%s\n",message);


    }
    return game_endRound(g, logic_isWon(g));
}
/* finishes the round and shows win or lose screen to user and allows to choose whether to play again or go to menu.
modules called : 1,3,4*/
int game_endRound(GameState *g, int won)
{
    if (logic_isWon(g)==1)
    {
        data_saveScore(g->playerName, g->score);
        ui_clearScreen();
        ui_showWinScreen(g);
    }
    else
    {
        ui_clearScreen();
        ui_showLoseScreen(g);
    }
    
    if (input_readMenuChoice(1, 2)==1)
    {
        return 1;
    }
    else
    {
        return 0;
    }

}

/* asks for username and starts by call game_run_menu and also shows goodbye at the end .
modules called : 3,4*/
int main()
{
    GameState g={0};

    if( data_init()==0)
    {
        printf("Error: Data folder Missing.");
        return 1;
    }
    input_readPlayerName(g.playerName);
    game_runMenu(&g);
    ui_showGoodbye();
    return 0;
}
