#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

const int FPS = 60.0;
// struct { double x; double y; } ball_speed;
struct
{
	double x;
	double y;
	double vx;
	double vy;
} ball;

struct 
{
	double x;
	double y;
	int type;
	bool isVisible;
} brick;

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

void init()
{
	pWindow = SDL_CreateWindow("Arknoid", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 600, 600, SDL_WINDOW_SHOWN);
	win_surf = SDL_GetWindowSurface(pWindow);
	plancheSprites = SDL_LoadBMP("./sprites.bmp");
	gameSprites = SDL_LoadBMP("./Arkanoid_sprites.bmp");

	SDL_SetColorKey(plancheSprites, true, 0); // 0: 00/00/00 noir -> transparent
	SDL_SetColorKey(gameSprites, true, 0);	 // 0: 00/00/00 noir -> transparent

	ball.x = win_surf->w / 2;
	ball.y = win_surf->h / 2;
	ball.vx = 2.0;
	ball.vy = 2.8;

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
		ball.vy *= -1;
	}

	// Collision avec la brique
	if (ball.y <= brick.y + srcBrick.h && ball.y >= brick.y - 24 && ball.x >= brick.x && ball.x <= brick.x + srcBrick.w)
    {
        ball.vy *= -1;
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
	SDL_Rect dstBrick = {brick.x, brick.y, 0, 0};
	SDL_BlitSurface(gameSprites, &srcBrick, win_surf, &dstBrick);
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
		printf("dt = %lf\n", delta_t * 1000);
		prev = SDL_GetPerformanceCounter();
	}

	SDL_Quit();
	return 0;
}
