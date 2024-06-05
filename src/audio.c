#include "audio.h"
#include <stdio.h>
#include <stdlib.h>

Mix_Music *menuSound = NULL;
Mix_Music *roundStartSound = NULL;
Mix_Music *laserSound = NULL;
Mix_Music *gameOverSound = NULL;

void initializeAudio()
{
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        fprintf(stderr, "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        exit(EXIT_FAILURE);
    }

    menuSound = Mix_LoadMUS("resources/audio/01_Story.mp3");
    roundStartSound = Mix_LoadMUS("resources/audio/02_Round_Start.mp3");
    gameOverSound = Mix_LoadMUS("resources/audio/06_Game_Over.mp3");

    if (!menuSound || !roundStartSound || !laserSound || !gameOverSound)
    {
        fprintf(stderr, "Failed to load audio files! SDL_mixer Error: %s\n", Mix_GetError());
        exit(EXIT_FAILURE);
    }
}

void playMenuSound()
{
    if (Mix_PlayingMusic() == 0)
    {
        Mix_PlayMusic(menuSound, -1); // Play menu sound in loop
    }
}

void playRoundStartSound()
{
    if (Mix_PlayingMusic() == 0)
    {
        Mix_PlayMusic(roundStartSound, 1); // Play round start sound once
    }
}

void playGameOverSound()
{
    if (Mix_PlayingMusic() == 0)
    {
        Mix_PlayMusic(gameOverSound, 1); // Play game over sound once
    }
}

void cleanupAudio()
{
    Mix_FreeMusic(menuSound);
    Mix_FreeMusic(roundStartSound);
    Mix_FreeMusic(laserSound);
    Mix_FreeMusic(gameOverSound);
    Mix_CloseAudio();
}
