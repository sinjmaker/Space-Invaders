#ifndef INPUT_H
#define INPUT_H

#include "game.h"

void handlePlayerMovement();
void handlePlayerInput(SDL_Event event);
void handleMouseInput(SDL_Event event);
void handleTextInput(SDL_Event event, char *playerName, bool *isEnterPressed);

#endif // INPUT_H