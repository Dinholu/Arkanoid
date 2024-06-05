#ifndef AUDIO_H
#define AUDIO_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>

extern Mix_Music *menuSound;
extern Mix_Music *roundStartSound;
extern Mix_Music *dohRoundSound;
extern Mix_Music *laserSound;
extern Mix_Music *gameOverSound;

void initializeAudio();
void playMenuSound();
void stopMusic();
void playRoundStartSound();
void playGameOverSound();
void cleanupAudio();

#endif // AUDIO_H