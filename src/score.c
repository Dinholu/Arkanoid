#include "score.h"

void showHighScores(SDL_Surface *win_surf, SDL_Surface *asciiSprites)
{
    SDL_FillRect(win_surf, NULL, SDL_MapRGB(win_surf->format, 0, 0, 0));

    // Pour afficher le logo dans le HighScore
    SDL_Rect destLogo = {0, 128, srcLogo.w, srcLogo.h};
    destLogo.x = (win_surf->w - srcLogo.w) / 2;
    SDL_BlitSurface(menuSprites, &srcLogo, win_surf, &destLogo);

    HighScore highScores[MAX_HIGHSCORE];
    int count;
    readHighScores(highScores, &count);
    int startLeaderBoardY = srcLogo.h + 192;

    renderString(asciiSprites, win_surf, "LEADERBOARD", win_surf->w / 2, startLeaderBoardY, "center");

    for (int i = 0; i < count; i++)
    {
        char scoreText[256];
        sprintf(scoreText, "%d. %s %d", i + 1, highScores[i].name, highScores[i].score);
        renderString(asciiSprites, win_surf, scoreText, 164, startLeaderBoardY + 50 + i * 32, "left");
    }

    SDL_UpdateWindowSurface(pWindow);

    // Wait for a key press to return to the menu
    bool waiting = true;
    SDL_Event event;
    while (waiting)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                exit(EXIT_SUCCESS);
            }
            if (event.type == SDL_KEYDOWN)
            {
                SDL_FillRect(win_surf, NULL, SDL_MapRGB(win_surf->format, 0, 0, 0));
                waiting = false;
            }
        }
    }
}