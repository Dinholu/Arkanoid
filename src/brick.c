#include "brick.h"
#include "ball.h"
#include "vault.h"
#include "harmful.h"

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