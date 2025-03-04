#ifndef RENDER_H
#define RENDER_H

#include "game.h"

void drawAliens(SDL_Renderer *renderer);
void drawPlayerShip(SDL_Renderer *renderer);
void drawLasers(SDL_Renderer *renderer);
void drawCloud(SDL_Renderer *renderer);
SDL_Texture *loadTexture(SDL_Renderer *renderer, const char *filePath);
void drawPauseMenu(SDL_Renderer *renderer);
void drawBarriers(SDL_Renderer *renderer);
void drawTurrets(SDL_Renderer *renderer);
void drawTurretLasers(SDL_Renderer *renderer);

#endif // RENDER_H