#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#define NUM_BRICKS_PER_ROW 16
#define BRICK_WIDTH 32
#define BRICK_HEIGHT 16
#define NUM_ROWS 16
#define FIRST_LINE 1
#define NUM_BRICKS (NUM_BRICKS_PER_ROW * NUM_ROWS)

#define BRICK(COIN, LIGNE)                                                   \
    {                                                                        \
        (FIRST_LINE * COIN), (FIRST_LINE * LIGNE), BRICK_WIDTH, BRICK_HEIGHT \
    }

#define WHITE_BRICK (SDL_Rect) BRICK(0, 0)
#define ORANGE_BRICK (SDL_Rect) BRICK(32, 0)
#define BLUE1_BRICK (SDL_Rect) BRICK(64, 0)
#define GREEN1_BRICK (SDL_Rect) BRICK(96, 0)
#define BLUE2_BRICK (SDL_Rect) BRICK(128, 0)
#define GREEN2_BRICK (SDL_Rect) BRICK(160, 0)
#define RED_BRICK (SDL_Rect) BRICK(0, 16)
#define BLUE3_BRICK (SDL_Rect) BRICK(32, 16)
#define PINK_BRICK (SDL_Rect) BRICK(64, 16)
#define YELLOW_BRICK (SDL_Rect) BRICK(96, 16)
#define RED2_BRICK (SDL_Rect) BRICK(128, 16)
#define BLUE4_BRICK (SDL_Rect) BRICK(156, 16)
#define GREY_BRICK (SDL_Rect) BRICK(0, 32)
#define GOLD_BRICK (SDL_Rect) BRICK(0, 48)

// slow down ball
#define S_BONUS (SDL_Rect) BRICK(256, 0) // pour animer ca doit faire (288, 0) puis (320, 0) puis (352, 0) et cela 8 x puis reprendre a (256, 0)

// catch and fire
#define C_BONUS (SDL_Rect) BRICK(256, 16)
// laser beam
#define L_BONUS (SDL_Rect) BRICK(256, 32)
// enlarge vault
#define E_BONUS (SDL_Rect) BRICK(256, 48)
// split ball
#define D_BONUS (SDL_Rect) BRICK(256, 64)
// wrap level
#define B_BONUS (SDL_Rect) BRICK(256, 80)
// add life
#define P_BONUS (SDL_Rect) BRICK(256, 96)
#define HARMFUL_2 \
    (SDL_Rect) { 0, 256, 32, 32 }
#define HARMFUL_3 \
    (SDL_Rect) { 0, 288, 32, 32 }
#define HARMFUL_5 \
    (SDL_Rect) { 0, 320, 32, 32 }
#define HARMFUL_EXPLOSITION \
    (SDL_Rect) { 0, 384, 32, 32 }
// Si on augmente de niveau penser a modifier la constante ci dessous <-----
#define NUM_LEVELS 33
#define BALL_SPEED_INCREMENT 1.0 // Speed increment when hitting a brick
#define MAX_BALLS 3
#define VIE_MAX 5
#define MAX_LASERS 10
#define MAX_BONUSES 10
#define MAX_NAME_LENGTH 3
#define MAX_HIGHSCORE 10
#define MAX_BACKGROUND 5
#define MAX_HARMFULS 10

struct Harmful
{
    double x;
    double y;
    double vx;
    double vy;
    bool isActive;
    int type;
    bool isFalling;
    double time;
    double initialY;
    double amplitude;
} harmfuls[MAX_HARMFULS];

struct Bonus
{
    double x;
    double y;
    double vy;
    bool isActive;
    int type;
    int animationFrame;   // Frame actuelle de l'animation
    double animationTime; // Temps écoulé depuis la dernière frame
};

typedef struct
{
    char name[MAX_NAME_LENGTH + 1];
    int score;
} HighScore;

struct Bonus bonuses[MAX_BONUSES];

struct Laser
{
    double x;
    double y;
    double vy;
    bool isActive;
    bool hasTouchedBrick;
    bool isAnimating;
    int animationFrame;
    Uint64 lastFrameTime;
} lasers[MAX_LASERS];

struct Ball
{
    double x;
    double y;
    double vx;
    double vy;
    bool isActive;
    bool isAttached;
    Uint64 attachTime;
    int relative;
} ball;

struct Ball balls[MAX_BALLS];

struct Brick
{
    double x;
    double y;
    char type;
    int touched;
    int scoreValue;
    bool isVisible;
    bool isDestructible;
    bool isAnimating;   // New field to track if the brick is animating
    int animationFrame; // New field to track the current animation frame
    Uint64 lastFrameTime;
};

typedef struct Level
{
    int bricks[NUM_ROWS][NUM_BRICKS_PER_ROW];
} Level;

struct Brick brick[NUM_BRICKS];

Uint64 prev, now;
Level levels[NUM_LEVELS];

SDL_Window *pWindow = NULL;
SDL_Surface *win_surf = NULL;

SDL_Surface *plancheSprites = NULL;
SDL_Surface *gameSprites = NULL;
SDL_Surface *asciiSprites = NULL;
SDL_Surface *menuSprites = NULL;
SDL_Surface *topWallSprites = NULL;
SDL_Surface *leftWallSprites = NULL;
SDL_Surface *rightWallSprites = NULL;
// ici, la variable x permet de changer le fond en commençant par 64 jusqu'a 320 en incrémentant a chaque fois par 64
// Pour l'ecran gameOver, incrémenter la variable y par 64 (version sombre du sprite actuel)
SDL_Rect srcBackground = {0, 128, 64, 64};
// ------------------------------------------------------------------------------------------------------------------
SDL_Rect srcBall = {0, 96, 24, 24};
SDL_Rect srcVault = {384, 160, 82, 16};
SDL_Rect srcBrick;
SDL_Rect destVault;
SDL_Rect srcLogo = {0, 0, 192, 42};
SDL_Rect srcVaus = {0, 50, 192, 90};
SDL_Rect srcLeftLaser = {0, 80, 16, 20};
SDL_Rect srcRightLaser = {16, 80, 16, 20};

// Mur
SDL_Rect srcTopWall = {22, 0, 512, 22};
SDL_Rect srcEdgeWall = {0, 0, 22, 650};
const int Y_WALLS = 144;

// variable pour la brique grise
int touched = 2;
int x_vault;
int vault_width;
int currentLevel = 1;
int currentScore = 0;
int activeBallCount = 1;

// test gameOver

char playerName[MAX_NAME_LENGTH + 1] = "";
bool enteringName = false;
bool isGameOver = false;
bool showMenu = true;
int nameIndex = 0;
// ---------------

// bonus enlarge le vaisseau
bool isVaultEnlarged = false;
bool isEnlarging = false;
bool isShrinking = false;
int enlargeSteps = 5;
int currentStep = 0;
Uint64 enlargeStartTime = 0;
double enlargeDuration = 0.5;      // Durée de l'agrandissement en secondes
double enlargedHoldDuration = 5.0; // Durée pendant laquelle le vaisseau reste agrandi en secondes
double shrinkDuration = 0.5;       // Durée de la réduction en secondes

// bonus laser beam
bool isLaserBeam = false;
// bonus catch and fire
Uint64 attachTime = 0;
int releaseCount = 0;
int currentLife = 3;
double delta_t;
double ballSpeedIncrement = BALL_SPEED_INCREMENT;
const int FPS = 60;
double max_speed = 8.0;

// Variable pour savoir si la touche V a été pressée donc a enlever quand ca sera fait par collision avec le bonus
bool vWasPressed = false;
// Variable pour savoir si la touche N a été pressée donc a enlever quand ca sera fait par collision avec le bonus
bool nwasPressed = false;
// Variable pour savoir si la touche M a été pressée donc a enlever quand ca sera fait par collision avec le bonus
bool mWasPressed = false;

bool spaceWasPressed = false;
// test background
int backgroundChange = 0;
bool isCollision(SDL_Rect rect1, SDL_Rect rect2)
{
    return !(rect1.x + rect1.w < rect2.x ||
             rect1.x > rect2.x + rect2.w ||
             rect1.y + rect1.h < rect2.y ||
             rect1.y > rect2.y + rect2.h);
}

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

void moveAndRenderLasers(SDL_Surface *gameSprites, SDL_Rect *srcLeftLaser, SDL_Rect *srcRightLaser, SDL_Surface *win_surf)
{
    for (int i = 0; i < MAX_LASERS; i++)
    {
        if (lasers[i].isActive)
        {
            lasers[i].y += lasers[i].vy;

            // Vérifier les collisions avec les briques
            for (int j = 0; j < NUM_BRICKS; j++)
            {
                if (brick[j].isVisible)
                {
                    SDL_Rect laserRect = {lasers[i].x, lasers[i].y, srcLeftLaser->w, srcLeftLaser->h};
                    SDL_Rect brickRect = {brick[j].x + srcEdgeWall.w, brick[j].y + srcTopWall.h + Y_WALLS, BRICK_WIDTH, BRICK_HEIGHT};

                    if (isCollision(laserRect, brickRect))
                    {
                        lasers[i].isAnimating = true;
                        lasers[i].animationFrame = 0;
                        lasers[i].lastFrameTime = SDL_GetPerformanceCounter();
                        lasers[i].hasTouchedBrick = true; // Marquer le laser comme ayant touché une brique
                        lasers[i].isActive = false;       // Désactiver le laser

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
                        break; // Sortir de la boucle dès qu'une collision est détectée
                    }
                }
            }

            // Désactiver le laser s'il sort de l'écran
            if (lasers[i].y < Y_WALLS + srcTopWall.h)
            {
                lasers[i].isActive = false;
            }
        }

        // Rendre le laser
        if (lasers[i].isActive || lasers[i].hasTouchedBrick)
        {
            SDL_Rect destLaser = {lasers[i].x, lasers[i].y, 0, 0};
            SDL_Rect srcLaser = (i % 2 == 0) ? *srcLeftLaser : *srcRightLaser;

            if (lasers[i].isAnimating)
            {
                double elapsed = (SDL_GetPerformanceCounter() - lasers[i].lastFrameTime) / (double)SDL_GetPerformanceFrequency();
                if (elapsed > 0.1)
                {
                    lasers[i].animationFrame++;
                    lasers[i].lastFrameTime = SDL_GetPerformanceCounter();
                    if (lasers[i].animationFrame >= 3)
                    { // 3 frames d'animation
                        lasers[i].isAnimating = false;
                        lasers[i].hasTouchedBrick = false; // Réinitialiser hasTouchedBrick après l'animation
                    }
                }
                srcLaser.y += lasers[i].animationFrame * 16;
            }
            SDL_BlitSurface(gameSprites, &srcLaser, win_surf, &destLaser);
        }
    }
}

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
        harmfuls[i].isFalling = false;
        harmfuls[i].initialY = 0; // Nouvelle variable pour la position initiale en Y
        harmfuls[i].time = 0;
        harmfuls[i].amplitude = 20; // Amplitude initiale de la sinusoïde
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
            releaseCount = 1;
            balls[i].vy = -5;
            balls[i].vx = -1;
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
// TODO: remplacer les nombres magiques pour que cette fonction soit applicable peu importe la taille des vaisseaux.
// 24 correspond à la largeur de la balle en SDL_Rect
// 32 correspond à la position qu'on voudrait positionner le vaisseau
// 128 correspond à la taille du vaisseau
// Ici x_vault nous indique la position relative du vaisseau sur l'affichage
void vaultCollision(struct Ball *ball)
{
    if ((ball->y + srcBall.h > win_surf->h - 32) && (ball->x + srcBall.w > x_vault) && (ball->x < x_vault + vault_width))
    {
        ball->relative = (x_vault + (vault_width / 2) - (ball->x + (srcBall.w / 2)));

        if (releaseCount > 0)
        {
            ball->isAttached = true;
            ball->attachTime = SDL_GetPerformanceCounter();
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

void resetAllBonuses()
{
    isLaserBeam = false;
    isEnlarging = false;
    releaseCount = 0;
}

void clearBonuses()
{
    for (int i = 0; i < MAX_BONUSES; i++)
    {
        bonuses[i].isActive = false;
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
                    brick[i].touched--;
                    if (brick[i].touched == 0)
                    {
                        brick[i].isVisible = false;
                        currentScore += brick[i].scoreValue;
                        printf("Score: %d\n", currentScore);

                        // Generate bonus
                        int randValue = rand() % 9;
                        if (randValue < 6)
                        {
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

void generateHarmfuls()
{
    for (int i = 0; i < MAX_HARMFULS; i++)
    {
        if (!harmfuls[i].isActive)
        {
            harmfuls[i].x = 355;
            harmfuls[i].y = Y_WALLS + srcTopWall.h;
            harmfuls[i].initialY = harmfuls[i].y; // Enregistrer la position initiale en Y
            harmfuls[i].vx = 2;                   // Déplacement initial à droite
            harmfuls[i].vy = 0;
            harmfuls[i].isActive = true;
            harmfuls[i].type = rand() % 3 + 2;
            harmfuls[i].isFalling = false;
            harmfuls[i].amplitude = 20; // Réinitialiser l'amplitude
            harmfuls[i].time = 0;       // Réinitialiser le temps
            break;
        }
    }
}
void moveAndRenderHarmfuls(SDL_Surface *gameSprites, SDL_Surface *win_surf)
{
    for (int i = 0; i < MAX_HARMFULS; i++)
    {
        if (harmfuls[i].isActive)
        {
            if (!harmfuls[i].isFalling)
            {
                harmfuls[i].x += harmfuls[i].vx;

                // Vérifier les collisions avec les bords de l'écran
                if (harmfuls[i].x < srcEdgeWall.w || harmfuls[i].x + 32 > win_surf->w - srcEdgeWall.w)
                {
                    harmfuls[i].vx *= -1; // Changer de direction en cas de collision avec les bords
                }

                // Vérifier les collisions avec les briques
                bool canFall = true;
                for (int j = 0; j < NUM_BRICKS; j++)
                {
                    if (brick[j].isVisible)
                    {
                        SDL_Rect harmfulRect = {harmfuls[i].x, harmfuls[i].y, 32, 32};
                        SDL_Rect brickRect = {brick[j].x + srcEdgeWall.w, brick[j].y + srcTopWall.h + Y_WALLS, BRICK_WIDTH, BRICK_HEIGHT};

                        if (isCollision(harmfulRect, brickRect))
                        {
                            harmfuls[i].vx *= -1; // Changer de direction en cas de collision avec une brique
                            canFall = false;
                            break;
                        }

                        // Vérifier si le harmful peut tomber
                        SDL_Rect belowRect = {harmfuls[i].x, harmfuls[i].y + 32, 32, 30};
                        if (isCollision(belowRect, brickRect))
                        {
                            canFall = false;
                        }
                    }
                }

                // Si le harmful peut tomber, mettre à jour sa vitesse verticale
                if (canFall)
                {
                    harmfuls[i].isFalling = true;
                    harmfuls[i].vx = 0;
                    harmfuls[i].vy = 2;
                }
            }
            else
            {
                harmfuls[i].y += harmfuls[i].vy;

                // Vérifier les collisions avec les briques pendant la chute
                for (int j = 0; j < NUM_BRICKS; j++)
                {
                    if (brick[j].isVisible)
                    {
                        SDL_Rect harmfulRect = {harmfuls[i].x, harmfuls[i].y, 32, 32};
                        SDL_Rect brickRect = {brick[j].x + srcEdgeWall.w, brick[j].y + srcTopWall.h + Y_WALLS, BRICK_WIDTH, BRICK_HEIGHT};

                        if (isCollision(harmfulRect, brickRect))
                        {
                            harmfuls[i].y = brickRect.y - 32;            // Ajuster la position pour rester au-dessus de la brique
                            harmfuls[i].isFalling = false;               // Recommencer le déplacement horizontal
                            harmfuls[i].vx = (rand() % 2 == 0) ? 2 : -2; // Déplacement aléatoire gauche/droite
                            break;
                        }
                    }
                }
            }

            // Vérifier si le harmful sort de l'écran
            if (harmfuls[i].y > win_surf->h)
            {
                harmfuls[i].isActive = false;
            }

            // Rendre le harmful
            if (harmfuls[i].isActive)
            {
                SDL_Rect srcHarmful = (harmfuls[i].type == 2) ? HARMFUL_2 : (harmfuls[i].type == 3) ? HARMFUL_3
                                                                                                    : HARMFUL_5;
                SDL_Rect destHarmful = {harmfuls[i].x, harmfuls[i].y, srcHarmful.w, srcHarmful.h};
                SDL_BlitSurface(gameSprites, &srcHarmful, win_surf, &destHarmful);
            }
        }
    }
}

void addRandomHarmfuls()
{
    if (rand() % 120 < 1) // Environ 0.83% de chance d'ajouter un harmful chaque frame
    {
        generateHarmfuls();
    }
}

void moveAndRenderBonuses(SDL_Surface *gameSprites, SDL_Surface *win_surf)
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

            // Vérifier si le bonus sort de l'écran
            if (bonuses[i].y > win_surf->h)
            {
                bonuses[i].isActive = false;
            }

            // Rendre le bonus
            if (bonuses[i].isActive)
            {
                SDL_Rect srcBonus;
                int frameOffset = bonuses[i].animationFrame * 32;
                switch (bonuses[i].type)
                {
                case 1:
                    srcBonus = (SDL_Rect){256 + frameOffset, 0, 32, 16};
                    break;
                case 2:
                    srcBonus = (SDL_Rect){256 + frameOffset, 16, 32, 16};
                    break;
                case 3:
                    srcBonus = (SDL_Rect){256 + frameOffset, 32, 32, 16};
                    break;
                case 4:
                    srcBonus = (SDL_Rect){256 + frameOffset, 48, 32, 16};
                    break;
                case 5:
                    srcBonus = (SDL_Rect){256 + frameOffset, 64, 32, 16};
                    break;
                case 6:
                    srcBonus = (SDL_Rect){256 + frameOffset, 80, 32, 16};
                    break;
                case 7:
                    srcBonus = (SDL_Rect){256 + frameOffset, 96, 32, 16};
                    break;
                default:
                    srcBonus = (SDL_Rect){256 + frameOffset, 0, 32, 16};
                    break;
                }
                SDL_Rect destBonus = {
                    bonuses[i].x + srcEdgeWall.w, bonuses[i].y + Y_WALLS + srcTopWall.h, srcBonus.w, srcBonus.h};
                SDL_BlitSurface(gameSprites, &srcBonus, win_surf, &destBonus);
            }
        }
    }
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
void handleHarmfulCollisions()
{
    SDL_Rect vaultRect = {x_vault, win_surf->h - 32, vault_width, srcVault.h};
    for (int i = 0; i < MAX_HARMFULS; i++)
    {
        if (harmfuls[i].isActive)
        {
            SDL_Rect harmfulRect = {harmfuls[i].x, harmfuls[i].y, 32, 32};

            // Collision avec le vaisseau
            if (isCollision(vaultRect, harmfulRect))
            {
                currentScore += 1000;
                harmfuls[i].isActive = false;
            }

            // si le harmful touche le mur du bas
            if (harmfuls[i].y > win_surf->h)
            {
                harmfuls[i].isActive = false;
            }

            // Collision avec les balles
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
                        break;
                    }
                }
            }
        }
    }
}

void loadLevelFromFile(const char *filename, bool isEigth)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier %s\n", filename);
        exit(EXIT_FAILURE);
    }

    int row = 0;
    int col = 0;
    char brickType;

    if (isEigth)
    {
        touched++;
    }
    printf("Touched:%i\n", touched);

    while (fscanf(file, "%1c", &brickType) != EOF)
    {
        if (brickType == '\n')
        {
            continue; // Skip newline characters
        }
        brick[row * NUM_BRICKS_PER_ROW + col].type = brickType;
        brick[row * NUM_BRICKS_PER_ROW + col].x = col * BRICK_WIDTH;
        brick[row * NUM_BRICKS_PER_ROW + col].y = row * BRICK_HEIGHT;
        brick[row * NUM_BRICKS_PER_ROW + col].isVisible = (brickType != '-');
        brick[row * NUM_BRICKS_PER_ROW + col].isDestructible = (brickType != 'D');
        brick[row * NUM_BRICKS_PER_ROW + col].touched = (brickType == 'E') ? touched : 1;
        brick[row * NUM_BRICKS_PER_ROW + col].isAnimating = false;
        brick[row * NUM_BRICKS_PER_ROW + col].animationFrame = 0;
        brick[row * NUM_BRICKS_PER_ROW + col].lastFrameTime = SDL_GetPerformanceCounter();

        col++;
        if (col == NUM_BRICKS_PER_ROW)
        {
            col = 0;
            row++;
            if (row == NUM_ROWS)
            {
                fclose(file);
                return;
            }
        }
    }

    fclose(file);
}

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
// ----------- SCORE --------------//
int compareHighScores(const void *a, const void *b)
{
    HighScore *scoreA = (HighScore *)a;
    HighScore *scoreB = (HighScore *)b;
    return scoreB->score - scoreA->score;
}

void sortHighScores(HighScore highScores[], int count)
{
    qsort(highScores, count, sizeof(HighScore), compareHighScores);
}

void writeHighScores(HighScore highScores[], int count)
{
    FILE *file = fopen("highscores.txt", "w");
    if (file == NULL)
    {
        printf("Unable to open highscores.txt for writing.\n");
        return;
    }

    for (int i = 0; i < count; i++)
    {
        fprintf(file, "%s %d\n", highScores[i].name, highScores[i].score);
    }

    fclose(file);
}

void readHighScores(HighScore highScores[], int *count)
{
    FILE *file = fopen("highscores.txt", "r");
    if (file == NULL)
    {
        printf("Unable to open highscores.txt for reading.\n");
        *count = 0;
        return;
    }

    *count = 0;
    while (fscanf(file, "%s %d", highScores[*count].name, &highScores[*count].score) != EOF)
    {
        (*count)++;
        if (*count >= MAX_HIGHSCORE)
        {
            break;
        }
    }

    fclose(file);
}

void saveHighScore(const char *playerName, int score)
{
    HighScore highScores[MAX_HIGHSCORE + 1];
    int count;
    readHighScores(highScores, &count);

    // Ajouter le nouveau score
    strcpy(highScores[count].name, playerName);
    highScores[count].score = score;
    count++;

    // Trier les scores
    sortHighScores(highScores, count);

    // Limiter à MAX_HIGH_SCORES
    if (count > MAX_HIGHSCORE)
    {
        count = MAX_HIGHSCORE;
    }

    // Écrire les scores dans le fichier
    writeHighScores(highScores, count);
}

int getHighestScore()
{
    int highScore;
    int count;
    HighScore highScores[MAX_HIGHSCORE];
    readHighScores(highScores, &count);

    if (count == 0)
    {
        highScore = 0;
    }
    else
    {
        highScore = highScores[0].score;
    }

    if (currentScore > highScore)
    {
        highScore = currentScore;
    }

    return highScore;
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

// ----- STRING -----//
SDL_Rect charToSDLRect(char character)
{
    const int spriteWidth = 16;
    const int spriteHeight = 32;
    const int charsPerRow = 16;
    const int spriteSpacing = 32;

    if (character < ' ' || character > '~')
    {
        fprintf(stderr, "Character out of printable ASCII range: %d\n", character);
        return (SDL_Rect){0, 0, spriteWidth, spriteHeight};
    }

    int index = character - ' ';
    int x = (index % charsPerRow) * spriteSpacing;
    int y = (index / charsPerRow) * spriteHeight;

    SDL_Rect rect = {x, y, spriteWidth, spriteHeight};
    return rect;
}
//------- RENDERING -------//
void renderString(SDL_Surface *sprites, SDL_Surface *surface, const char *string, int startX, int startY)
{
    int x = startX;
    int y = startY;
    const int spacing = 1;

    SDL_Rect srcRect, destRect;
    while (*string)
    {
        srcRect = charToSDLRect(*string);
        destRect = (SDL_Rect){x, y, srcRect.w, srcRect.h};

        SDL_BlitSurface(sprites, &srcRect, surface, &destRect);

        x += srcRect.w + spacing;
        string++;
    }
}

void renderGameOverScreen(SDL_Surface *sprites, SDL_Rect *srcLogo, SDL_Surface *win_surf)
{
    SDL_FillRect(win_surf, NULL, SDL_MapRGB(win_surf->format, 0, 0, 0));
    SDL_Rect dest = {0, 128, srcLogo->w, srcLogo->h};
    dest.x = (win_surf->w - srcLogo->w) / 2;

    SDL_BlitSurface(sprites, srcLogo, win_surf, &dest);

    renderString(asciiSprites, win_surf, "GAME OVER", (win_surf->w - 128) / 2, 300);
    renderString(asciiSprites, win_surf, "ENTER NAME", (win_surf->w - 160) / 2, 350);
    renderString(asciiSprites, win_surf, playerName, (win_surf->w - 160) / 2, 400);
}

void renderCongratulationsScreen(SDL_Surface *sprites, SDL_Rect *srcLogo, SDL_Surface *win_surf)
{
    SDL_FillRect(win_surf, NULL, SDL_MapRGB(win_surf->format, 0, 0, 0));
    SDL_Rect dest = {0, 128, srcLogo->w, srcLogo->h};
    dest.x = (win_surf->w - srcLogo->w) / 2;

    SDL_BlitSurface(sprites, srcLogo, win_surf, &dest);

    renderString(asciiSprites, win_surf, "CONGRATULATIONS!", (win_surf->w - 256) / 2, 300);
    renderString(asciiSprites, win_surf, "ENTER NAME", (win_surf->w - 160) / 2, 350);
    renderString(asciiSprites, win_surf, playerName, (win_surf->w - 160) / 2, 400);
}

void renderMenu(SDL_Surface *sprites, SDL_Rect *srcLogo, SDL_Surface *win_surf)
{
    SDL_Rect dest = {0, 128, srcLogo->w, srcLogo->h};
    dest.x = (win_surf->w - srcLogo->w) / 2;
    SDL_BlitSurface(sprites, srcLogo, win_surf, &dest);

    int vausOffset = 128;
    SDL_Rect destVaus = {(win_surf->w - srcVaus.w) / 2, win_surf->h - srcVaus.h - vausOffset, srcVaus.w, srcVaus.h};
    SDL_BlitSurface(sprites, &srcVaus, win_surf, &destVaus);
}

void renderBackground(SDL_Surface *sprites, SDL_Rect *srcBackground, SDL_Surface *win_surf)
{
    SDL_Rect dest = {0, 0, 0, 0};
    for (int j = Y_WALLS + srcTopWall.h; j < win_surf->h; j += srcBackground->h)
    {
        for (int i = srcEdgeWall.w; i < win_surf->w; i += srcBackground->w)
        {
            dest.x = i;
            dest.y = j;
            SDL_BlitSurface(sprites, srcBackground, win_surf, &dest);
        }
    }
}

void changeBackground()
{
    if (backgroundChange < MAX_BACKGROUND)
    {
        srcBackground.x += 64;
        backgroundChange++;
    }
    else
    {
        srcBackground.x = 0;
        backgroundChange = 0;
    }
}

void renderBalls(SDL_Surface *sprites, SDL_Rect *srcBall, SDL_Surface *win_surf)
{
    for (int i = 0; i < MAX_BALLS; i++)
    {
        if (balls[i].isActive)
        {
            SDL_Rect destBall = {balls[i].x, balls[i].y, 0, 0};
            SDL_BlitSurface(sprites, srcBall, win_surf, &destBall);
            balls[i].x += balls[i].vx;
            balls[i].y += balls[i].vy;
        }
    }
}
void renderVault(SDL_Surface *sprites, SDL_Rect *srcVault, SDL_Surface *win_surf, int x_vault)
{
    destVault = (SDL_Rect){x_vault, win_surf->h - 32, 0, 0};
    SDL_BlitSurface(sprites, srcVault, win_surf, &destVault);
}

void attachBallToVault(struct Ball *ball, int x_vault)
{
    // Garder le x en fonction du vaisseau
    ball->x = x_vault + (vault_width / 2) - (srcBall.w / 2) - ball->relative;
    ball->y = destVault.y - srcBall.h;
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
        balls[i].isActive = false;
        balls[i].isAttached = false;
        balls[i].relative = 0;
        balls[i].attachTime = 0;
    }
    balls[0].isActive = true;
    balls[0].isAttached = true;
    balls[0].attachTime = SDL_GetPerformanceCounter();
}

void renderBricks(SDL_Surface *sprites, int num_bricks)
{
    for (int i = 0; i < num_bricks; i++)
    {
        if (brick[i].isVisible)
        {
            SDL_Rect destBrick = {brick[i].x + srcEdgeWall.w, brick[i].y + srcTopWall.h + Y_WALLS, 0, 0};
            if (brick[i].isAnimating)
            {
                double elapsed = (now - brick[i].lastFrameTime) / (double)SDL_GetPerformanceFrequency();
                if (elapsed > 0.1)
                {
                    brick[i].animationFrame = (brick[i].animationFrame + 1) % 5; // Assuming 5 frames of animation
                    brick[i].lastFrameTime = now;
                    if (brick[i].animationFrame == 0)
                    {
                        brick[i].isAnimating = false;
                    }
                }
                int x = 0;
                switch (brick[i].type)
                {
                case 'E':
                    x = 32;
                    break;
                case 'D':
                    x = 48;
                    break;
                default:
                    break;
                }
                srcBrick = (SDL_Rect){(brick[i].animationFrame + 1) * BRICK_WIDTH, x, BRICK_WIDTH, BRICK_HEIGHT};
            }
            else
            {

                switch (brick[i].type)
                {

                case 'W': // White
                    srcBrick = WHITE_BRICK;
                    brick[i].scoreValue = 50;
                    break;
                case 'Y': // Yellow
                    srcBrick = YELLOW_BRICK;
                    brick[i].scoreValue = 120;
                    break;
                case 'B': // Blue1
                    srcBrick = BLUE1_BRICK;
                    brick[i].scoreValue = 70;
                    break;
                case 'G': // Green1
                    srcBrick = GREEN1_BRICK;
                    brick[i].scoreValue = 80;
                    break;
                case 'b': // Blue2
                    srcBrick = BLUE2_BRICK;
                    brick[i].scoreValue = 100;
                    break;
                case 'O': // Orange
                    srcBrick = ORANGE_BRICK;
                    brick[i].scoreValue = 60;
                    break;
                case 'R': // Red
                    srcBrick = RED_BRICK;
                    brick[i].scoreValue = 90;
                    break;
                case 'L': // bLue3
                    srcBrick = BLUE3_BRICK;
                    brick[i].scoreValue = 120;
                    break;
                case 'P': // Pink
                    srcBrick = PINK_BRICK;
                    brick[i].scoreValue = 110;
                    break;
                case 'E': // grEy
                    srcBrick = GREY_BRICK;
                    brick[i].scoreValue = 50 * currentLevel;
                    break;
                case 'D': // golD
                    srcBrick = GOLD_BRICK;
                    break;
                default:
                    continue;
                }
            }
            SDL_BlitSurface(sprites, &srcBrick, win_surf, &destBrick);
        }
    }
}

void renderInfo(SDL_Surface *sprites, int value, char *label, int startX, int startY)
{
    char *string = malloc(sizeof(*string) * 256);
    sprintf(string, "%s%d", label, value);
    renderString(sprites, win_surf, string, startX, startY);
    free(string);
}

void showHighScores(SDL_Surface *win_surf, SDL_Surface *asciiSprites)
{
    SDL_FillRect(win_surf, NULL, SDL_MapRGB(win_surf->format, 0, 0, 0));

    HighScore highScores[MAX_HIGHSCORE];
    int count;
    readHighScores(highScores, &count);

    renderString(asciiSprites, win_surf, "HIGH SCORES", (win_surf->w - 160) / 2, 100);

    for (int i = 0; i < count; i++)
    {
        char scoreText[256];
        sprintf(scoreText, "%s %d", highScores[i].name, highScores[i].score);
        renderString(asciiSprites, win_surf, scoreText, 50, 150 + i * 40);
    }

    SDL_UpdateWindowSurface(pWindow);

    // Wait for a key press to return to the menu
    bool waiting = true;
    SDL_Event event;
    while (waiting)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                exit(EXIT_SUCCESS);
            }
            if (event.type == SDL_KEYDOWN)
            {
                SDL_FillRect(win_surf, NULL, SDL_MapRGB(win_surf->format, 0, 0, 0));
                waiting = false;
            }
        }
    }
}

void showOptionsMenu(SDL_Window *pWindow, SDL_Surface *win_surf)
{
    SDL_FillRect(win_surf, NULL, SDL_MapRGB(win_surf->format, 0, 0, 0));
    bool inMenu = true;
    SDL_Event event;

    int optionWidth = 160;
    int startOptionX = (win_surf->w - optionWidth) / 2;

    while (inMenu)
    {
        renderMenu(menuSprites, &srcLogo, win_surf);
        renderString(asciiSprites, win_surf, "1. START", startOptionX, srcLogo.h + 192);
        renderString(asciiSprites, win_surf, "2. HIGH SCORES", startOptionX, srcLogo.h + 256);
        renderString(asciiSprites, win_surf, "3. QUIT", startOptionX, srcLogo.h + 320);

        SDL_UpdateWindowSurface(pWindow);

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                exit(EXIT_SUCCESS);
            }
            if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym)
                {
                case SDLK_1:
                    inMenu = false;
                    showMenu = false;
                    break;
                case SDLK_2:
                    showHighScores(win_surf, asciiSprites);
                    break;
                case SDLK_3:
                    exit(EXIT_SUCCESS);
                    break;
                default:
                    break;
                }
            }
        }
    }
}

void loadCurrentLevel(bool isEigth)
{
    char filename[20];
    sprintf(filename, "level%d.txt", currentLevel);
    loadLevelFromFile(filename, isEigth);
}

void processCongratulationsInput(SDL_Event *event)
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

void nextLevel()
{
    changeBackground();
    clearBonuses();
    resetAllBonuses();
    currentLevel++;
    if (currentLevel >= NUM_LEVELS)
    {
        printf("Félicitations! Vous avez terminé tous les niveaux!\n");
        enteringName = true;
        showMenu = false;
        isGameOver = true;
        return;
    }
    initializeBalls();
    initializeLasers();
    initializeBonuses();
    initializeHarmfuls();
    max_speed = max_speed + 2.0;
    loadCurrentLevel(((currentLevel) % 8 == 0));
}

void resetGame()
{
    currentLife = 3;
    currentScore = 0;
    currentLevel = 1;
    max_speed = 8.0;
    ballSpeedIncrement = BALL_SPEED_INCREMENT;
    attachTime = SDL_GetPerformanceCounter();
    initializeBalls();
    initializeLasers();
    initializeBonuses();
    loadCurrentLevel(((currentLevel) % 8 == 0));
}

void initializeSDL()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
    {
        fprintf(stderr, "SDL Initialization failed: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    pWindow = SDL_CreateWindow("Arkanoid", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 556, 800, SDL_WINDOW_SHOWN);
    if (!pWindow)
    {
        fprintf(stderr, "Window creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    win_surf = SDL_GetWindowSurface(pWindow);
    plancheSprites = SDL_LoadBMP("./sprites.bmp");
    gameSprites = SDL_LoadBMP("./Arkanoid_sprites.bmp");
    asciiSprites = SDL_LoadBMP("./Arkanoid_ascii_sprites.bmp");
    menuSprites = SDL_LoadBMP("./Arkanoid_menu_sprites.bmp");
    topWallSprites = SDL_LoadBMP("./edge_top.bmp");
    leftWallSprites = SDL_LoadBMP("./edge_left.bmp");
    rightWallSprites = SDL_LoadBMP("./edge_right.bmp");

    if (!plancheSprites || !gameSprites || !asciiSprites || !menuSprites || !topWallSprites || !leftWallSprites || !rightWallSprites)
    {
        fprintf(stderr, "Sprite loading failed: %s\n", SDL_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    SDL_SetColorKey(plancheSprites, SDL_TRUE, 0);
    SDL_SetColorKey(gameSprites, SDL_TRUE, 0);
    SDL_SetColorKey(asciiSprites, SDL_TRUE, 0);
    SDL_SetColorKey(menuSprites, SDL_TRUE, 0);
    SDL_SetColorKey(topWallSprites, SDL_TRUE, 0);
    SDL_SetColorKey(leftWallSprites, SDL_TRUE, 0);
    SDL_SetColorKey(rightWallSprites, SDL_TRUE, 0);

    x_vault = (win_surf->w - srcVault.w) / 2;
    vault_width = srcVault.w;
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
    nwasPressed = true;
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

void renderWall(SDL_Surface *sprites, SDL_Rect *srcWall, int positionX, int positionY, int width, int height)
{
    SDL_Rect destWall = {positionX, positionY, width, height};
    SDL_BlitSurface(sprites, srcWall, win_surf, &destWall);
}

void renderAllWalls()
{
    renderWall(leftWallSprites, &srcEdgeWall, 0, Y_WALLS, srcEdgeWall.w, srcEdgeWall.h);
    renderWall(rightWallSprites, &srcEdgeWall, win_surf->w - srcEdgeWall.w, Y_WALLS, srcEdgeWall.w, srcEdgeWall.h);
    renderWall(topWallSprites, &srcTopWall, srcEdgeWall.w, Y_WALLS, srcTopWall.w, srcTopWall.h);
}

void render()
{
    SDL_FillRect(win_surf, NULL, SDL_MapRGB(win_surf->format, 0, 0, 0));
    renderBackground(gameSprites, &srcBackground, win_surf);
    renderVault(gameSprites, &srcVault, win_surf, x_vault);
    for (int i = 0; i < MAX_BALLS; i++)
    {
        if (balls[i].isAttached)
        {
            attachBallToVault(&balls[i], x_vault);
        }
    }
    renderAllWalls();
    renderBalls(plancheSprites, &srcBall, win_surf);
    renderBricks(gameSprites, NUM_BRICKS);
    renderInfo(asciiSprites, currentScore, "", 16, 10);
    renderInfo(asciiSprites, currentLife, "HP ", win_surf->w - 96, 10);
    renderInfo(asciiSprites, currentLevel, "LEVEL ", win_surf->w / 2 - 64, 10);         // a clean
    renderInfo(asciiSprites, getHighestScore(), "HI-SCORE ", win_surf->w / 2 - 64, 92); // a clean
    moveAndRenderLasers(gameSprites, &srcLeftLaser, &srcRightLaser, win_surf);
    moveAndRenderBonuses(gameSprites, win_surf);
    moveAndRenderHarmfuls(gameSprites, win_surf);
    handleCollisions();
    handleHarmfulCollisions();
    handleBonusCollision();
}

void processInput(bool *quit)
{
    SDL_Event event;
    SDL_PumpEvents();
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_SPACE])
    {

        for (int i = 0; i < MAX_BALLS; i++)
        {
            if (balls[i].isAttached)
            {
                balls[i].isAttached = false;
                balls[i].vy = -5;
                balls[i].vx = -1;
                releaseCount--;
                break; // Libérer une seule balle à chaque appui
            }
            break;
        }
    }

    // BONUS SPLIT BALL (D_BONUS)
    if (keys[SDL_SCANCODE_B] && activeBallCount == 1)
    {
        splitBall();
    }
    if (keys[SDL_SCANCODE_N] == 0)
    {
        nwasPressed = false;
    }
    // BONUS WRAP LEVEL (B_BONUS)
    if (keys[SDL_SCANCODE_N])
    {
        if (!nwasPressed)
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
    // BONUS FIRE LASER (L_BONUS)
    if (keys[SDL_SCANCODE_M])
    {
        if (!mWasPressed && isLaserBeam)
        {
            fireLaser();
        }
    }

    if (keys[SDL_SCANCODE_M] == 0)
    {
        mWasPressed = false;
    }
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
            balls[i].vy = -5;
            balls[i].vx = -1;
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

void updateDeltaTime()
{
    now = SDL_GetPerformanceCounter();
    delta_t = 1.0 / FPS - (double)(now - prev) / (double)SDL_GetPerformanceFrequency();
    prev = now;
    if (delta_t > 0)
        SDL_Delay((Uint32)(delta_t * 1000));
    prev = SDL_GetPerformanceCounter();
}

void mainGameLoop()
{
    bool quit = false;
    prev = SDL_GetPerformanceCounter();

    while (!quit)
    {
        if (showMenu)
        {
            showOptionsMenu(pWindow, win_surf);
            resetGame();
            showMenu = false;
            isGameOver = false;
            enteringName = false;
            nameIndex = 0;
            playerName[0] = '\0';
        }
        if (!isGameOver)
        {
            processInput(&quit);
            updateVaultEnlargement();

            if (allBricksInvisible())
            {
                nextLevel();
            }

            if (currentLife <= 0)
            {
                printf("Life: 0, GAME OVER!\n");
                isGameOver = true;
                enteringName = true;
            }
            addRandomHarmfuls(); // Ajouter des harmfuls aléatoirement
            render();
        }

        if (enteringName)
        {
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT)
                {
                    quit = true;
                }
                else
                {
                    processNameInput(&event);
                }
            }

            if (currentLevel >= NUM_LEVELS)
            {
                renderCongratulationsScreen(menuSprites, &srcLogo, win_surf);
            }
            else
            {
                renderGameOverScreen(menuSprites, &srcLogo, win_surf);
            }
        }

        SDL_UpdateWindowSurface(pWindow);
        updateDeltaTime();
    }
}

int main(int argc, char **argv)
{
    initializeSDL();
    showOptionsMenu(pWindow, win_surf);
    resetGame();

    mainGameLoop();

    SDL_Quit();
    return 0;
}
