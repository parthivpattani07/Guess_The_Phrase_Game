#include<stdio.h>
#include "game.h"
#include "game_types.h"
#include "data_manager.h"
#include "ui.h"
#include "input.h"

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
