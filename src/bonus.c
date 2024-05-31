#include "bonus.h"

struct Bonus bonuses[MAX_BONUSES];
struct Laser lasers[MAX_LASERS];

SDL_Rect srcLeftLaser = {0, 80, 16, 20};
SDL_Rect srcRightLaser = {16, 80, 16, 20};

bool isVaultEnlarged = false;
bool isEnlarging = false;
bool isShrinking = false;
bool isLaserBeam = false;
int enlargeSteps = 5;
int currentStep = 0;
Uint64 enlargeStartTime = 0;
double enlargeDuration = 0.5;
double shrinkDuration = 0.5;

void fireLaser()
{
    mWasPressed = true;
    for (int i = 0; i < MAX_LASERS; i += 2)
    {
        if (!lasers[i].isActive && !lasers[i + 1].isActive)
        {
            // Laser gauche
            lasers[i].x = x_vault + 10;     // Positionner le laser sur le côté gauche du vaisseau
            lasers[i].y = destVault.y - 20; // Positionner le laser juste au-dessus du vaisseau
            lasers[i].isActive = true;

            // Laser droit
            lasers[i + 1].x = x_vault + vault_width - 26; // Positionner le laser sur le côté droit du vaisseau
            lasers[i + 1].y = destVault.y - 20;           // Positionner le laser juste au-dessus du vaisseau
            lasers[i + 1].isActive = true;

            break;
        }
    }
}

void initializeBonuses()
{
    for (int i = 0; i < MAX_BONUSES; i++)
    {
        bonuses[i].x = 0;
        bonuses[i].y = 0;
        bonuses[i].vy = 200;
        bonuses[i].isActive = false;
        bonuses[i].type = 0;
        bonuses[i].animationFrame = 0;
        bonuses[i].animationTime = 0;
    }
}

void initializeLasers()
{
    for (int i = 0; i < MAX_LASERS; i++)
    {
        lasers[i].x = 0;
        lasers[i].y = 0;
        lasers[i].vy = -10;
        lasers[i].hasTouchedBrick = false;
        lasers[i].isActive = false;
        lasers[i].isAnimating = false;
        lasers[i].animationFrame = 0;
        lasers[i].lastFrameTime = 0;
    }
}

void splitBall()
{
    for (int i = 0; i < MAX_BALLS; i++)
    {
        if (balls[i].isActive && balls[i].isAttached)
        {
            balls[i].isAttached = false;
            releaseCount = 0;
            balls[i].vy = -5;
            balls[i].vx = -0;
        }
    }

    if (activeBallCount == 1)
    {
        balls[1] = balls[0];
        balls[2] = balls[0];
        balls[1].vx = balls[0].vx * cos(M_PI / 6) - balls[0].vy * sin(M_PI / 6);
        balls[1].vy = balls[0].vx * sin(M_PI / 6) + balls[0].vy * cos(M_PI / 6);
        balls[2].vx = balls[0].vx * cos(-M_PI / 6) - balls[0].vy * sin(-M_PI / 6);
        balls[2].vy = balls[0].vx * sin(-M_PI / 6) + balls[0].vy * cos(-M_PI / 6);
        balls[1].isActive = true;
        balls[2].isActive = true;
        activeBallCount = 3;
    }
}

void resetAllBonuses()
{
    isLaserBeam = false;
    isEnlarging = false;
    releaseCount = 0;
    ballSpeedIncrement = BALL_SPEED_INCREMENT;
}

void clearBonuses()
{
    for (int i = 0; i < MAX_BONUSES; i++)
    {
        bonuses[i].isActive = false;
    }
}

void enlargeVault()
{
    if (!isEnlarging && !isVaultEnlarged && !isShrinking)
    {
        printf("Agrandissement du vaisseau!\n");
        isEnlarging = true;
        currentStep = 0;
        enlargeStartTime = SDL_GetPerformanceCounter();
    }
}

void updateVaultEnlargement()
{
    Uint64 now = SDL_GetPerformanceCounter();
    double elapsed = (now - enlargeStartTime) / (double)SDL_GetPerformanceFrequency();

    if (isEnlarging)
    {
        double stepDuration = enlargeDuration / enlargeSteps;
        double t = elapsed / stepDuration;
        if (t >= 1.0 && currentStep < enlargeSteps)
        {
            vault_width = srcVault.w;

            srcVault.y += 16;         // Déplacer vers la ligne du sprite agrandi
            srcVault.w += 9;          // Augmenter la largeur du sprite
            vault_width = srcVault.w; // Mettre à jour la largeur du vaisseau
            currentStep++;
            enlargeStartTime = SDL_GetPerformanceCounter(); // Redémarrer la minuterie pour la prochaine étape
        }

        if (currentStep == enlargeSteps)
        {
            isVaultEnlarged = true;
            enlargeStartTime = SDL_GetPerformanceCounter(); // Démarrer la minuterie pour la durée de maintien
        }
    }
    else if (isVaultEnlarged && !isEnlarging)
    {
        isVaultEnlarged = false;
        isShrinking = true;
        currentStep = 0;
        enlargeStartTime = SDL_GetPerformanceCounter(); // Redémarrer la minuterie pour la réduction
    }
    else if (isShrinking)
    {
        double stepDuration = shrinkDuration / enlargeSteps;
        double t = elapsed / stepDuration;

        if (t >= 1.0 && currentStep < enlargeSteps)
        {
            srcVault.y -= 16;         // Revenir à la ligne du sprite original
            srcVault.w -= 9;          // Réduire la largeur du sprite
            vault_width = srcVault.w; // Mettre à jour la largeur du vaisseau
            currentStep++;
            enlargeStartTime = SDL_GetPerformanceCounter(); // Redémarrer la minuterie pour la prochaine étape
        }

        if (currentStep == enlargeSteps)
        {
            isShrinking = false;
        }
    }
}

void addLife()
{
    if (currentLife <= VIE_MAX)
    {
        currentLife++;
    }
}

void wraplevel()
{
    nWasPressed = true;
    currentScore += 10000;
    nextLevel();
}

void slowDownBall()
{
    for (int i = 0; i < MAX_BALLS; i++)
    {
        if (balls[i].isActive)
        {
            balls[i].vx /= 2;
            balls[i].vy /= 2;
        }
    }
    ballSpeedIncrement = 0;
}

void CatchAndFire()
{
    releaseCount = 5;
}

void handleBonusCollision()
{
    SDL_Rect vaultRect = {x_vault, win_surf->h - 32, vault_width, srcVault.h};
    for (int i = 0; i < MAX_BONUSES; i++)
    {
        if (bonuses[i].isActive)
        {
            SDL_Rect bonusRect = {bonuses[i].x + srcEdgeWall.w, bonuses[i].y + Y_WALLS + srcTopWall.h, 32, 16};
            // Assurez-vous que la taille est correcte

            if (isCollision(vaultRect, bonusRect))
            {
                resetAllBonuses();
                bonuses[i].isActive = false;
                currentScore += 100;
                // Appliquer l'effet du bonus
                switch (bonuses[i].type)
                {
                case 1:
                    slowDownBall();
                    break;
                case 2:
                    CatchAndFire();
                    break;
                case 3:
                    isLaserBeam = true;
                    break;
                case 4:
                    enlargeVault();
                    break;
                case 5:
                    splitBall();
                    break;
                case 6:
                    wraplevel();
                    break;
                case 7:
                    addLife();
                    break;
                default:
                    break;
                }
            }
        }
    }
}

void moveBonuses()
{
    for (int i = 0; i < MAX_BONUSES; i++)
    {
        if (bonuses[i].isActive)
        {
            bonuses[i].y += bonuses[i].vy * delta_t; // Mise à jour de la position en fonction de delta_t
            bonuses[i].animationTime += delta_t;     // Mise à jour du temps d'animation

            if (bonuses[i].animationTime >= 0.1)
            {
                bonuses[i].animationFrame = (bonuses[i].animationFrame + 1) % 8; // Boucler sur 8 frames d'animation
                bonuses[i].animationTime = 0;
            }

            if (bonuses[i].y > win_surf->h)
            {
                bonuses[i].isActive = false;
            }
        }
    }
}