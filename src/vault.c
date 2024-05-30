#include "vault.h"

void moveVault(const Uint8 *keys)
{
    const int wallWidth = srcEdgeWall.w;
    const int vaultSpeed = 10;

    if (keys[SDL_SCANCODE_LEFT])
    {
        x_vault -= vaultSpeed;
    }
    if (keys[SDL_SCANCODE_RIGHT])
    {
        x_vault += vaultSpeed;
    }

    if (x_vault < wallWidth)
    {
        x_vault = wallWidth;
    }

    if (x_vault > win_surf->w - vault_width - wallWidth)
    {
        x_vault = win_surf->w - vault_width - wallWidth;
    }
}