#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define NUM_BRICKS_PER_ROW 20
#define BRICK_WIDTH 30
#define BRICK_HEIGHT 13
#define NUM_ROWS 15
#define FIRST_LINE 1
#define NUM_BRICKS (NUM_BRICKS_PER_ROW * NUM_ROWS)
#define WHITE_BRICK {FIRST_LINE,FIRST_LINE,BRICK_WIDTH,BRICK_HEIGHT}
#define ORANGE_BRICK {(FIRST_LINE * 32),FIRST_LINE,BRICK_WIDTH,BRICK_HEIGHT}
#define BLUE1_BRICK {(FIRST_LINE * 64),FIRST_LINE,BRICK_WIDTH,BRICK_HEIGHT}
#define GREEN1_BRICK {(FIRST_LINE * 96),FIRST_LINE,BRICK_WIDTH,BRICK_HEIGHT}
#define BLUE2_BRICK {(FIRST_LINE * 128),FIRST_LINE,BRICK_WIDTH,BRICK_HEIGHT}
#define GREEN2_BRICK {(FIRST_LINE * 156),FIRST_LINE,BRICK_WIDTH,BRICK_HEIGHT}
#define RED_BRICK {FIRST_LINE,(FIRST_LINE * 16),BRICK_WIDTH,BRICK_HEIGHT}
#define BLUE3_BRICK {(FIRST_LINE * 32),(FIRST_LINE * 16),BRICK_WIDTH,BRICK_HEIGHT}
#define PINK_BRICK {(FIRST_LINE * 64),(FIRST_LINE * 16),BRICK_WIDTH,BRICK_HEIGHT}
#define YELLOW_BRICK {(FIRST_LINE * 96),(FIRST_LINE * 16),BRICK_WIDTH,BRICK_HEIGHT}
#define RED2_BRICK {(FIRST_LINE * 128),(FIRST_LINE * 16),BRICK_WIDTH,BRICK_HEIGHT}
#define BLUE4_BRICK {(FIRST_LINE * 156),(FIRST_LINE * 16),BRICK_WIDTH,BRICK_HEIGHT}
#define GREY_BRICK {FIRST_LINE,(FIRST_LINE * 32),BRICK_WIDTH,BRICK_HEIGHT}
#define S_BONUS {(FIRST_LINE * 256),FIRST_LINE,BRICK_WIDTH,BRICK_HEIGHT}
#define C_BONUS {(FIRST_LINE * 256),(FIRST_LINE * 16),BRICK_WIDTH,BRICK_HEIGHT}
#define L_BONUS {(FIRST_LINE * 256),(FIRST_LINE * 32),BRICK_WIDTH,BRICK_HEIGHT}
#define E_BONUS {(FIRST_LINE * 256),(FIRST_LINE * 48),BRICK_WIDTH,BRICK_HEIGHT}
#define O_BONUS {(FIRST_LINE * 256),(FIRST_LINE * 64),BRICK_WIDTH,BRICK_HEIGHT}
#define D_BONUS {(FIRST_LINE * 256),(FIRST_LINE * 80),BRICK_WIDTH,BRICK_HEIGHT}
#define B_BONUS {(FIRST_LINE * 256),(FIRST_LINE * 96),BRICK_WIDTH,BRICK_HEIGHT}
#define P_BONUS {(FIRST_LINE * 256),(FIRST_LINE * 112),BRICK_WIDTH,BRICK_HEIGHT}

const int FPS = 60.0;

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

struct Brick brick[NUM_BRICKS];

bool ballIsAttached = false;
Uint64 prev, now; // timers
double delta_t;   // durée frame en ms
int x_vault;

SDL_Window *pWindow = NULL;
SDL_Surface *win_surf = NULL;
// 
SDL_Surface *plancheSprites = NULL;
SDL_Surface *gameSprites = NULL;
SDL_Surface *asciiSprites = NULL;

SDL_Rect srcBg = {0, 128, 96, 128}; // x,y, w,h (0,0) en haut a gauche
SDL_Rect srcBall = {0, 96, 24, 24};
SDL_Rect srcVaiss = {128, 0, 128, 32};
SDL_Rect srcBrick = C_BONUS;

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

void handleCollisions()
{
    // Walls collision
    if ((ball.x < 1) || (ball.x > (win_surf->w - 25)))
        ball.vx *= -1;
    if ((ball.y < 1) || (ball.y > (win_surf->h - 25)))
        ball.vy *= -1;

    // Vault collision
    if ((ball.y + 24 > win_surf->h - 32) && (ball.x + 24 > x_vault) && (ball.x < x_vault + 128))
    {
        double relativeCollisionX = (ball.x + 12) - (x_vault + 64);
        double normalizedRelativeCollisionX = relativeCollisionX / 64.0;

        double bounceAngle = normalizedRelativeCollisionX * M_PI / 3.0;
        double speed = sqrt(ball.vx * ball.vx + ball.vy * ball.vy);

        ball.vx = speed * sin(bounceAngle);
        ball.vy = -speed * cos(bounceAngle);
    }

    // Bottom wall collision
    if (ball.y > (win_surf->h - 25))
    {
        ball.x = x_vault + 52;
        ball.y = win_surf->h - 58;
        ball.vy = 0;
        ball.vx = 0;
        ballIsAttached = true;
    }

    // Brick collision
    for (int i = 0; i < NUM_BRICKS; i++)
    {
        if (brick[i].isVisible)
        {
            SDL_Rect ballRect = {ball.x, ball.y, 24, 24};
            SDL_Rect brickRect = {brick[i].x, brick[i].y, BRICK_WIDTH, BRICK_HEIGHT};

            if (isCollision(ballRect, brickRect))
            {
                brick[i].isVisible = false;

                double ballCenterX = ball.x + 12;
                double ballCenterY = ball.y + 12;

                double brickCenterX = brick[i].x + (BRICK_WIDTH / 2);
                double brickCenterY = brick[i].y + (BRICK_HEIGHT / 2);

                double dx = ballCenterX - brickCenterX;
                double dy = ballCenterY - brickCenterY;

                double reflectionAngle = atan2(dy, dx);

                double speed = sqrt(ball.vx * ball.vx + ball.vy * ball.vy);
                ball.vx = speed * cos(reflectionAngle);
                ball.vy = speed * sin(reflectionAngle);

                break;
            }
        }
    }
}


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
    while (fscanf(file, "%1d", &brick[row * NUM_BRICKS_PER_ROW + col].type) == 1)
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

void init()
{
    pWindow = SDL_CreateWindow("Arknoid", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 600, 600, SDL_WINDOW_SHOWN);
    win_surf = SDL_GetWindowSurface(pWindow);
    plancheSprites = SDL_LoadBMP("./sprites.bmp");
    gameSprites = SDL_LoadBMP("./Arkanoid_sprites.bmp");
    asciiSprites = SDL_LoadBMP("./Arkanoid_ascii.bmp");

    SDL_SetColorKey(plancheSprites, true, 0); // 0: 00/00/00 noir -> transparent
    SDL_SetColorKey(gameSprites, true, 0);    // 0: 00/00/00 noir -> transparent

    ball.x = win_surf->w / 2;
    ball.y = win_surf->h / 2;
    ball.vx = 2.0;
    ball.vy = 2.8;

    loadLevelFromFile("level1.txt");
    now = SDL_GetPerformanceCounter();
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

    ball.x += ball.vx; // / delta_t;
    ball.y += ball.vy; // / delta_t;

    handleCollisions(); // Handle all collisions

    dest.x = x_vault;
    dest.y = win_surf->h - 32;
    SDL_BlitSurface(plancheSprites, &srcVaiss, win_surf, &dest);

    if (ballIsAttached)
    {
        ball.x = x_vault + 52;
        ball.y = win_surf->h - 58;
    }

    for (int i = 0; i < NUM_BRICKS; i++)
    {
        if (brick[i].isVisible)
        {
            SDL_Rect dstBrick = {brick[i].x, brick[i].y, 0, 0};
            SDL_BlitSurface(gameSprites, &srcBrick, win_surf, &dstBrick);
        }
    }
}

int main(int argc, char **argv)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
    {
        return 1;
    }

    init();

    bool quit = false;
    SDL_Event event;

    while (!quit)
    {
        if (allBricksInvisible())
        {
            printf("Congratulations! You've won the game!\n");
            quit = true;
        }
        SDL_PumpEvents();
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        if (keys[SDL_SCANCODE_SPACE] && ball.vy == 0)
        {
            ballIsAttached = false;
            ball.vy = -1.4;
            ball.vx = -1.0;
        }

        if (keys[SDL_SCANCODE_LEFT])
            x_vault -= 10;
        if (keys[SDL_SCANCODE_RIGHT])
            x_vault += 10;
        if (keys[SDL_SCANCODE_ESCAPE])
            quit = true;

        if (x_vault < 0)
            x_vault = 0;
        if (x_vault > win_surf->w - 128)
            x_vault = win_surf->w - 128;

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                quit = true;
            }
        }

        draw();

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
