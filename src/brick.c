#include "brick.h"

struct Brick brick[NUM_BRICKS];
SDL_Rect srcBrick;
int touched = 2;

bool allBricksInvisible()
{
    for (int i = 0; i < NUM_BRICKS; i++)
    {
        if (brick[i].isVisible)
        {
            return false;
        }
    }
    return true;
}