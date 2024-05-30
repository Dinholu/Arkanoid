#include "harmful.h"

void initializeHarmfuls()
{
    for (int i = 0; i < MAX_HARMFULS; i++)
    {
        harmfuls[i].x = 0;
        harmfuls[i].y = 0;
        harmfuls[i].vx = 0;
        harmfuls[i].vy = 0;
        harmfuls[i].isActive = false;
        harmfuls[i].type = 0;
        harmfuls[i].randonElongationTime = 0;
        harmfuls[i].isFalling = false;
        harmfuls[i].initialY = 0; // Nouvelle variable pour la position initiale en Y
        harmfuls[i].time = 0;
        harmfuls[i].height = 0; // Nouvelle variable pour la hauteur
        harmfuls[i].elongationTime = 0;
        harmfuls[i].amplitude = 20; // Amplitude initiale de la sinusoïde
        harmfuls[i].isSinusoidal = false;
        harmfuls[i].animationFrame = 0;
        harmfuls[i].animationTime = 0;
        harmfuls[i].maxSteps = 1;
        harmfuls[i].isDestroying = false; // Nouvelle variable pour suivre la destruction
        harmfuls[i].destroyAnimationFrame = 0;
        harmfuls[i].destroyAnimationTime = 0;
    }
}

void generateHarmfuls()
{
    printf("Current harmfuls type: %d\n", currentHarmfulsType);
    if (currentHarmfulsType > 3)
    {
        currentHarmfulsType = 1;
    }
    for (int i = 0; i < MAX_HARMFULS; i++)
    {
        if (!harmfuls[i].isActive)
        {
            harmfuls[i].x = 355;
            harmfuls[i].y = Y_WALLS + srcTopWall.h;
            harmfuls[i].initialY = harmfuls[i].y; // Enregistrer la position initiale en Y
            harmfuls[i].vx = 1;                   // Déplacement initial à droite
            harmfuls[i].vy = 1;
            harmfuls[i].isActive = true;
            harmfuls[i].type = currentHarmfulsType;
            harmfuls[i].isFalling = true;
            harmfuls[i].amplitude = 2; // Réinitialiser l'amplitude
            harmfuls[i].time = 0;      // Réinitialiser le temps
            harmfuls[i].height = 0;    // Réinitialiser la hauteur
            harmfuls[i].elongationTime = 0;
            harmfuls[i].randonElongationTime = 10.0f + (rand() % 6);
            harmfuls[i].isSinusoidal = false;
            harmfuls[i].animationFrame = 0;
            harmfuls[i].animationTime = 0;
            switch (currentHarmfulsType)
            {
            case 1:
                harmfuls[i].maxSteps = 8;
                break;
            case 2:
                harmfuls[i].maxSteps = 8;
                break;
            case 3:
                harmfuls[i].maxSteps = 11;
                break;
            default:
                harmfuls[i].maxSteps = 1;
                break;
            }
            break;
        }
    }
}

SDL_Rect getHarmfulSrcRect(int type, int frame)
{
    switch (type)
    {
    case 1:
        return (SDL_Rect){HARMFUL_2.x + frame * 32, HARMFUL_2.y, HARMFUL_2.w, HARMFUL_2.h};
    case 2:
        return (SDL_Rect){HARMFUL_3.x + frame * 32, HARMFUL_3.y, HARMFUL_3.w, HARMFUL_3.h};
    case 3:
        return (SDL_Rect){HARMFUL_5.x + frame * 32, HARMFUL_5.y, HARMFUL_5.w, HARMFUL_5.h};
    default:
        return (SDL_Rect){0, 0, 32, 32}; // Default rectangle
    }
}

void addRandomHarmfuls()
{
    if (rand() % 120 < 1)
    {
        generateHarmfuls();
    }
}

void handleHarmfulCollisions()
{
    SDL_Rect vaultRect = {x_vault, win_surf->h - 32, vault_width, srcVault.h};
    for (int i = 0; i < MAX_HARMFULS; i++)
    {
        if (harmfuls[i].isActive)
        {
            SDL_Rect harmfulRect = {harmfuls[i].x, harmfuls[i].y, 32, 32};

            if (isCollision(vaultRect, harmfulRect))
            {
                currentScore += 1000;
                harmfuls[i].isActive = false;
                harmfuls[i].isDestroying = true; // Trigger destruction animation
                harmfuls[i].destroyAnimationFrame = 0;
                harmfuls[i].destroyAnimationTime = 0;
            }

            if (harmfuls[i].y > win_surf->h)
            {
                harmfuls[i].isActive = false;
            }

            for (int j = 0; j < MAX_BALLS; j++)
            {
                if (balls[j].isActive)
                {
                    SDL_Rect ballRect = {balls[j].x, balls[j].y, srcBall.w, srcBall.h};
                    if (isCollision(ballRect, harmfulRect))
                    {
                        currentScore += 100;
                        balls[j].vx *= -1;
                        balls[j].vy *= -1;
                        harmfuls[i].isActive = false;
                        harmfuls[i].isDestroying = true;
                        harmfuls[i].destroyAnimationFrame = 0;
                        harmfuls[i].destroyAnimationTime = 0;
                        break;
                    }
                }
            }
        }
    }
}

void clearHarmfuls()
{
    for (int i = 0; i < MAX_HARMFULS; i++)
    {
        harmfuls[i].isActive = false;
    }
}