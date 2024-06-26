#ifndef AUDIO_H
#define AUDIO_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>
#include "game.h"
#define NUM_CHANNELS 16

extern Mix_Music *menuSound;
extern Mix_Music *roundStartSound;
extern Mix_Music *dohRoundSound;
extern Mix_Music *gameOverSound;
extern Mix_Music *victorySound;
extern Mix_Music *dohLevelSound;
extern Mix_Music *dohFaceSound;
extern Mix_Chunk *dohDeathSoundEffect;
extern Mix_Chunk *laserSoundEffect;
extern Mix_Chunk *brickSoundEffect;
extern Mix_Chunk *brickUndestructibleSoundEffect;
extern Mix_Chunk *enlargeSoundEffect;
extern Mix_Chunk *explosionSoundEffect;
extern Mix_Chunk *vaultCollisionSoundEffect;

void initializeAudio();
void playLoopingMusic(Mix_Music *music);
void playOneTimeMusic(Mix_Music *music);
void playLoopingSoundEffect(Mix_Chunk *soundEffect);
void playOneTimeSoundEffect(Mix_Chunk *soundEffect);
void playLevelSound();
void playBrickCollisionSound(bool isDestroyed);
void stopMusic();
void cleanupAudio();

#endif // AUDIO_H