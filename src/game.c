#include "game.h"

Uint64 prev, now;
Level levels[NUM_LEVELS];
int currentLevel = 1;
bool isGameOver = false;
bool showMenu = true;
int currentLife = 3;
double delta_t;
const int FPS = 60;

bool isCollision(SDL_Rect rect1, SDL_Rect rect2)
{
    return !(rect1.x + rect1.w < rect2.x ||
             rect1.x > rect2.x + rect2.w ||
             rect1.y + rect1.h < rect2.y ||
             rect1.y > rect2.y + rect2.h);
}

void loadLevelFromFile(const char *filename, bool isEight)
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

    if (isEight)
    {
        touched++;
        max_speed++;
    }

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

void loadCurrentLevel(bool isEight)
{
    char filename[20];
    sprintf(filename, "level%d.txt", currentLevel);
    loadLevelFromFile(filename, isEight);
}

void nextLevel()
{
    changeBackground();
    clearBonuses();
    resetAllBonuses();
    clearHarmfuls();
    vault_width = srcVault.w;
    currentHarmfulsType++;
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
    currentLevel++;
    loadCurrentLevel(((currentLevel) % 8 == 0));
}

void resetGame()
{
    clearBonuses();
    resetAllBonuses();
    clearHarmfuls();
    currentLife = 3;
    currentScore = 0;
    currentLevel = 1;
    max_speed = 8.0;
    srcBackground.x = 0;
    ballSpeedIncrement = BALL_SPEED_INCREMENT;
    attachTime = SDL_GetPerformanceCounter();
    initializeBalls();
    initializeLasers();
    initializeBonuses();
    loadCurrentLevel(((currentLevel) % 8 == 0));
    initializeHarmfuls();
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
        else if (isPaused)
        {
            showPauseMenu(win_surf);
            SDL_UpdateWindowSurface(pWindow);

            // Boucle pour gérer les événements pendant la pause
            SDL_Event event;
            while (isPaused && !quit)
            {
                while (SDL_PollEvent(&event))
                {
                    if (event.type == SDL_QUIT)
                    {
                        quit = true;
                    }
                    else if (event.type == SDL_KEYDOWN)
                    {
                        switch (event.key.keysym.sym)
                        {
                        case SDLK_1:
                            isPaused = false;
                            break;
                        case SDLK_2:
                            resetGame();
                            isPaused = false;
                            break;
                        case SDLK_3:
                            showMenu = true;
                            isPaused = false;
                            break;
                        default:
                            break;
                        }
                    }
                }
                SDL_Delay(16); // Pour éviter de consommer trop de ressources CPU
            }
        }
        else if (!isGameOver)
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

            SDL_UpdateWindowSurface(pWindow);
            updateDeltaTime();
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

            SDL_UpdateWindowSurface(pWindow);
            updateDeltaTime();
        }
    }
}
