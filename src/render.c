#include "render.h"

SDL_Window *pWindow = NULL;
SDL_Surface *win_surf = NULL;

SDL_Surface *gameSprites = NULL;
SDL_Surface *asciiSprites = NULL;

SDL_Surface *menuSprites = NULL;
SDL_Surface *topWallSprites = NULL;
SDL_Surface *leftWallSprites = NULL;
SDL_Surface *rightWallSprites = NULL;

SDL_Rect srcBackground = {0, 128, 64, 64};
SDL_Rect srcLogo = {0, 0, 388, 96};
SDL_Rect srcVaus = {0, 108, 192, 90};
SDL_Rect srcTopWall = {22, 0, 512, 22};
SDL_Rect srcEdgeWall = {0, 0, 22, 650};

Color red = {255, 0, 0};
Color green = {0, 255, 0};
Color blue = {0, 0, 255};
Color white = {255, 255, 255};
Color grey = {180, 180, 180};

const int Y_WALLS = 144;
int backgroundChange = 0;

void initializeSDL()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
    {
        fprintf(stderr, "SDL Initialization failed: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    pWindow = SDL_CreateWindow("Arkanoid", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 556, 768, SDL_WINDOW_SHOWN);
    if (!pWindow)
    {
        fprintf(stderr, "Window creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    win_surf = SDL_GetWindowSurface(pWindow);
    gameSprites = SDL_LoadBMP("./Arkanoid_sprites.bmp");
    asciiSprites = SDL_LoadBMP("./Arkanoid_ascii_sprites.bmp");
    menuSprites = SDL_LoadBMP("./Arkanoid_menu_sprites.bmp");
    topWallSprites = SDL_LoadBMP("./edge_top.bmp");
    leftWallSprites = SDL_LoadBMP("./edge_left.bmp");
    rightWallSprites = SDL_LoadBMP("./edge_right.bmp");

    if (!gameSprites || !asciiSprites || !menuSprites || !topWallSprites || !leftWallSprites || !rightWallSprites)
    {
        fprintf(stderr, "Sprite loading failed: %s\n", SDL_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    SDL_SetColorKey(gameSprites, SDL_TRUE, 0);
    SDL_SetColorKey(asciiSprites, SDL_TRUE, 0);
    SDL_SetColorKey(menuSprites, SDL_TRUE, 0);
    SDL_SetColorKey(topWallSprites, SDL_TRUE, 0);
    SDL_SetColorKey(leftWallSprites, SDL_TRUE, 0);
    SDL_SetColorKey(rightWallSprites, SDL_TRUE, 0);

    x_vault = (win_surf->w - srcVault.w) / 2;
    vault_width = srcVault.w;
}

void moveAndRenderHarmfuls(SDL_Surface *gameSprites, SDL_Surface *win_surf)
{
    for (int i = 0; i < MAX_HARMFULS; i++)
    {
        if (harmfuls[i].isDestroying)
        {
            harmfuls[i].destroyAnimationTime += 1.0 / FPS;
            if (harmfuls[i].destroyAnimationTime >= 0.1)
            {
                harmfuls[i].destroyAnimationFrame++;
                harmfuls[i].destroyAnimationTime = 0;
                if (harmfuls[i].destroyAnimationFrame >= 5)
                {
                    harmfuls[i].isDestroying = false;
                    harmfuls[i].isActive = false;
                }
            }

            SDL_Rect srcHarmfulExplosion = {
                HARMFUL_EXPLOSITION.x + harmfuls[i].destroyAnimationFrame * 32,
                HARMFUL_EXPLOSITION.y,
                HARMFUL_EXPLOSITION.w,
                HARMFUL_EXPLOSITION.h};

            SDL_Rect destHarmful = {harmfuls[i].x, harmfuls[i].y, srcHarmfulExplosion.w, srcHarmfulExplosion.h};
            SDL_BlitSurface(gameSprites, &srcHarmfulExplosion, win_surf, &destHarmful);
        }
        if (harmfuls[i].isActive)
        {
            if (harmfuls[i].y > win_surf->h / 2 && !harmfuls[i].isSinusoidal)
            {
                harmfuls[i].isSinusoidal = true;
                harmfuls[i].amplitude = 2; // Amplitude réduite pour une sinusoïde plus douce
                harmfuls[i].time = 0.0f;
            }
            if (harmfuls[i].isSinusoidal)
            {
                harmfuls[i].time += 1.0 / FPS;
                harmfuls[i].elongationTime += 1.0 / FPS;

                harmfuls[i].x += harmfuls[i].vx;

                if (harmfuls[i].elongationTime >= harmfuls[i].randomElongationTime)
                {
                    harmfuls[i].y += harmfuls[i].vy; // Descendre vers le bas
                    harmfuls[i].vy += 0.1;           // Augmente la vitesse progressivement
                    harmfuls[i].height += 1;         // Augmenter la hauteur progressivement

                    if (harmfuls[i].y > win_surf->h)
                    {
                        harmfuls[i].isActive = false; // Désactiver quand il sort de l'écran
                    }
                }

                harmfuls[i].y = harmfuls[i].y + harmfuls[i].amplitude * sin(2 * M_PI * 0.5 * harmfuls[i].time);
                if (harmfuls[i].x < srcEdgeWall.w || harmfuls[i].x + 32 > win_surf->w - srcEdgeWall.w)
                {
                    harmfuls[i].vx *= -1;
                }

                if (harmfuls[i].y > win_surf->h)
                {
                    harmfuls[i].isActive = false;
                }
            }
            else
            {
                if (!harmfuls[i].isFalling)
                {
                    harmfuls[i].x += harmfuls[i].vx;

                    if (harmfuls[i].x < srcEdgeWall.w || harmfuls[i].x + 32 > win_surf->w - srcEdgeWall.w)
                    {
                        harmfuls[i].vx *= -1; // Changer de direction en cas de collision avec les bords
                    }

                    bool hitSideBrick = false;
                    for (int j = 0; j < NUM_BRICKS; j++)
                    {
                        if (brick[j].isVisible)
                        {
                            SDL_Rect harmfulRect = {harmfuls[i].x, harmfuls[i].y, 32, 32};
                            SDL_Rect brickRect = {brick[j].x + srcEdgeWall.w, brick[j].y + srcTopWall.h + Y_WALLS, BRICK_WIDTH, BRICK_HEIGHT};

                            if (isCollision(harmfulRect, brickRect))
                            {
                                // Vérifier uniquement les collisions latérales
                                if ((harmfulRect.x < brickRect.x + brickRect.w && harmfulRect.x + harmfulRect.w > brickRect.x) &&
                                    (harmfulRect.y < brickRect.y + brickRect.h && harmfulRect.y + harmfulRect.h > brickRect.y))
                                {
                                    harmfuls[i].vx *= -1;
                                    hitSideBrick = true;
                                    break;
                                }
                            }
                        }
                    }

                    if (!hitSideBrick)
                    {
                        bool canFall = true;
                        for (int j = 0; j < NUM_BRICKS; j++)
                        {
                            if (brick[j].isVisible)
                            {
                                SDL_Rect belowRect = {harmfuls[i].x, harmfuls[i].y + 32, 32, 1}; // Rect pour vérifier juste en dessous
                                SDL_Rect brickRect = {brick[j].x + srcEdgeWall.w, brick[j].y + srcTopWall.h + Y_WALLS, BRICK_WIDTH, BRICK_HEIGHT};

                                if (isCollision(belowRect, brickRect))
                                {
                                    canFall = false;
                                    break;
                                }
                            }
                        }

                        if (canFall)
                        {
                            harmfuls[i].isFalling = true;
                            harmfuls[i].vy = 1;
                        }
                    }
                }
                else
                {
                    harmfuls[i].y += harmfuls[i].vy;

                    for (int j = 0; j < NUM_BRICKS; j++)
                    {
                        if (brick[j].isVisible)
                        {
                            SDL_Rect harmfulRect = {harmfuls[i].x, harmfuls[i].y, 32, 32};
                            SDL_Rect brickRect = {brick[j].x + srcEdgeWall.w, brick[j].y + srcTopWall.h + Y_WALLS, BRICK_WIDTH, BRICK_HEIGHT};

                            if (isCollision(harmfulRect, brickRect))
                            {
                                harmfuls[i].y = brickRect.y - 32;
                                harmfuls[i].isFalling = false;
                                harmfuls[i].vx = (rand() % 2 == 0) ? 2 : -2;
                                break;
                            }
                        }
                    }
                }
            }
            // Gestion de l'animation des harmfuls
            harmfuls[i].animationTime += 1.0 / FPS;
            if (harmfuls[i].animationTime >= 0.1)
            {
                harmfuls[i].animationFrame = (harmfuls[i].animationFrame + 1) % harmfuls[i].maxSteps;
                harmfuls[i].animationTime = 0;
            }

            SDL_Rect srcHarmful = getHarmfulSrcRect(harmfuls[i].type, harmfuls[i].animationFrame);
            SDL_Rect destHarmful = {harmfuls[i].x, harmfuls[i].y, srcHarmful.w, srcHarmful.h};
            renderShadow(gameSprites, &srcHarmful, &destHarmful, 5, 4, 255);
            SDL_BlitSurface(gameSprites, &srcHarmful, win_surf, &destHarmful);
        }
    }
}

void renderString(SDL_Surface *sprites, SDL_Surface *surface, const char *string, int startX, int startY, const char *alignment, Color color)
{
    int x = startX;
    int y = startY;
    const int spacing = 1;
    int totalWidth = 0;

    SDL_SetSurfaceColorMod(sprites, color.r, color.g, color.b);

    for (const char *temp = string; *temp; temp++)
    {
        SDL_Rect srcRect = charToSDLRect(*temp);
        totalWidth += srcRect.w + spacing;
    }
    totalWidth -= spacing;

    if (strcmp(alignment, "right") == 0)
    {
        x -= totalWidth;
    }
    else if (strcmp(alignment, "center") == 0)
    {
        x -= totalWidth / 2;
    }

    while (*string)
    {
        SDL_Rect srcRect = charToSDLRect(*string);
        SDL_Rect destRect = {x, y, srcRect.w, srcRect.h};
        SDL_BlitSurface(sprites, &srcRect, surface, &destRect);
        x += srcRect.w + spacing;
        string++;
    }

    SDL_SetSurfaceColorMod(sprites, 255, 255, 255);
}

void renderGameOverScreen(SDL_Surface *sprites, SDL_Rect *srcLogo, SDL_Surface *win_surf)
{
    SDL_FillRect(win_surf, NULL, SDL_MapRGB(win_surf->format, 0, 0, 0));
    SDL_Rect dest = {0, 128, srcLogo->w, srcLogo->h};
    dest.x = (win_surf->w - srcLogo->w) / 2;

    SDL_BlitSurface(sprites, srcLogo, win_surf, &dest);

    renderString(asciiSprites, win_surf, "GAME OVER", win_surf->w / 2, 300, "center", red);
    renderString(asciiSprites, win_surf, "ENTER NAME", win_surf->w / 2, 350, "center", grey);
    renderString(asciiSprites, win_surf, playerName, win_surf->w / 2, 400, "center", white);
}

void renderCongratulationsScreen(SDL_Surface *sprites, SDL_Rect *srcLogo, SDL_Surface *win_surf)
{
    SDL_FillRect(win_surf, NULL, SDL_MapRGB(win_surf->format, 0, 0, 0));
    SDL_Rect dest = {0, 128, srcLogo->w, srcLogo->h};
    dest.x = (win_surf->w - srcLogo->w) / 2;

    SDL_BlitSurface(sprites, srcLogo, win_surf, &dest);

    renderString(asciiSprites, win_surf, "CONGRATULATIONS!", win_surf->w / 2, 300, "center", green);
    renderString(asciiSprites, win_surf, "ENTER NAME", win_surf->w / 2, 350, "center", grey);
    renderString(asciiSprites, win_surf, playerName, win_surf->w / 2, 400, "center", white);
}

void renderMenu(SDL_Surface *sprites, SDL_Rect *srcLogo, SDL_Surface *win_surf)
{
    SDL_Rect destLogo = {0, 128, srcLogo->w, srcLogo->h};
    destLogo.x = (win_surf->w - srcLogo->w) / 2;
    SDL_BlitSurface(sprites, srcLogo, win_surf, &destLogo);

    int vausOffset = 128;
    SDL_Rect destVaus = {(win_surf->w - srcVaus.w) / 2, win_surf->h - srcVaus.h - vausOffset, srcVaus.w, srcVaus.h};
    SDL_BlitSurface(sprites, &srcVaus, win_surf, &destVaus);
}

void renderBackground(SDL_Surface *sprites, SDL_Rect *srcBackground, SDL_Surface *win_surf)
{
    SDL_Rect dest = {0, 0, 0, 0};
    for (int j = Y_WALLS + srcTopWall.h; j < win_surf->h; j += srcBackground->h)
    {
        for (int i = srcEdgeWall.w; i < win_surf->w - srcEdgeWall.w; i += srcBackground->w)
        {
            dest.x = i;
            dest.y = j;
            SDL_BlitSurface(sprites, srcBackground, win_surf, &dest);
        }
    }
    renderBorderShadows();
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

void renderBallTrail(SDL_Surface *sprites, SDL_Rect *srcBall, SDL_Surface *win_surf, struct Ball *ball)
{
    for (int i = 1; i < ball->trailLength; i++)
    {
        SDL_Rect dest = {ball->trail[i].x, ball->trail[i].y, srcBall->w, srcBall->h};

        SDL_SetSurfaceBlendMode(sprites, SDL_BLENDMODE_BLEND);
        SDL_SetSurfaceAlphaMod(sprites, ball->trail[i].alpha);

        SDL_BlitSurface(sprites, srcBall, win_surf, &dest);
    }

    SDL_SetSurfaceAlphaMod(sprites, 255);
}

void renderBalls(SDL_Surface *sprites, SDL_Rect *srcBall, SDL_Surface *win_surf)
{
    for (int i = 0; i < MAX_BALLS; i++)
    {
        if (balls[i].isActive)
        {
            renderBallTrail(sprites, srcBall, win_surf, &balls[i]);

            SDL_Rect destBall = {balls[i].x, balls[i].y, srcBall->w, srcBall->h};
            renderShadow(sprites, srcBall, &destBall, 4, 4, 144);
            SDL_BlitSurface(sprites, srcBall, win_surf, &destBall);
        }
    }
}

void renderVault(SDL_Surface *sprites, SDL_Rect *srcVault, SDL_Surface *win_surf, int x_vault)
{
    destVault = (SDL_Rect){x_vault, win_surf->h - 32, 0, 0};
    renderShadow(sprites, srcVault, &destVault, 5, 7, 255);
    SDL_BlitSurface(sprites, srcVault, win_surf, &destVault);
}

void renderHP(SDL_Surface *sprites, SDL_Surface *win_surf, int currentLife)
{
    int startX = srcEdgeWall.w;
    int startY = win_surf->h - 12;

    for (int i = 0; i < currentLife; i++)
    {
        SDL_Rect destHP = {startX + i * (VAUS_HP.w + 2), startY, VAUS_HP.w, VAUS_HP.h};
        SDL_BlitSurface(sprites, &VAUS_HP, win_surf, &destHP);
    }
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
            renderShadow(sprites, &srcBrick, &destBrick, 7, 12, 96);
            SDL_BlitSurface(sprites, &srcBrick, win_surf, &destBrick);
        }
    }
}

void renderInfo(SDL_Surface *sprites, int value, char *label, int startX, int startY, const char *alignment, Color color)
{
    char *string = (char *)malloc(sizeof(*string) * 256);
    sprintf(string, "%s%d", label, value);
    renderString(sprites, win_surf, string, startX, startY, alignment, color);
    free(string);
}

void showOptionsMenu(SDL_Window *pWindow, SDL_Surface *win_surf)
{
    SDL_FillRect(win_surf, NULL, SDL_MapRGB(win_surf->format, 0, 0, 0));
    bool inMenu = true;
    SDL_Event event;

    int startOptionX = 192;

    while (inMenu)
    {
        renderMenu(menuSprites, &srcLogo, win_surf);
        renderString(asciiSprites, win_surf, "1. START", startOptionX, srcLogo.h + 192, "left", grey);
        renderString(asciiSprites, win_surf, "2. LEADERBOARD", startOptionX, srcLogo.h + 256, "left", grey);
        renderString(asciiSprites, win_surf, "3. QUIT", startOptionX, srcLogo.h + 320, "left", grey);

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

void showPauseMenu(SDL_Surface *win_surf)
{
    int startOptionX = 192;

    SDL_FillRect(win_surf, NULL, SDL_MapRGB(win_surf->format, 0, 0, 0));

    renderString(asciiSprites, win_surf, "PAUSED", win_surf->w /2, 250, "center", grey);
    renderString(asciiSprites, win_surf, "1. CONTINUE", startOptionX, 350, "left", grey);
    renderString(asciiSprites, win_surf, "2. RETRY", startOptionX, 400, "left", grey);
    renderString(asciiSprites, win_surf, "3. QUIT", startOptionX, 450, "left", grey);
    SDL_Rect destVaus = {(win_surf->w - srcVaus.w) / 2, win_surf->h - srcVaus.h - 96, srcVaus.w, srcVaus.h};
    SDL_BlitSurface(menuSprites, &srcVaus, win_surf, &destVaus);
    SDL_UpdateWindowSurface(pWindow);
}

void renderBonuses(SDL_Surface *gameSprites, SDL_Surface *win_surf)
{
    for (int i = 0; i < MAX_BONUSES; i++)
    {
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
            SDL_Rect destBonus = {bonuses[i].x + srcEdgeWall.w, bonuses[i].y + Y_WALLS + srcTopWall.h, srcBonus.w, srcBonus.h};

            renderShadow(gameSprites, &srcBonus, &destBonus, 3, 5, 192);
            SDL_BlitSurface(gameSprites, &srcBonus, win_surf, &destBonus);
        }
    }
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

void showHighScores(SDL_Surface *win_surf, SDL_Surface *asciiSprites)
{
    SDL_FillRect(win_surf, NULL, SDL_MapRGB(win_surf->format, 0, 0, 0));

    SDL_Rect destLogo = {0, 128, srcLogo.w, srcLogo.h};
    destLogo.x = (win_surf->w - srcLogo.w) / 2;
    SDL_BlitSurface(menuSprites, &srcLogo, win_surf, &destLogo);

    HighScore highScores[MAX_HIGHSCORE];
    int count;
    readHighScores(highScores, &count);
    int startLeaderBoardY = srcLogo.h + 192;

    renderString(asciiSprites, win_surf, "LEADERBOARD", win_surf->w / 2, startLeaderBoardY, "center", blue);

    for (int i = 0; i < count; i++)
    {
        char scoreText[256];
        sprintf(scoreText, "%d. %s %d", i + 1, highScores[i].name, highScores[i].score);
        renderString(asciiSprites, win_surf, scoreText, 164, startLeaderBoardY + 50 + i * 32, "left", white);
    }

    SDL_UpdateWindowSurface(pWindow);

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

void render()
{
    SDL_FillRect(win_surf, NULL, SDL_MapRGB(win_surf->format, 0, 0, 0));
    renderAllWalls();
    renderBackground(gameSprites, &srcBackground, win_surf);
    renderVault(gameSprites, &srcVault, win_surf, x_vault);
    renderBricks(gameSprites, NUM_BRICKS);
    renderHP(gameSprites, win_surf, currentLife);
    renderInfo(asciiSprites, currentScore, "", 16, 32, "left", white);
    renderInfo(asciiSprites, currentLevel, "ROUND ", win_surf->w / 2, 108, "center", grey); // a clean
    renderString(asciiSprites, win_surf, "HI-SCORE", win_surf->w - 16, 32, "right", red);
    renderInfo(asciiSprites, getHighestScore(), "", win_surf->w - 16, 64, "right", white);

    for (int i = 0; i < MAX_BALLS; i++)
    {
        if (balls[i].isAttached)
        {
            attachBallToVault(&balls[i], x_vault);
        }
    }
    handleBallUpdates();
    renderBalls(gameSprites, &srcBall, win_surf);
    moveBonuses();
    renderBonuses(gameSprites, win_surf);
    moveAndRenderLasers(gameSprites, &srcLeftLaser, &srcRightLaser, win_surf);
    moveAndRenderHarmfuls(gameSprites, win_surf);
    handleCollisions();
    handleHarmfulCollisions();
    handleBonusCollision();
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

void renderShadow(SDL_Surface *sprites, SDL_Rect *srcRect, SDL_Rect *destRect, int offsetX, int offsetY, int alpha)
{
    if (ACTIVATE_SHADOW)
    {
        SDL_SetSurfaceBlendMode(sprites, SDL_BLENDMODE_BLEND);
        SDL_SetSurfaceAlphaMod(sprites, alpha);
        SDL_SetSurfaceColorMod(sprites, 10, 10, 10);
        SDL_Rect shadowDest = {destRect->x + offsetX, destRect->y + offsetY, destRect->w, destRect->h};

        SDL_BlitSurface(sprites, srcRect, win_surf, &shadowDest);

        SDL_SetSurfaceAlphaMod(sprites, 255);
        SDL_SetSurfaceColorMod(sprites, 255, 255, 255);
    }
}

void renderBorderShadows()
{
    if (ACTIVATE_SHADOW)
    {
        SDL_Renderer *renderer = SDL_CreateSoftwareRenderer(win_surf);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

        SDL_Rect topShadow = {srcEdgeWall.w, Y_WALLS + srcTopWall.h, win_surf->w - 2 * srcEdgeWall.w, 16};
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 112);
        SDL_RenderFillRect(renderer, &topShadow);

        SDL_Rect leftShadow = {srcEdgeWall.w, Y_WALLS + srcTopWall.h, 16, win_surf->h - Y_WALLS - srcTopWall.h};
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 112);
        SDL_RenderFillRect(renderer, &leftShadow);

        SDL_DestroyRenderer(renderer);
    }
}
