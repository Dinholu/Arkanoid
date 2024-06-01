#ifndef INPUT_H
#define INPUT_H

#include <SDL2/SDL.h>
#include "score.h"
#include "game.h"

extern bool vWasPressed;
extern bool nWasPressed;
extern bool mWasPressed;
extern bool isPaused;

extern bool spaceWasPressed;
extern bool enteringName;
extern int nameIndex;

void processInput(bool *quit);
void processCongratulationsInput(SDL_Event *event);
void processNameInput(SDL_Event *event);

#endif // INPUT_H
