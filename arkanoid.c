#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#define NUM_BRICKS_PER_ROW 20
#define BRICK_WIDTH 30
#define BRICK_HEIGHT 13
#define NUM_ROWS 15
#define FIRST_LINE 1
#define NUM_BRICKS (NUM_BRICKS_PER_ROW * NUM_ROWS)

#define BRICK(COIN, LIGNE)                                                   \
    {                                                                        \
        (FIRST_LINE * COIN), (FIRST_LINE * LIGNE), BRICK_WIDTH, BRICK_HEIGHT \
    }

#define WHITE_BRICK (SDL_Rect) BRICK(1, 1)
#define ORANGE_BRICK (SDL_Rect) BRICK(32, 1)
#define BLUE1_BRICK (SDL_Rect) BRICK(64, 1)
#define GREEN1_BRICK (SDL_Rect) BRICK(96, 1)
#define BLUE2_BRICK (SDL_Rect) BRICK(128, 1)
#define GREEN2_BRICK (SDL_Rect) BRICK(160, 1)
#define RED_BRICK (SDL_Rect) BRICK(1, 16)
#define BLUE3_BRICK (SDL_Rect) BRICK(32, 16)
#define PINK_BRICK (SDL_Rect) BRICK(64, 16)
#define YELLOW_BRICK (SDL_Rect) BRICK(96, 16)
#define RED2_BRICK (SDL_Rect) BRICK(128, 16)
#define BLUE4_BRICK (SDL_Rect) BRICK(156, 16)
#define GREY_BRICK (SDL_Rect) BRICK(1, 32)
#define S_BONUS (SDL_Rect) BRICK(1, 256)
#define C_BONUS (SDL_Rect) BRICK(256, 16)
#define L_BONUS (SDL_Rect) BRICK(256, 32)
#define E_BONUS (SDL_Rect) BRICK(256, 48)
#define O_BONUS (SDL_Rect) BRICK(256, 64)
#define D_BONUS (SDL_Rect) BRICK(256, 80)
#define B_BONUS (SDL_Rect) BRICK(256, 96)
#define P_BONUS (SDL_Rect) BRICK(256, 112)

// Si on augmente de niveau penser a modifier la constante ci dessous <-----
#define NUM_LEVELS 2
const int FPS = 60.0;
#define BALL_SPEED_INCREMENT 1.0; // Speed increment when hitting a brick

struct
{
    double x;
    double y;
    double vx;
    double vy;
} ball;

struct Brick
{
    double x;
    double y;
    int type;
    bool isVisible;
};

typedef struct Level
{
    int bricks[NUM_ROWS][NUM_BRICKS_PER_ROW];
} Level;

struct Brick brick[NUM_BRICKS];

double max_speed = 5.0;
bool ballIsAttached = false;
Uint64 prev, now;
double delta_t;
int x_vault;
double ballSpeedIncrement = BALL_SPEED_INCREMENT; // Track the speed increment
Level levels[NUM_LEVELS];
int currentLevel = 0;
int currentScore = 0;

SDL_Window *pWindow = NULL;
SDL_Surface *win_surf = NULL;

SDL_Surface *plancheSprites = NULL;
SDL_Surface *gameSprites = NULL;
SDL_Surface *asciiSprites = NULL;

SDL_Rect srcBg = {0, 128, 96, 128};
SDL_Rect srcBall = {0, 96, 24, 24};
SDL_Rect srcVaiss = {128, 0, 128, 32};
SDL_Rect srcBrick;
SDL_Rect asciiRects[10];

bool isCollision(SDL_Rect rect1, SDL_Rect rect2)
{
    return rect1.x < rect2.x + rect2.w &&
           rect1.x + rect1.w > rect2.x &&
           rect1.y < rect2.y + rect2.h &&
           rect1.y + rect1.h > rect2.y;
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

void wallCollision()
{
    if ((ball.x < 1) || (ball.x > (win_surf->w - 25)))
    {
        ball.vx *= -1;
    }

    if ((ball.y < 1) || (ball.y > (win_surf->h - 25)))
    {
        ball.vy *= -1;
    }
}

void vaultCollision()
{
    if ((ball.y + 24 > win_surf->h - 32) && (ball.x + 24 > x_vault) && (ball.x < x_vault + 128))
    {
        double relativeCollisionX = (ball.x + 12) - (x_vault + 64);
        double normalizedRelativeCollisionX = relativeCollisionX / 64.0;

        double bounceAngle = normalizedRelativeCollisionX * M_PI / 3.0;
        double speed = sqrt(ball.vx * ball.vx + ball.vy * ball.vy);

        ball.vx = speed * sin(bounceAngle);
        ball.vy = -speed * cos(bounceAngle);
    }
}

void defeatCollision()
{
    // TODO : Gérer la défaite et désincrémentation des vies
    if (ball.y > (win_surf->h - 25))
    {
        ball.x = x_vault + 52;
        ball.y = win_surf->h - 58;
        ball.vy = 0;
        ball.vx = 0;
        ballIsAttached = true;
    }
}

void handleBallProperty(int brickIndex)
{
    double ballCenterX = ball.x + 12;
    double ballCenterY = ball.y + 12;
    double brickCenterX = brick[brickIndex].x + (BRICK_WIDTH / 2);
    double brickCenterY = brick[brickIndex].y + (BRICK_HEIGHT / 2);
    double dx = ballCenterX - brickCenterX;
    double dy = ballCenterY - brickCenterY;

    double reflectionAngle = atan2(dy, dx);
    double speed = sqrt(ball.vx * ball.vx + ball.vy * ball.vy);
    ball.vx = speed * cos(reflectionAngle);
    ball.vy = speed * sin(reflectionAngle);

    if (speed <= max_speed)
    {
        ball.vx += (ball.vx > 0) ? ballSpeedIncrement : -ballSpeedIncrement;
        ball.vy += (ball.vy > 0) ? ballSpeedIncrement : -ballSpeedIncrement;
    }

    printf("Speed: %f\n", sqrt(ball.vx * ball.vx + ball.vy * ball.vy));
}
// TODO: Simplify this function + separate logic
void brickCollision()
{
    for (int i = 0; i < NUM_BRICKS; i++)
    {
        if (brick[i].isVisible)
        {
            SDL_Rect ballRect = {ball.x, ball.y, 24, 24};
            SDL_Rect brickRect = {brick[i].x, brick[i].y, BRICK_WIDTH, BRICK_HEIGHT};

            if (isCollision(ballRect, brickRect))
            {
                brick[i].isVisible = false;
                currentScore += 10;

                handleBallProperty(i);
                printf("Score: %d\n", currentScore);
                break;
            }
        }
    }
}

void handleCollisions()
{
    wallCollision();
    vaultCollision();
    defeatCollision();
    brickCollision();
}

// Fonction pour charger un niveau à partir d'un fichier
void loadLevelFromFile(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier %s\n", filename);
        exit(EXIT_FAILURE);
    }

    int row = 0;
    int col = 0;
    while (fscanf(file, "%1d", &brick[row * NUM_BRICKS_PER_ROW + col].type) != EOF)
    {
        brick[row * NUM_BRICKS_PER_ROW + col].x = col * BRICK_WIDTH;
        brick[row * NUM_BRICKS_PER_ROW + col].y = row * BRICK_HEIGHT;
        brick[row * NUM_BRICKS_PER_ROW + col].isVisible = brick[row * NUM_BRICKS_PER_ROW + col].type;

        printf("Loaded brick at (%f, %f) - Type: %d\n", brick[row * NUM_BRICKS_PER_ROW + col].x, brick[row * NUM_BRICKS_PER_ROW + col].y, brick[row * NUM_BRICKS_PER_ROW + col].type);

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
    if (keys[SDL_SCANCODE_LEFT])
        x_vault -= 10;
    if (keys[SDL_SCANCODE_RIGHT])
        x_vault += 10;

    if (x_vault < 0)
        x_vault = 0;
    if (x_vault > win_surf->w - 128)
        x_vault = win_surf->w - 128;
}

void setupAsciiRects() // A utilisre pour les chaines de caractères
{
    for (int i = 0; i < 10; i++)
    {
        asciiRects[i].x = i * 32;
        asciiRects[i].y = 32;
        asciiRects[i].w = 16;
        asciiRects[i].h = 32;
    }
}

void drawScore()
{
    int score = currentScore;
    int digits[10];
    int i = 0;

    if (score == 0)
    {
        digits[0] = 0;
        i = 1;
    }

    while (score > 0)
    {
        digits[i] = score % 10;
        score /= 10;
        i++;
    }

    int x = win_surf->w - 16 * i;
    for (int j = i - 1; j >= 0; j--)
    {
        SDL_Rect src = asciiRects[digits[j]];
        SDL_Rect dst = {x, 10, src.w, src.h};
        SDL_BlitSurface(asciiSprites, &src, win_surf, &dst);
        x += 16;
    }
}

void draw()
{
    SDL_Rect dest = {0, 0, 0, 0};
    for (int j = 0; j < win_surf->h; j += 128)
        for (int i = 0; i < win_surf->w; i += 96)
        {
            dest.x = i;
            dest.y = j;
            SDL_BlitSurface(plancheSprites, &srcBg, win_surf, &dest);
        }

    SDL_Rect dstBall = {ball.x, ball.y, 0, 0};
    SDL_BlitSurface(plancheSprites, &srcBall, win_surf, &dstBall);

    ball.x += ball.vx;
    ball.y += ball.vy;

    handleCollisions(); // Handle all collisions

    dest.x = x_vault;
    dest.y = win_surf->h - 32;
    SDL_BlitSurface(plancheSprites, &srcVaiss, win_surf, &dest);

    if (ballIsAttached)
    {
        ball.x = x_vault + 52;
        ball.y = win_surf->h - 58;
    }
    // Draw each brick
    for (int i = 0; i < NUM_BRICKS; i++)
    {
        if (brick[i].isVisible)
        {
            SDL_Rect dstBrick = {brick[i].x, brick[i].y, 0, 0};

            switch (brick[i].type)
            {
            case 1:
                srcBrick = GREY_BRICK;
                break;
            case 2:
                srcBrick = ORANGE_BRICK;
                break;

            case 3:
                srcBrick = BLUE1_BRICK;
                break;
            case 4:
                srcBrick = GREEN1_BRICK;
                break;
            case 5:
                srcBrick = BLUE2_BRICK;
                break;
            case 6:
                srcBrick = GREEN2_BRICK;
                break;
            case 7:
                srcBrick = RED_BRICK;
                break;
            case 8:
                srcBrick = BLUE3_BRICK;
                break;
            case 9:
                srcBrick = PINK_BRICK;
                break;
            default:
                srcBrick = WHITE_BRICK;
                break;
            }

            SDL_BlitSurface(gameSprites, &srcBrick, win_surf, &dstBrick);
        }
    }

    drawScore();
}

// fonction pour que la balle soit accroché au vaisseau
void loadCurrentLevel()
{
    char filename[20];
    sprintf(filename, "level%d.txt", currentLevel + 1);
    loadLevelFromFile(filename);
}

// Fonction pour passer au niveau suivant
void nextLevel()
{
    currentLevel++;
    if (currentLevel >= NUM_LEVELS)
    {
        printf("Félicitations! Vous avez terminé tous les niveaux!\n");
        exit(EXIT_SUCCESS);
    }
    ballIsAttached = true;
    ball.vy = 0;
    ball.vx = 0;
    max_speed = max_speed + 2.0;
    loadCurrentLevel();
}

int main(int argc, char **argv)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
    {
        return 1;
    }

    pWindow = SDL_CreateWindow("Arknoid", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 600, 800, SDL_WINDOW_SHOWN);
    win_surf = SDL_GetWindowSurface(pWindow);
    plancheSprites = SDL_LoadBMP("./sprites.bmp");
    gameSprites = SDL_LoadBMP("./Arkanoid_sprites.bmp");
    asciiSprites = SDL_LoadBMP("./Arkanoid_ascii.bmp");
    setupAsciiRects();

    SDL_SetColorKey(plancheSprites, true, 0); // 0: 00/00/00 noir -> transparent
    SDL_SetColorKey(gameSprites, true, 0);    // 0: 00/00/00 noir -> transparent
    SDL_SetColorKey(asciiSprites, true, 0);
    // TODO : faire proprement
    ballIsAttached = true;
    ball.y = 0;
    ball.x = 0;

    loadCurrentLevel();

    bool quit = false;
    SDL_Event event;

    while (!quit)
    {
        if (allBricksInvisible())
        {
            printf("Congratulations! You've won the game!\n");
            nextLevel();
        }
        SDL_PumpEvents();
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        // Quand on meurt, on peut relancer la balle
        if (keys[SDL_SCANCODE_SPACE] && ball.vy == 0)
        {
            ballIsAttached = false;
            ball.vy = -1.4; // Vitesse de la balle réduite avant reprise.
            ball.vx = -1.0;
        }

        moveVault(keys);

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                quit = true;
            }
        }

        draw();
        // Update the window surface
        SDL_UpdateWindowSurface(pWindow);

        now = SDL_GetPerformanceCounter();
        delta_t = 1.0 / FPS - (double)(now - prev) / (double)SDL_GetPerformanceFrequency();
        prev = now;

        if (delta_t > 0)
            SDL_Delay((Uint32)(delta_t * 1000));
        prev = SDL_GetPerformanceCounter();
    }

    SDL_Quit();
    return 0;
}
