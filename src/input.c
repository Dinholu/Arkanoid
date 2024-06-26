#include "input.h"

bool vWasPressed = false;
bool nWasPressed = false;
bool mWasPressed = false;
bool escapeWasPressed = false;
bool isPaused = false;
bool spaceWasPressed = false;
bool enteringName = false;
int nameIndex = 0;

void processInput(bool *quit)
{
    SDL_Event event;
    SDL_PumpEvents();
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_SPACE] && !spaceWasPressed)
    {
        spaceWasPressed = true;
        // isLaserBeam = true; POUR TEST
        if (isLaserBeam)
        {
            fireLaser();
        }

        for (int i = 0; i < MAX_BALLS; i++)
        {
            if (balls[i].isAttached)
            {
                balls[i].isAttached = false;
                balls[i].vy = ((int)balls[i].vyOld) != 0 ? (balls[i].vyOld * -1) : -5;
                balls[i].vx = ((int)balls[i].vxOld) != 0 ? (balls[i].vxOld * -1) : 0;
                balls[i].vyOld = 0;
                balls[i].vxOld = 0;
                releaseCount--;
                break;
            }
        }
    }
    if (keys[SDL_SCANCODE_ESCAPE] && !escapeWasPressed)
    {
        printf("Escape pressed\n");
        isPaused = true;
        escapeWasPressed = true;
    }

    if (keys[SDL_SCANCODE_ESCAPE] == 0)
    {
        escapeWasPressed = false;
    }

    if (isPaused)
    {
        return;
    }

    if (keys[SDL_SCANCODE_SPACE] == 0)
    {
        spaceWasPressed = false;
    }

    // BONUS SPLIT BALL (D_BONUS)
    if (keys[SDL_SCANCODE_B] && !vWasPressed)
    {
        splitBall();
        vWasPressed = true;
    }

    if (keys[SDL_SCANCODE_B] == 0)
    {
        vWasPressed = false;
    }
    if (keys[SDL_SCANCODE_N] == 0)
    {
        nWasPressed = false;
    }
    // BONUS WRAP LEVEL (B_BONUS)
    if (keys[SDL_SCANCODE_N])
    {
        if (!nWasPressed)
        {
            wraplevel();
        }
    }
    // BONUS ADD LIFE (P_BONUS)
    // if (keys[SDL_SCANCODE_V])
    // {
    //     if (!vWasPressed)
    //     {
    //         addLife();
    //         vWasPressed = true;
    //     }
    // }

    // if (keys[SDL_SCANCODE_V] == 0)
    // {
    //     vWasPressed = false;
    // }
    // BONUS SLOW DOWN BALL (S_BONUS)
    // if (keys[SDL_SCANCODE_C])
    // {
    //     slowDownBall();
    // }

    // BONUS CATCH AND FIRE (C_BONUS)
    if (keys[SDL_SCANCODE_X])
    {
        CatchAndFire();
    }
    // BONUS ENLARGE VAULT(E_BONUS)
    if (keys[SDL_SCANCODE_Z])
    {
        enlargeVault();
    }

    for (int i = 0; i < MAX_BALLS; i++)
    {
        if (balls[i].isAttached && (SDL_GetPerformanceCounter() - balls[i].attachTime) / (double)SDL_GetPerformanceFrequency() > 5.0)
        {
            balls[i].isAttached = false;
            printf("Ball %f released\n", balls[i].vy);
            balls[i].vy = ((int)balls[i].vyOld != 0) ? (balls[i].vyOld * -1) : -5;
            balls[i].vx = ((int)balls[i].vxOld != 0) ? (balls[i].vxOld * -1) : 0;
            balls[i].vyOld = 0;
            balls[i].vxOld = 0;
        }
    }

    moveVault(keys);

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            *quit = true;
        }
    }
}

void processNameInput(SDL_Event *event)
{
    if (event->type == SDL_KEYDOWN)
    {
        if (event->key.keysym.sym == SDLK_RETURN)
        {
            enteringName = false;
            showMenu = true;
            saveHighScore(playerName, currentScore);
            printf("Player Name: %s, Score: %d\n", playerName, currentScore);
        }
        else if (event->key.keysym.sym == SDLK_BACKSPACE && nameIndex > 0)
        {
            playerName[--nameIndex] = '\0';
        }
        else if (nameIndex < MAX_NAME_LENGTH)
        {
            char key = (char)event->key.keysym.sym;
            if ((key >= 'a' && key <= 'z') || (key >= 'A' && key <= 'Z'))
            {
                playerName[nameIndex++] = key;
                playerName[nameIndex] = '\0';
            }
        }
    }
}
