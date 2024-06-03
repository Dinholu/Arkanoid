#include <SDL2/SDL.h>
#include "game.h"

void freeAllResources()
{
    if (gameSprites)
        SDL_FreeSurface(gameSprites);
    if (asciiSprites)
        SDL_FreeSurface(asciiSprites);
    if (menuSprites)
        SDL_FreeSurface(menuSprites);
    if (topWallSprites)
        SDL_FreeSurface(topWallSprites);
    if (leftWallSprites)
        SDL_FreeSurface(leftWallSprites);
    if (rightWallSprites)
        SDL_FreeSurface(rightWallSprites);

    if (pWindow)
        SDL_DestroyWindow(pWindow);
    SDL_Quit();
}

int main(int argc, char **argv)
{
    initializeSDL();
    showOptionsMenu(pWindow, win_surf);
    resetGame();
    mainGameLoop();
    freeAllResources();
    return 0;
}