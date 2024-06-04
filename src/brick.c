#include "brick.h"

struct Brick brick[NUM_BRICKS];
SDL_Rect srcBrick;
int health = 2;

bool allBricksInvisible()
{
    for (int i = 0; i < NUM_BRICKS; i++)
    {
        if (brick[i].isVisible && brick[i].isDestructible)
        {
            return false;
        }
    }
    return true;
}
