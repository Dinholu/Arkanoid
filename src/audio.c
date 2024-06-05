#include "audio.h"
#include <stdio.h>
#include <stdlib.h>

Mix_Music *menuSound = NULL;
Mix_Music *roundStartSound = NULL;
Mix_Music *gameOverSound = NULL;
Mix_Music *victorySound = NULL;
Mix_Music *dohLevelSound = NULL;
Mix_Music *dohFaceSound = NULL;
Mix_Chunk *laserSoundEffect = NULL;
Mix_Chunk *brickSoundEffect = NULL;
Mix_Chunk *brickUndestructibleSoundEffect = NULL;
Mix_Chunk *enlargeSoundEffect = NULL;
Mix_Chunk *explosionSoundEffect = NULL;

void initializeAudio()
{
    if (Mix_OpenAudio(48000, AUDIO_F32, 2, 4096) < 0)
    {
        fprintf(stderr, "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        exit(EXIT_FAILURE);
    }

    menuSound = Mix_LoadMUS("./01_Story.mp3");
    roundStartSound = Mix_LoadMUS("./02_Round_Start.mp3");
    dohLevelSound = Mix_LoadMUS("./03_Doh.mp3");
    victorySound = Mix_LoadMUS("./04_Ending.mp3");
    dohFaceSound = Mix_LoadMUS("./05_Doh_Face.mp3");
    gameOverSound = Mix_LoadMUS("./06_Game_Over.mp3");
    laserSoundEffect = Mix_LoadWAV("./laser_sound.wav");

    if (!menuSound || !roundStartSound || !gameOverSound || !dohLevelSound || !victorySound || !dohFaceSound || !laserSoundEffect)
    {
        fprintf(stderr, "Failed to load audio files! SDL_mixer Error: %s\n", Mix_GetError());
        exit(EXIT_FAILURE);
    }
}

void playLoopingMusic(Mix_Music *music)
{
    if (Mix_PlayingMusic() == 0)
    {
        Mix_PlayMusic(music, -1); // -1 for looping
    }
}

void playOneTimeMusic(Mix_Music *music)
{
    if (Mix_PlayingMusic() == 0)
    {
        Mix_PlayMusic(music, 1); // 1 for playing once
    }
}

void playLoopingSoundEffect(Mix_Chunk *soundEffect)
{
    Mix_PlayChannel(-1, soundEffect, -1); // -1 for looping
}

void playOneTimeSoundEffect(Mix_Chunk *soundEffect)
{
    Mix_PlayChannel(-1, soundEffect, 0); // 0 for playing once
}

void stopMusic()
{
    Mix_HaltMusic();
}

void cleanupAudio()
{
    Mix_FreeMusic(menuSound);
    Mix_FreeMusic(roundStartSound);
    Mix_FreeMusic(dohLevelSound);
    Mix_FreeMusic(victorySound);
    Mix_FreeMusic(dohFaceSound);
    Mix_FreeMusic(gameOverSound);
    Mix_FreeChunk(laserSoundEffect);
    Mix_CloseAudio();
}
