#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#include "ball.h"
#include "brick.h"
#include "vault.h"
#include "harmful.h"
#include "bonus.h"
#include "render.h"
#include "score.h"

// Variable pour savoir si la touche V a été pressée donc a enlever quand ca sera fait par collision avec le bonus
bool vWasPressed = false;
// Variable pour savoir si la touche N a été pressée donc a enlever quand ca sera fait par collision avec le bonus
bool nwasPressed = false;
// Variable pour savoir si la touche M a été pressée donc a enlever quand ca sera fait par collision avec le bonus
bool mWasPressed = false;

bool spaceWasPressed = false;
// test background

bool isCollision(SDL_Rect rect1, SDL_Rect rect2)
{
    return !(rect1.x + rect1.w < rect2.x ||
             rect1.x > rect2.x + rect2.w ||
             rect1.y + rect1.h < rect2.y ||
             rect1.y > rect2.y + rect2.h);
}

// TODO: remplacer les nombres magiques pour que cette fonction soit applicable peu importe la taille des vaisseaux.
// 24 correspond à la largeur de la balle en SDL_Rect
// 32 correspond à la position qu'on voudrait positionner le vaisseau
// 128 correspond à la taille du vaisseau
// Ici x_vault nous indique la position relative du vaisseau sur l'affichage

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
    max_speed = max_speed + 2.0;
    loadCurrentLevel(((currentLevel) % 8 == 0));
    currentLevel++;
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
