#ifndef ARKANOID_VAULT_H
#define ARKANOID_VAULT_H

#include <SDL2/SDL.h>
#include <stdbool.h>

#include "ball.h"

SDL_Rect srcVault = {384, 160, 82, 16};
SDL_Rect destVault;
int x_vault;
int vault_width;


void moveVault(const Uint8 *keys);

#endif // ARKANOID_VAULT_H
