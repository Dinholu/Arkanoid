#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define NUM_BRICKS_PER_ROW 20
#define BRICK_WIDTH 30
#define BRICK_HEIGHT 13
#define NUM_ROWS 15
#define NUM_BRICKS (NUM_BRICKS_PER_ROW * NUM_ROWS)

const int FPS = 60.0;
// struct { double x; double y; } ball_speed;

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
double delta_t;	  // durée frame en ms
int x_vault;

SDL_Window *pWindow = NULL;
SDL_Surface *win_surf = NULL;
SDL_Surface *plancheSprites = NULL;
SDL_Surface *gameSprites = NULL;

SDL_Rect srcBg = {0, 128, 96, 128}; // x,y, w,h (0,0) en haut a gauche
SDL_Rect srcBall = {0, 96, 24, 24};
SDL_Rect srcVaiss = {128, 0, 128, 32};
SDL_Rect srcBrick = {1, 1, 30, 13};

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
	SDL_Rect ballRect = {ball.x, ball.y, 24, 24};
	for (int i = 0; i < NUM_BRICKS; i++)
	{
		if (brick[i].isVisible)
		{
			SDL_Rect brickRect = {brick[i].x, brick[i].y, 30, 13};
			if (isCollision(ballRect, brickRect))
			{
				brick[i].isVisible = false;

				// Calculez l'angle de rebond en fonction de la position de la collision
				double relativeCollisionX = (ball.x + 12) - (brick[i].x + 15);	 // Position relative de la collision par rapport au centre de la brique
				double normalizedRelativeCollisionX = relativeCollisionX / 15.0; // Normalisez la position relative

				// Ajustez l'angle de rebond en fonction de la position relative
				double bounceAngle = normalizedRelativeCollisionX * M_PI / 3.0; // Utilisez un angle de rebond entre -pi/3 et pi/3
				double speed = sqrt(ball.vx * ball.vx + ball.vy * ball.vy);		// Calculez la vitesse actuelle de la balle

				// Mettez à jour les composantes de la vitesse en fonction de l'angle de rebond
				ball.vx = speed * sin(bounceAngle);
				ball.vy = -speed * cos(bounceAngle);

				const double ACCELERATION_FACTOR = 1.1;
				ball.vx *= ACCELERATION_FACTOR;
				ball.vy *= ACCELERATION_FACTOR;
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
		brick[row * NUM_BRICKS_PER_ROW + col].isVisible = brick[row * NUM_BRICKS_PER_ROW + col].type; // Mettez à jour en fonction de la valeur lue

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

	SDL_SetColorKey(plancheSprites, true, 0); // 0: 00/00/00 noir -> transparent
	SDL_SetColorKey(gameSprites, true, 0);	  // 0: 00/00/00 noir -> transparent

	ball.x = win_surf->w / 2;
	ball.y = win_surf->h / 2;
	ball.vx = 2.0;
	ball.vy = 2.8;

	loadLevelFromFile("level1.txt");
	now = SDL_GetPerformanceCounter();
}

// fonction qui met à jour la surface de la fenetre "win_surf"
void draw()
{
	// remplit le fond
	SDL_Rect dest = {0, 0, 0, 0};
	for (int j = 0; j < win_surf->h; j += 128)
		for (int i = 0; i < win_surf->w; i += 96)
		{
			dest.x = i;
			dest.y = j;
			SDL_BlitSurface(plancheSprites, &srcBg, win_surf, &dest);
		}

	// affiche balle
	SDL_Rect dstBall = {ball.x, ball.y, 0, 0};
	SDL_BlitSurface(plancheSprites, &srcBall, win_surf, &dstBall);

	// dedplacement
	ball.x += ball.vx; // / delta_t;
	ball.y += ball.vy; // / delta_t;

	// collision bord
	if ((ball.x < 1) || (ball.x > (win_surf->w - 25)))
		ball.vx *= -1;
	if ((ball.y < 1) || (ball.y > (win_surf->h - 25)))
		ball.vy *= -1;

	// Collision avec le vaisseau
	if ((ball.y + 24 > win_surf->h - 32) && (ball.x + 24 > x_vault) && (ball.x < x_vault + 128))
	{
		// La balle a collisionné avec le vaisseau
		// Calculez l'angle de rebond en fonction de la position de la collision
		double relativeCollisionX = (ball.x + 12) - (x_vault + 64);		 // Position relative de la collision par rapport au centre du vaisseau
		double normalizedRelativeCollisionX = relativeCollisionX / 64.0; // Normalisez la position relative

		// Ajustez l'angle de rebond en fonction de la position relative
		double bounceAngle = normalizedRelativeCollisionX * M_PI / 3.0; // Utilisez un angle de rebond entre -pi/3 et pi/3
		double speed = sqrt(ball.vx * ball.vx + ball.vy * ball.vy);		// Calculez la vitesse actuelle de la balle

		// Mettez à jour les composantes de la vitesse en fonction de l'angle de rebond
		ball.vx = speed * sin(bounceAngle);
		ball.vy = -speed * cos(bounceAngle);
	}


	// touche bas -> rouge
	if (ball.y > (win_surf->h - 25))
	{
		ball.x = x_vault + 52;	   // 52 est la moitié de la largeur du vaisseau
		ball.y = win_surf->h - 58; // 58 est la hauteur de la balle + la hauteur du vaisseau
		ball.vy = 0;
		ball.vx = 0;
		ballIsAttached = true;
	}
	// touche bas -> vert
	if (ball.y < 1)
		srcBall.y = 96;

	// vaisseau
	dest.x = x_vault;
	dest.y = win_surf->h - 32;
	SDL_BlitSurface(plancheSprites, &srcVaiss, win_surf, &dest);

	// Si la balle est attachée, la positionner sur le vaisseau
	if (ballIsAttached)
	{
		ball.x = x_vault + 52;
		ball.y = win_surf->h - 58;
	}

	// Brique
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
	SDL_Event event; // Déclaration de la variable event ici

	while (!quit)
	{
		if (allBricksInvisible())
		{
			printf("Congratulations! You've won the game!\n");
			quit = true; // End the game loop
		}
		SDL_PumpEvents();
		const Uint8 *keys = SDL_GetKeyboardState(NULL);
		// Lancer le jeu avec la touche "Space"
		if (keys[SDL_SCANCODE_SPACE] && ball.vy == 0)
		{
			// Lancer le jeu (assigner une vitesse initiale à la balle)
			ballIsAttached = false;
			ball.vy = -1.4; // Vitesse vers le haut
			ball.vx = -1.0;
		}
		handleCollisions();

		if (keys[SDL_SCANCODE_LEFT])
			x_vault -= 10;
		if (keys[SDL_SCANCODE_RIGHT])
			x_vault += 10;
		if (keys[SDL_SCANCODE_ESCAPE])
			quit = true;

		if (x_vault < 0)
			x_vault = 0;
		if (x_vault > win_surf->w - 128) // 128 est la largeur du vaisseau
			x_vault = win_surf->w - 128;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				quit = true; // Quitte la boucle principale si l'événement de fermeture de la fenêtre est détecté
			}
		}

		draw();

		SDL_UpdateWindowSurface(pWindow);
		now = SDL_GetPerformanceCounter();
		delta_t = 1.0 / FPS - (double)(now - prev) / (double)SDL_GetPerformanceFrequency();
		prev = now;

		if (delta_t > 0)
			SDL_Delay((Uint32)(delta_t * 1000));
		// printf("dt = %lf\n", delta_t * 1000);
		prev = SDL_GetPerformanceCounter();
	}

	SDL_Quit();
	return 0;
}
