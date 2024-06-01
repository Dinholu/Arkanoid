#include <SDL2/SDL.h>
#include "game.h"

void freeBricks()
{
    if (brick != NULL)
    {
        free(brick);
        brick = NULL;
    }
}
void freeAllResources()
{
    // Libérer toutes les surfaces SDL
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
    freeBricks();

    // Détruire la fenêtre
    if (pWindow)
        SDL_DestroyWindow(pWindow);

    // Quitter SDL
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