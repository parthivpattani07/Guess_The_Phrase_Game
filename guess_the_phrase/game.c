#include<stdio.h>
#include<string.h>
#include "game.h"
#include "game_logic.h"
#include "data_manager.h"
#include "ui.h"
#include "input.h"


/* running the menu using modules: 3,4 */
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
                // repeats automatically until game_playRound() no longer returns 1
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
/*shows scorecard on screen when chosen using modules: 1,3,4*/
void game_showScoreboard(const GameState *g)
{
    ScoreEntry list[MAX_SCORES];
    int n = data_loadPlayerScores(g->playerName, list, MAX_SCORES);
    data_sortScores(list,n);
    ui_clearScreen();
    ui_showScoreboard(g->playerName, list, n);
    input_waitForEnter();
}
/*shows how to play the game using modules:3,4 */
void  game_showHelp()
{
    ui_clearScreen();
    ui_showHelp();
    input_waitForEnter();
}
/*gives option to choose category using modules: 3,4*/
int game_chooseCategory()
{
    char names[MAX_CATEGORIES][MAX_CATEGORY_LEN];
    int count = data_getCategoryCount();
    for (int i=0;i<count;i++)
    {
        data_getCategoryName(i, names[i]);
        
    }
    ui_showCategoryMenu(names, count);
    
    int choice = input_readMenuChoice(1, count +1);
    if (choice == count + 1)
    {
        return -1;
    }
    else
    {
        return choice-1;
    }
}
/*chooses theme using the modules: 3,4 */
Theme game_chooseTheme()
{
    ui_showThemeMenu();

    return (Theme)input_readMenuChoice(1, 3);
}
/* starts round and plays round on repeat using : 2,3 */
int game_playRound(GameState *g)
{
    #define MAX_MESSAGE_LEN 256
    char message[MAX_MESSAGE_LEN];
    
    Theme theme = game_chooseTheme();

    int category_index = game_chooseCategory();
    
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
            case GUESS_LETTER: {
                int matches = logic_checkLetter(g, guess[0]);
                if (matches > 0) {
                    strcpy(message, "Correct!");
                } else {
                    strcpy(message, "Wrong letter");
                }
                break;
            }
            case GUESS_PHRASE: {
                if (logic_checkPhrase(g, guess) == 1) {
                    strcpy(message, "Correct!");
                } else {
                    strcpy(message, "Wrong phrase");
                }
                break;
            }
            case GUESS_HINT: {
                if (logic_useHint(g) == 1) {
                    strcpy(message, "Hint used: -20 points");
                } else {
                    strcpy(message, "Hint already shown");
                }
                break;
            }
            default:
                break;

        }
        printf("%s\n",message);


    }
    return game_endRound(g, logic_isWon(g));
}
/* finished the round using the modules: 1,3,4*/
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
/*starts game by showing menu using modules: 3,4*/
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


