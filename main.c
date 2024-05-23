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
#define S_BONUS (SDL_Rect) BRICK(0, 256)
#define C_BONUS (SDL_Rect) BRICK(256, 16)
#define L_BONUS (SDL_Rect) BRICK(256, 32)
#define E_BONUS (SDL_Rect) BRICK(256, 48)
#define O_BONUS (SDL_Rect) BRICK(256, 64)
#define D_BONUS (SDL_Rect) BRICK(256, 80)
#define B_BONUS (SDL_Rect) BRICK(256, 96)
#define P_BONUS (SDL_Rect) BRICK(256, 112)
// Si on augmente de niveau penser a modifier la constante ci dessous <-----
#define NUM_LEVELS 2
#define BALL_SPEED_INCREMENT 1.0 // Speed increment when hitting a brick

struct Ball {
    double x;
    double y;
    double vx;
    double vy;
} ball;

struct Brick {
    double x;
    double y;
    int type;
    bool isVisible;
};

typedef struct Level {
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

SDL_Rect srcBackground = {64, 128, 64, 64};
SDL_Rect srcBall = {0, 96, 24, 24};
SDL_Rect srcVaisseau = {384, 160, 82, 16};
SDL_Rect srcBrick;
SDL_Rect asciiRects[10];
SDL_Rect srcLogo = {0,0,400,144};

int x_vault;
int vault_width;
int currentLevel = 0;
int currentScore = 0;
int currentLife = 3;
double delta_t;
double ballSpeedIncrement = BALL_SPEED_INCREMENT;
const int FPS = 60;
double max_speed = 8.0;
bool ballIsAttached = false;

bool isCollision(SDL_Rect rect1, SDL_Rect rect2)
{
    return rect1.x < rect2.x + rect2.w &&
           rect1.x + rect2.w > rect2.x &&
           rect1.y < rect2.y + rect2.h &&
           rect1.y + rect2.h > rect2.y;
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
// TODO: remplacer les nombres magiques pour que cette fonction soit applicable peu importe la taille des vaisseaux.
// 24 correspond à la largeur de la balle en SDL_Rect
// 32 correspond à la position qu'on vouudrait positionner le vaisseau
// 128 correspond à la taille du vaisseau
// Ici x_vault nous indique la position relative du vaisseau sur l'affichage
void vaultCollision()
{
    if ((ball.y + srcBall.h > win_surf->h - 32) && (ball.x + srcBall.w > x_vault) && (ball.x < x_vault + vault_width))
    {
        double relativeCollisionX = (ball.x + 12) - (x_vault + vault_width / 2);
        double normalizedRelativeCollisionX = relativeCollisionX / (vault_width / 2);

        double bounceAngle = normalizedRelativeCollisionX * M_PI / 3.0;
        double speed = sqrt(ball.vx * ball.vx + ball.vy * ball.vy);

        ball.vx = speed * sin(bounceAngle);
        ball.vy = -speed * cos(bounceAngle);
    }
}

void defeatCollision()
{
    if (ball.y > (win_surf->h - 25))
    {
        currentLife--;
        ball.x = x_vault + 52;
        ball.y = win_surf->h - 58;
        ball.vy = 0;
        ball.vx = 0;
        ballIsAttached = true;
    }
}

void handleBallProperty(int brickIndex)
{
    double ballCenterX = ball.x + srcBall.w / 2;
    double ballCenterY = ball.y + srcBall.h / 2;
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

void brickCollision()
{
    for (int i = 0; i < NUM_BRICKS; i++)
    {
        if (brick[i].isVisible)
        {
            SDL_Rect ballRect = {ball.x, ball.y, srcBall.w, srcBall.h};
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
    brickCollision();
    defeatCollision();
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
    if (x_vault > win_surf->w - srcVaisseau.w)
        x_vault = win_surf->w - srcVaisseau.w;
}

SDL_Rect charToSDLRect(char character)
{
    const int spriteWidth = 16;
    const int spriteHeight = 32;
    const int charsPerRow = 16;
    const int spriteSpacing = 32;

    if (character < ' ' || character > '~') {
        fprintf(stderr, "Character out of printable ASCII range: %d\n", character);
        return (SDL_Rect){0, 0, spriteWidth, spriteHeight};
    }

    int index = character - ' ';
    int x = (index % charsPerRow) * spriteSpacing;
    int y = (index / charsPerRow) * spriteHeight;

    SDL_Rect rect = {x, y, spriteWidth, spriteHeight};
    return rect;
}

void renderString(SDL_Surface* surface, SDL_Surface* spriteSheet, const char* string, int startX, int startY) 
{
    int x = startX;
    int y = startY;
    const int spacing = 1;

    SDL_Rect srcRect, destRect;
    while (*string) 
    {
        srcRect = charToSDLRect(*string);
        destRect = (SDL_Rect){x, y, srcRect.w, srcRect.h};

        SDL_BlitSurface(spriteSheet, &srcRect, surface, &destRect);

        x += srcRect.w + spacing;
        string++;
    }
}

void renderMenu(SDL_Surface* menuSprites, SDL_Rect* srcLogo, SDL_Surface* win_surf)
{
    SDL_Rect dest = {0, 128, srcLogo->w, srcLogo->h};
    dest.x = (win_surf->w - srcLogo->w) / 2;

    SDL_BlitSurface(menuSprites, srcLogo, win_surf, &dest);
}


void renderBackground(SDL_Surface* gameSprites, SDL_Rect* srcBackground, SDL_Surface* win_surf)
{
    SDL_Rect dest = {0, 0, 0, 0};
    for (int j = 0; j < win_surf->h; j += 64)
    {
        for (int i = 0; i < win_surf->w; i += 64)
        {
            dest.x = i;
            dest.y = j;
            SDL_BlitSurface(gameSprites, srcBackground, win_surf, &dest);
        }
    }
}

void renderBall(SDL_Surface* plancheSprites, SDL_Rect* srcBall, SDL_Surface* win_surf, struct Ball* ball)
{
    SDL_Rect destBall = {ball->x, ball->y, 0, 0};
    SDL_BlitSurface(plancheSprites, srcBall, win_surf, &destBall);

    ball->x += ball->vx;
    ball->y += ball->vy;
}

void renderVault(SDL_Surface* gameSprites, SDL_Rect* srcVaisseau, SDL_Surface* win_surf, int x_vault)
{
    SDL_Rect dest = {x_vault, win_surf->h - 32, 0, 0};
    vault_width = srcVaisseau->w;
    SDL_BlitSurface(gameSprites, srcVaisseau, win_surf, &dest);
}

// TODO: remplacer 52 par la moitie de la taille du vaisseau pour que la balle se positionne au centre du vaisseau
// 58 correspond à la hauteur ou le vaisseeau est positionné + largeur de la balle
void attachBallToVault(struct Ball* ball, int x_vault, int win_surf_height)
{
    ball->x = x_vault + (vault_width / 2) - 12;
    ball->y = win_surf_height - 58;
}

void renderBricks(SDL_Surface* gameSprites, SDL_Surface* win_surf, struct Brick bricks[], int num_bricks)
{
    for (int i = 0; i < num_bricks; i++)
    {
        if (bricks[i].isVisible)
        {
            SDL_Rect destBrick = {bricks[i].x, bricks[i].y, 0, 0};
            SDL_Rect srcBrick;

            switch (bricks[i].type)
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

            SDL_BlitSurface(gameSprites, &srcBrick, win_surf, &destBrick);
        }
    }
}

void renderInfo(SDL_Surface* win_surf, SDL_Surface* asciiSprites, int value, char* label, int startX, int startY)
{
    char* string = malloc (sizeof (*string) * 256);;
    sprintf(string, "%s:%d", label, value);
    renderString(win_surf, asciiSprites, string, startX, startY);
    free(string);
}

void render()
{
    renderBackground(gameSprites, &srcBackground, win_surf);
    renderBall(plancheSprites, &srcBall, win_surf, &ball);

    handleCollisions();

    renderVault(gameSprites, &srcVaisseau, win_surf, x_vault);

    if (ballIsAttached)
    {
        attachBallToVault(&ball, x_vault, win_surf->h);
    }

    renderBricks(gameSprites, win_surf, brick, NUM_BRICKS);
    renderInfo(win_surf, asciiSprites, currentScore, "SCORE", 16, 10); // A faire proprement le fait qu'il soit en haut à gauche de l'écran
    renderInfo(win_surf, asciiSprites, currentLife, "LIFE",win_surf->w - 116,10); // A faire proprement le fait qu'il soit en haut à droite de l'écran
    // TODO : Rajouter un render pour le highscore au milieu

}

void showOptionsMenu(SDL_Window *pWindow, SDL_Surface *win_surf)
{
    bool inMenu = true;
    SDL_Event event;

    int optionWidth = 128;
    int optionX = (win_surf->w - optionWidth) / 2;

    while (inMenu)
    {
        renderMenu(menuSprites, &srcLogo, win_surf);

        renderString(win_surf, asciiSprites, "1. START", optionX, srcLogo.h + 192);
        renderString(win_surf, asciiSprites, "2. QUIT ", optionX, srcLogo.h + 256);

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
                        break;
                    case SDLK_2:
                        exit(EXIT_SUCCESS);
                        break;
                    default:
                        break;
                }
            }
        }
    }
}

void loadCurrentLevel()
{
    char filename[20];
    sprintf(filename, "level%d.txt", currentLevel + 1);
    loadLevelFromFile(filename);
}

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

void initializeSDL()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
    {
        fprintf(stderr, "SDL Initialization failed: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    pWindow = SDL_CreateWindow("Arkanoid", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 600, 800, SDL_WINDOW_SHOWN);
    if (!pWindow)
    {
        fprintf(stderr, "Window creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    win_surf = SDL_GetWindowSurface(pWindow);
    plancheSprites = SDL_LoadBMP("./sprites.bmp");
    gameSprites = SDL_LoadBMP("./Arkanoid_sprites.bmp");
    asciiSprites = SDL_LoadBMP("./Arkanoid_ascii.bmp");
    menuSprites = SDL_LoadBMP("./logo.bmp");

    if (!plancheSprites || !gameSprites || !asciiSprites)
    {
        fprintf(stderr, "Sprite loading failed: %s\n", SDL_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    SDL_SetColorKey(plancheSprites, SDL_TRUE, 0);
    SDL_SetColorKey(gameSprites, SDL_TRUE, 0);
    SDL_SetColorKey(asciiSprites, SDL_TRUE, 0);
}

void processInput(bool* quit)
{
    SDL_Event event;
    SDL_PumpEvents();
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_SPACE] && ball.vy == 0)
    {
        ballIsAttached = false;
        ball.vy = -5;
        ball.vx = -1;
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

int main(int argc, char **argv)
{
    initializeSDL();
    showOptionsMenu(pWindow, win_surf);

    ballIsAttached = true;
    ball.y = 0;
    ball.x = 0;

    loadCurrentLevel();

    bool quit = false;

    while (!quit)
    {
        if (allBricksInvisible())
        {
            printf("Congratulations! You've won the game!\n");
            nextLevel();
        }

        if (currentLife <= 0)
        {
            printf("Life: 0, GAME OVER!\n");
            quit = true;
        }
        processInput(&quit);
        render();
        SDL_UpdateWindowSurface(pWindow);
        updateDeltaTime();
    }

    SDL_Quit();
    return 0;
}
