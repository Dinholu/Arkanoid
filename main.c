#include <SDL.h>
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
#define MAX_BALLS 3
#define VIE_MAX 5

struct Ball
{
    double x;
    double y;
    double vx;
    double vy;
    bool isActive;
} ball;

struct Ball balls[MAX_BALLS];
int activeBallCount = 1;

struct Brick
{
    double x;
    double y;
    int type;
    char destructType;
    bool isVisible;
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
SDL_Rect srcBackground = {320, 128, 64, 64};
// ------------------------------------------------------------------------------------------------------------------
SDL_Rect srcBall = {0, 96, 24, 24};
SDL_Rect srcVaisseau = {384, 160, 82, 16};
SDL_Rect srcBrick;
SDL_Rect destVaisseau;
SDL_Rect srcLogo = {0, 0, 400, 144};

// Mur
SDL_Rect srcTopWall = {0, 0, 556, 22};
SDL_Rect srcEdgeWall = {0, 0, 650, 22};

int x_vault;
int vault_width;
int currentLevel = 0;
int currentScore = 0;

// bonus enlarge le vaisseau
bool isVaultEnlarged = false;
Uint64 enlargeStartTime;
const double enlargeDuration = 5.0;

// bonus catch and fire
Uint64 attachTime = 0;
bool ballIsAttached = false;
int releaseCount = 0;

int currentLife = 3;
double delta_t;
double ballSpeedIncrement = BALL_SPEED_INCREMENT;
const int FPS = 60;
double max_speed = 8.0;

// Variable pour savoir si la touche V a été pressée donc a enlever quand ca sera fait par collision avec le bonus
bool vWasPressed = false;
// -------------------------------
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

void initializeBalls()
{
    for (int i = 0; i < MAX_BALLS; i++)
    {
        balls[i].x = 0;
        balls[i].y = 0;
        balls[i].vx = 0;
        balls[i].vy = 0;
        balls[i].isActive = false;
    }
    balls[0].isActive = true;
}
void splitBall()
{
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
    if (ball->x < 0)
    {
        ball->x = 0; // Forcer le reset de position pour éviter d'être bloqué
        ball->vx *= -1;
    }
    else if (ball->x + srcBall.w > win_surf->w)
    {
        ball->x = win_surf->w - srcBall.w; // De même ici
        ball->vx *= -1;
    }

    if (ball->y < 0)
    {
        ball->y = 0; // ET ici
        ball->vy *= -1;
    }
}

// TODO: remplacer les nombres magiques pour que cette fonction soit applicable peu importe la taille des vaisseaux.
// 24 correspond à la largeur de la balle en SDL_Rect
// 32 correspond à la position qu'on vouudrait positionner le vaisseau
// 128 correspond à la taille du vaisseau
// Ici x_vault nous indique la position relative du vaisseau sur l'affichage
void vaultCollision(struct Ball *ball)
{
    if ((ball->y + srcBall.h > win_surf->h - 32) && (ball->x + srcBall.w > x_vault) && (ball->x < x_vault + vault_width))
    {
        if (releaseCount > 0)
        {
            ballIsAttached = true;
            attachTime = SDL_GetPerformanceCounter();
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

void defeatCollision(struct Ball *ball)
{
    if (ball->y > (win_surf->h - 25))
    {
        ball->isActive = false;
        activeBallCount--;
        if (activeBallCount == 0)
        {
            currentLife--;
            ballIsAttached = true;
            attachTime = SDL_GetPerformanceCounter(); // Définir le temps d'attachement
            balls[0].isActive = true;
            balls[0].x = x_vault + (vault_width / 2) - 12;
            balls[0].y = win_surf->h - 58;
            balls[0].vy = 0;
            balls[0].vx = 0;
            activeBallCount = 1;
        }
    }
}

void handleBallProperty(struct Ball *ball, int brickIndex)
{
    double ballCenterX = ball->x + srcBall.w / 2;
    double ballCenterY = ball->y + srcBall.h / 2;
    double brickCenterX = brick[brickIndex].x + (BRICK_WIDTH / 2);
    double brickCenterY = brick[brickIndex].y + (BRICK_HEIGHT / 2);
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
            SDL_Rect brickRect = {brick[i].x, brick[i].y, BRICK_WIDTH, BRICK_HEIGHT};

            if (isCollision(ballRect, brickRect))
            {
                brick[i].isVisible = false;
                currentScore += 10;

                handleBallProperty(ball, i);
                printf("Score: %d\n", currentScore);
                break;
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

void renderString(SDL_Surface *surface, SDL_Surface *sprites, const char *string, int startX, int startY)
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

void renderMenu(SDL_Surface *sprites, SDL_Rect *srcLogo, SDL_Surface *win_surf)
{
    SDL_Rect dest = {0, 128, srcLogo->w, srcLogo->h};
    dest.x = (win_surf->w - srcLogo->w) / 2;

    SDL_BlitSurface(sprites, srcLogo, win_surf, &dest);
}

void renderBackground(SDL_Surface *sprites, SDL_Rect *srcBackground, SDL_Surface *win_surf)
{
    SDL_Rect dest = {0, 0, 0, 0};
    for (int j = 0; j < win_surf->h; j += 64)
    {
        for (int i = 0; i < win_surf->w; i += 64)
        {
            dest.x = i;
            dest.y = j;
            SDL_BlitSurface(sprites, srcBackground, win_surf, &dest);
        }
    }
}

void renderBalls(SDL_Surface *sprites, SDL_Rect *srcBall, SDL_Surface *win_surf, struct Ball *ball)
{
    for (int i = 0; i < MAX_BALLS; i++)
    {
        if (balls[i].isActive)
        {
            SDL_Rect destBall = {balls[i].x, balls[i].y, 0, 0};
            SDL_BlitSurface(plancheSprites, srcBall, win_surf, &destBall);
            balls[i].x += balls[i].vx;
            balls[i].y += balls[i].vy;
        }
    }
}

void renderVault(SDL_Surface *gameSprites, SDL_Rect *srcVaisseau, SDL_Surface *win_surf, int x_vault)
{
    destVaisseau = (SDL_Rect){x_vault, win_surf->h - 32, 0, 0};
    SDL_BlitSurface(gameSprites, srcVaisseau, win_surf, &destVaisseau);
}

// TODO: remplacer 52 par la moitie de la taille du vaisseau pour que la balle se positionne au centre du vaisseau
// 58 correspond à la hauteur ou le vaisseeau est positionné + largeur de la balle
void attachBallToVault(struct Ball *ball, int x_vault, int win_surf_height)
{
    ball->x = x_vault + (vault_width / 2) - (srcBall.w / 2);
    ball->y = destVaisseau.y - srcBall.h;
}

void renderBricks(SDL_Surface *gameSprites, SDL_Surface *win_surf, struct Brick bricks[], int num_bricks)
{
    for (int i = 0; i < num_bricks; i++)
    {
        if (bricks[i].isVisible)
        {
            SDL_Rect destBrick = {bricks[i].x, bricks[i].y, 0, 0};

            switch (bricks[i].type)
            {
            case 1:
                srcBrick = WHITE_BRICK;
                break;
            case 2:
                srcBrick = YELLOW_BRICK;
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
                srcBrick = ORANGE_BRICK;
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
                break;
            }
            SDL_BlitSurface(gameSprites, &srcBrick, win_surf, &destBrick);
        }
    }
}

void renderInfo(SDL_Surface *win_surf, SDL_Surface *asciiSprites, int value, char *label, int startX, int startY)
{
    char *string = malloc(sizeof(*string) * 256);
    sprintf(string, "%s:%d", label, value);
    renderString(win_surf, asciiSprites, string, startX, startY);
    free(string);
}

void render()
{
    renderBackground(gameSprites, &srcBackground, win_surf);

    handleCollisions();

    renderVault(gameSprites, &srcVaisseau, win_surf, x_vault);
    if (ballIsAttached)
    {
        attachBallToVault(&balls[0], x_vault, win_surf->h);
    }

    renderBalls(plancheSprites, &srcBall, win_surf, &ball);
    renderBricks(gameSprites, win_surf, brick, NUM_BRICKS);
    renderInfo(win_surf, asciiSprites, currentScore, "SCORE", 16, 10);
    renderInfo(win_surf, asciiSprites, currentLife, "LIFE", win_surf->w - 116, 10);
}

void showOptionsMenu(SDL_Window *pWindow, SDL_Surface *win_surf)
{
    bool inMenu = true;
    SDL_Event event;

    int optionWidth = 128;
    int startOptionX = (win_surf->w - optionWidth) / 2;

    while (inMenu)
    {
        renderMenu(menuSprites, &srcLogo, win_surf);

        renderString(win_surf, asciiSprites, "1. START", startOptionX, srcLogo.h + 192);
        renderString(win_surf, asciiSprites, "2. QUIT ", startOptionX, srcLogo.h + 256);

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
    attachTime = SDL_GetPerformanceCounter(); // Définir le temps d'attachement
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

    x_vault = (win_surf->w - srcVaisseau.w) / 2;
    vault_width = srcVaisseau.w;
}

void enlargeVault()
{

    if (!isVaultEnlarged)
    {
        printf("Vaisseau agrandi!\n");
        for (int i = 0; i < 5; i++)
        {
            srcVaisseau.y += 16; // Déplacer vers la ligne du sprite agrandi
            srcVaisseau.w += 16; // Augmenter la largeur du sprite
            vault_width = srcVaisseau.w;
        }
        // Mettre à jour la largeur du vaisseau
        isVaultEnlarged = true;
        enlargeStartTime = SDL_GetPerformanceCounter(); // Démarrer la minuterie
    }
    else
    {
        printf("Vaisseau réduit!\n");
        Uint64 now = SDL_GetPerformanceCounter();
        double elapsed = (now - enlargeStartTime) / (double)SDL_GetPerformanceFrequency();
        if (elapsed > enlargeDuration)
        {
            for (int i = 0; i < 5; i++)
            {
                srcVaisseau.y -= 16;         // Revenir à la ligne du sprite original
                srcVaisseau.w -= 16;         // Réduire la largeur du sprite
                vault_width = srcVaisseau.w; // Mettre à jour la largeur du vaisseau
            }
            isVaultEnlarged = false;
        }
    }
}

void updateVaultEnlargement()
{
    if (isVaultEnlarged)
    {
        Uint64 now = SDL_GetPerformanceCounter();
        double elapsed = (now - enlargeStartTime) / (double)SDL_GetPerformanceFrequency();
        if (elapsed > enlargeDuration)
        {
            for (int i = 0; i < 5; i++)
            {
                srcVaisseau.y -= 16; // Revenir à la ligne du sprite original
                srcVaisseau.w -= 16;
                vault_width = srcVaisseau.w;
            }
            isVaultEnlarged = false;
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

void slowDownBall()
{
    for (int i = 0; i < MAX_BALLS; i++)
    {
        if (balls[i].isActive)
        {
            balls[i].vx /= 1.15;
            balls[i].vy /= 1.15;
        }
    }
}

void CatchAndFire()
{
    releaseCount = 5;
}

void processInput(bool *quit)
{

    SDL_Event event;
    SDL_PumpEvents();
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_SPACE] && ballIsAttached)
    {
        if (releaseCount > 0)
        {
            releaseCount--;
        }
        ballIsAttached = false;
        balls[0].vy = -5;
        balls[0].vx = -1;
    }

    if (keys[SDL_SCANCODE_B] && !ballIsAttached && activeBallCount == 1)
    {
        splitBall();
    }

    if (keys[SDL_SCANCODE_V])
    {
        if (!vWasPressed)
        {
            addLife();
            vWasPressed = true;
        }
    }

    if (keys[SDL_SCANCODE_V] == 0)
    {
        vWasPressed = false;
    }

    if (keys[SDL_SCANCODE_C])
    {
        slowDownBall();
    }

    if (keys[SDL_SCANCODE_X])
    {
        CatchAndFire();
    }

    if (keys[SDL_SCANCODE_Z])
    {
        enlargeVault();
    }

    if (ballIsAttached && (SDL_GetPerformanceCounter() - attachTime) / (double)SDL_GetPerformanceFrequency() > 5.0)
    {
        ballIsAttached = false;
        balls[0].vy = -5;
        balls[0].vx = -1;
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
    attachTime = SDL_GetPerformanceCounter(); // Définir le temps d'attachement
    initializeBalls();
    loadCurrentLevel();

    bool quit = false;
    prev = SDL_GetPerformanceCounter();

    while (!quit)
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
            quit = true;
        }

        render();
        SDL_UpdateWindowSurface(pWindow);
        updateDeltaTime();
    }

    SDL_Quit();
    return 0;
}
