#ifndef RENDER_H
#define RENDER_H

#include "game.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

void drawAliens(SDL_Renderer *renderer);
void drawPlayerShip(SDL_Renderer *renderer);
void drawLasers(SDL_Renderer *renderer);
void drawCloud(SDL_Renderer *renderer);
SDL_Texture *loadTexture(SDL_Renderer *renderer, const char *filePath);
void drawPauseMenu(SDL_Renderer *renderer);
void drawBarriers(SDL_Renderer *renderer);
void drawTurrets(SDL_Renderer *renderer);
void drawTurretLasers(SDL_Renderer *renderer);
void drawScore(SDL_Renderer *renderer, TTF_Font *font);
void drawGameOverScreen(SDL_Renderer *renderer, TTF_Font *font);
void drawTextInputScreen(SDL_Renderer *renderer, TTF_Font *font, const char *playerName, bool showError);

#endif // RENDER_H