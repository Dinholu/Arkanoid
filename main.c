#include <SDL2/SDL.h>
#include "game.h"

int main(int argc, char **argv)
{
    initializeSDL();
    showOptionsMenu(pWindow, win_surf);
    resetGame();
    mainGameLoop();

    SDL_Quit();
    return 0;
}