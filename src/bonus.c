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
        // degage les balles qui sont attachees
        if (balls[i].isActive && balls[i].isAttached)
        {
            balls[i].isAttached = false;
            releaseCount = 0;
            balls[i].vy = -5;
            balls[i].vx = -0;
        }
    }

    struct Ball tempBalls[MAX_BALLS];
    int tempActiveBallCount = 0;
    for (int i = 0; i < MAX_BALLS; i++)
    {
        if (balls[i].isActive)
        {
            if (tempActiveBallCount + 3 <= MAX_BALLS)
            {
                tempBalls[tempActiveBallCount] = balls[i];

                tempBalls[tempActiveBallCount + 1] = balls[i];
                tempBalls[tempActiveBallCount + 1].vx = balls[i].vx * cos(M_PI / 6) - balls[i].vy * sin(M_PI / 6);
                tempBalls[tempActiveBallCount + 1].vy = balls[i].vx * sin(M_PI / 6) + balls[i].vy * cos(M_PI / 6);

                tempBalls[tempActiveBallCount + 2] = balls[i];
                tempBalls[tempActiveBallCount + 2].vx = balls[i].vx * cos(-M_PI / 6) - balls[i].vy * sin(-M_PI / 6);
                tempBalls[tempActiveBallCount + 2].vy = balls[i].vx * sin(-M_PI / 6) + balls[i].vy * cos(-M_PI / 6);

                tempBalls[tempActiveBallCount + 1].isActive = true;
                tempBalls[tempActiveBallCount + 2].isActive = true;

                tempActiveBallCount += 3;
            }
            else
            {
                break;
            }
        }
    }

    for (int i = 0; i < tempActiveBallCount && i < MAX_BALLS; i++)
    {
        balls[i] = tempBalls[i];
    }

    activeBallCount = tempActiveBallCount < MAX_BALLS ? tempActiveBallCount : MAX_BALLS;
}

void resetAllBonuses()
{
    isLaserBeam = false;
    isEnlarging = false;
    isShrinking = false;
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
        if (fabs(balls[i].vx) > MIN_SPEED || fabs(balls[i].vy) > MIN_SPEED)
        {
            printf("Ralentissement des balles!\n");

            balls[i].vx /= 2;
            balls[i].vy /= 2;
        }
        ballSpeedIncrement = 0;
    }
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

                bonuses[i].isActive = false;
                currentScore += 100;
                switch (bonuses[i].type)
                {
                case 1:
                    resetAllBonuses();
                    slowDownBall();
                    break;
                case 2:
                    resetAllBonuses();
                    CatchAndFire();
                    break;
                case 3:
                    resetAllBonuses();
                    isLaserBeam = true;
                    break;
                case 4:
                    if (isVaultEnlarged || isEnlarging)
                    {
                        break;
                    }
                    else
                    {
                        resetAllBonuses();
                        enlargeVault();
                    }
                    break;
                case 5:
                    resetAllBonuses();
                    splitBall();
                    break;
                case 6:
                    wraplevel();
                    break;
                case 7:
                    resetAllBonuses();
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

void moveLasers()
{
    for (int i = 0; i < MAX_LASERS; i++)
    {
        if (lasers[i].isActive)
        {
            lasers[i].y += lasers[i].vy;
            laserCollisions();
            if (lasers[i].y < Y_WALLS + srcTopWall.h)
            {
                lasers[i].isActive = false;
            }
        }
    }
}

void laserCollisions()
{
    for (int i = 0; i < MAX_LASERS; i++)
    {
        for (int j = 0; j < NUM_BRICKS; j++)
        {
            if (brick[j].isVisible)
            {
                SDL_Rect laserRect = {lasers[i].x, lasers[i].y, srcLeftLaser.w, srcLeftLaser.h};
                SDL_Rect brickRect = {brick[j].x + srcEdgeWall.w, brick[j].y + srcTopWall.h + Y_WALLS, BRICK_WIDTH, BRICK_HEIGHT};

                if (isCollision(laserRect, brickRect))
                {
                    lasers[i].isAnimating = true;
                    lasers[i].animationFrame = 0;
                    lasers[i].lastFrameTime = SDL_GetPerformanceCounter();
                    lasers[i].hasTouchedBrick = true;
                    lasers[i].isActive = false;

                    if (brick[j].isDestructible)
                    {
                        brick[j].touched--;
                        if (brick[j].touched == 0)
                        {
                            brick[j].isVisible = false;
                            currentScore += brick[j].scoreValue;
                        }
                        else
                        {
                            brick[j].isAnimating = true;
                            brick[j].animationFrame = 0;
                            brick[j].lastFrameTime = SDL_GetPerformanceCounter();
                        }
                    }
                    else
                    {
                        brick[j].isAnimating = true;
                        brick[j].animationFrame = 0;
                        brick[j].lastFrameTime = SDL_GetPerformanceCounter();
                    }
                    break;
                }
            }
        }
    }
}
