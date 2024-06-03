#include "ball.h"

struct Ball balls[MAX_BALLS];
SDL_Rect srcBall = {63, 65, 16, 16};
Uint64 attachTime = 0;
int releaseCount = 0;
int activeBallCount = 1;
double ballSpeedIncrement = BALL_SPEED_INCREMENT;
double max_speed = 7.0;

void vaultCollision(struct Ball *ball)
{
    if ((ball->y + srcBall.h > win_surf->h - 32) && (ball->x + srcBall.w > x_vault) && (ball->x < x_vault + vault_width))
    {
        ball->relative = (x_vault + (vault_width / 2) - (ball->x + (srcBall.w / 2)));

        if (releaseCount > 0)
        {
            ball->isAttached = true;
            ball->attachTime = SDL_GetPerformanceCounter();
            ball->vxOld = ball->vx;
            ball->vyOld = ball->vy;
            ball->vx = 0;
            ball->vy = 0;
        }
        else
        {
            double relativeCollisionX = (ball->x + 12) - (x_vault + vault_width / 2);
            double normalizedRelativeCollisionX = relativeCollisionX / (vault_width / 2);

            double bounceAngle = normalizedRelativeCollisionX * M_PI / 3.0;
            double speed = sqrt(ball->vx * ball->vx + ball->vy * ball->vy);

            ball->vx = speed * sin(bounceAngle);
            ball->vy = -speed * cos(bounceAngle);
        }
    }
}

void handleBallProperty(struct Ball *ball, SDL_Rect brickRect)
{
    double ballCenterX = ball->x + srcBall.w / 2;
    double ballCenterY = ball->y + srcBall.h / 2;
    double brickCenterX = brickRect.x + (BRICK_WIDTH / 2);
    double brickCenterY = brickRect.y + (BRICK_HEIGHT / 2);
    double dx = ballCenterX - brickCenterX;
    double dy = ballCenterY - brickCenterY;
    double reflectionAngle = atan2(dy, dx);
    double speed = sqrt(ball->vx * ball->vx + ball->vy * ball->vy);

    ball->vx = speed * cos(reflectionAngle);
    ball->vy = speed * sin(reflectionAngle);

    if (speed <= max_speed)
    {
        ball->vx += (ball->vx > 0) ? ballSpeedIncrement : -ballSpeedIncrement;
        ball->vy += (ball->vy > 0) ? ballSpeedIncrement : -ballSpeedIncrement;
    }

    printf("Speed: %f\n", sqrt(ball->vx * ball->vx + ball->vy * ball->vy));
}

void handleCollisions()
{
    for (int i = 0; i < MAX_BALLS; i++)
    {
        if (balls[i].isActive)
        {
            wallCollision(&balls[i]);
            vaultCollision(&balls[i]);
            brickCollision(&balls[i]);
            defeatCollision(&balls[i]);
        }
    }
}

void brickCollision(struct Ball *ball)
{
    for (int i = 0; i < NUM_BRICKS; i++)
    {
        if (brick[i].isVisible)
        {
            SDL_Rect ballRect = {ball->x, ball->y, srcBall.w, srcBall.h};
            SDL_Rect brickRect = {
                brick[i].x + srcEdgeWall.w, brick[i].y + srcTopWall.h + Y_WALLS, BRICK_WIDTH, BRICK_HEIGHT};

            if (isCollision(ballRect, brickRect))
            {
                // si E cest grey OU D cest gold j'incrémente le srcBrick 5x vers la droite et je reviens a la valeur de base
                handleBallProperty(ball, brickRect);
                if (brick[i].isDestructible)
                {
                    printf("Brick touched: %i\n", i);
                    brick[i].touched--;
                    if (brick[i].touched == 0)
                    {
                        brick[i].isVisible = false;
                        currentScore += brick[i].scoreValue;
                        printf("Score: %d\n", currentScore);

                        // Generate bonus
                        int randValue = rand() % 100;
                        if (randValue < 20) // 20% chance to generate a bonus
                        {
                            int bonusTypeRand = rand() % 100;
                            int bonusType;
                            if (bonusTypeRand < 2) // 5% chance for rose bonus
                            {
                                bonusType = 1; // Assuming 1 is the rose bonus type
                            }
                            else if (bonusTypeRand < 25) // 20% chance for other specific bonus
                            {
                                bonusType = 2; // Another bonus type
                            }
                            else if (bonusTypeRand < 45) // 20% chance for another bonus
                            {
                                bonusType = 3; // Another bonus type
                            }
                            else if (bonusTypeRand < 65) // 20% chance for another bonus
                            {
                                bonusType = 4; // Another bonus type
                            }
                            else if (bonusTypeRand < 85) // 20% chance for another bonus
                            {
                                bonusType = 5; // Another bonus type
                            }
                            else // 15% chance for the remaining bonus types
                            {
                                bonusType = rand() % 2 + 6; // Assuming the last two bonus types
                            }
                            for (int j = 0; j < MAX_BONUSES; j++)
                            {
                                if (!bonuses[j].isActive)
                                {
                                    bonuses[j].x = brick[i].x + BRICK_WIDTH / 2;
                                    bonuses[j].y = brick[i].y + BRICK_HEIGHT / 2;
                                    bonuses[j].isActive = true;
                                    bonuses[j].type = rand() % 7 + 1;
                                    break;
                                }
                            }
                        }
                        break;
                    }
                    else
                    {
                        brick[i].isAnimating = true;
                        brick[i].animationFrame = 0;
                        brick[i].lastFrameTime = SDL_GetPerformanceCounter();
                    }
                }
                else
                {
                    brick[i].isAnimating = true;
                    brick[i].animationFrame = 0;
                    brick[i].lastFrameTime = SDL_GetPerformanceCounter();
                }
            }
        }
    }
}

void defeatCollision(struct Ball *ball)
{
    if (ball->y > (win_surf->h - 25))
    {
        ball->isActive = false;
        ball->isAttached = false;
        ball->relative = 0;
        activeBallCount--;
        if (activeBallCount == 0)
        {
            clearBonuses();
            currentLife--;
            resetAllBonuses();
            printf("Vies restantes: %d\n", currentLife);
            balls[0].isActive = true;
            balls[0].isAttached = true;
            balls[0].attachTime = SDL_GetPerformanceCounter();
            balls[0].vy = 0;
            balls[0].vx = 0;
            activeBallCount = 1;
        }
    }
}

void initializeBalls()
{
    activeBallCount = 1;
    for (int i = 0; i < MAX_BALLS; i++)
    {
        balls[i].x = 0;
        balls[i].y = destVault.y - srcBall.h;
        balls[i].vx = 0;
        balls[i].vy = 0;
        balls[i].vxOld = 0;
        balls[i].vyOld = 0;
        balls[i].isActive = false;
        balls[i].isAttached = false;
        balls[i].relative = 0;
        balls[i].attachTime = 0;
    }
    balls[0].isActive = true;
    balls[0].isAttached = true;
    balls[0].attachTime = SDL_GetPerformanceCounter();
}

void wallCollision(struct Ball *ball)
{
    // Collision mur latéral
    if (ball->x < srcEdgeWall.w)
    {
        ball->x = srcEdgeWall.w; // Reset position to avoid getting stuck
        ball->vx *= -1;
    }
    else if (ball->x + srcBall.w > win_surf->w - srcEdgeWall.w)
    {
        ball->x = win_surf->w - srcEdgeWall.w - srcBall.w;
        ball->vx *= -1;
    }

    // Collision mur top
    if (ball->y < srcTopWall.h + Y_WALLS)
    {
        ball->y = srcTopWall.h + Y_WALLS;
        ball->vy *= -1;
    }
}

void attachBallToVault(struct Ball *ball, int x_vault)
{
    // Garder le x en fonction du vaisseau
    ball->x = x_vault + (vault_width / 2) - (srcBall.w / 2) - ball->relative;
    ball->y = destVault.y - srcBall.h;
}

void updateBallTrail(struct Ball *ball)
{
    if (!ball->isActive)
        return;

    for (int i = ball->trailLength - 1; i > 0; i--)
    {
        ball->trail[i] = ball->trail[i - 1];
    }

    ball->trail[0].x = ball->x;
    ball->trail[0].y = ball->y;
    ball->trail[0].alpha = 255;

    ball->trailLength = (ball->trailLength < 50) ? ball->trailLength + 1 : 50;

    for (int i = 1; i < ball->trailLength; i++)
    {
        ball->trail[i].alpha = (Uint8)(ball->trail[i - 1].alpha * 0.75);
    }
}

void handleBallUpdates()
{
    for (int i = 0; i < MAX_BALLS; i++)
    {
        if (balls[i].isActive)
        {
            updateBallTrail(&balls[i]);
            balls[i].x += balls[i].vx;
            balls[i].y += balls[i].vy;
        }
    }
}
