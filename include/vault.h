#ifndef ARKANOID_VAULT_H
#define ARKANOID_VAULT_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "ball.h"

extern SDL_Rect srcVault;
extern SDL_Rect destVault;
extern int x_vault;
extern int vault_width;

void resetVaultPosition();
void moveVault(const Uint8 *keys);

#endif // ARKANOID_VAULT_H
