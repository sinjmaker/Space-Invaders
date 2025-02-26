#include "render.h"

void drawAliens(SDL_Renderer *renderer) {
    for (int i = 0; i < ALIEN_ROWS; i++) {
        for (int j = 0; j < ALIEN_COLUMNS; j++) {
            if (aliens[i][j].alive) {
                SDL_Rect alienRect = {
                    aliens[i][j].x,
                    aliens[i][j].y,
                    aliens[i][j].width,
                    aliens[i][j].height
                };
                SDL_RenderCopy(renderer, alienTexture, NULL, &alienRect);
            }
        }
    }
}

void drawPlayerShip(SDL_Renderer *renderer) {
    SDL_Rect playerRect = {playerShip.x, playerShip.y, playerShip.width, playerShip.height};
    SDL_RenderCopy(renderer, persoTexture, NULL, &playerRect);

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    for (int i = 0; i < playerShip.lives; i++) {
        SDL_Rect lifeRect = {10 + i * 30, 10, 20, 20};
        SDL_RenderFillRect(renderer, &lifeRect);
    }
}

void drawLasers(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    for (int i = 0; i < ALIEN_COLUMNS; i++) {
        if (alienLasers[i].active) {
            SDL_Rect laserRect = {alienLasers[i].x, alienLasers[i].y, LASER_WIDTH, LASER_HEIGHT};
            SDL_RenderFillRect(renderer, &laserRect);
        }
    }

    if (playerLaser.active) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        SDL_Rect playerLaserRect = {playerLaser.x, playerLaser.y, LASER_WIDTH, LASER_HEIGHT};
        SDL_RenderFillRect(renderer, &playerLaserRect);
    }
}

void drawCloud(SDL_Renderer *renderer) {
    if (revivalCloud.active) {
        SDL_Rect cloudRect = {revivalCloud.x, revivalCloud.y, revivalCloud.width, revivalCloud.height};
        SDL_RenderCopy(renderer, cloudTexture, NULL, &cloudRect);
    }
}

SDL_Texture *loadTexture(SDL_Renderer *renderer, const char *filePath) {
    SDL_Surface *surface = SDL_LoadBMP(filePath);
    if (!surface) {
        fprintf(stderr, "Erreur de chargement de l'image %s: %s\n", filePath, SDL_GetError());
        return NULL;
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
        fprintf(stderr, "Erreur de création de la texture: %s\n", SDL_GetError());
    }
    return texture;
}