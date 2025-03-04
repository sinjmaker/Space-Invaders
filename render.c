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

    for (int i = 0; i < playerShip.ecrous; i++) {
        SDL_Rect ecrousRect = {10 + i * 30, screenHeight -50, 40, 40};
        SDL_RenderCopy(renderer, ecrousTexture, NULL, &ecrousRect);
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

void drawPauseMenu(SDL_Renderer *renderer) {
    // Afficher l'image de pause
    if (pauseTexture) {
        int pauseWidth = 200;  // Largeur de l'image de pause
        int pauseHeight = 100; // Hauteur de l'image de pause
        SDL_Rect pauseRect = {
            screenWidth / 2 - pauseWidth / 2,  // Position X (centré)
            screenHeight / 2 - pauseHeight / 2, // Position Y (centré)
            pauseWidth,                        // Largeur
            pauseHeight                         // Hauteur
        };
        SDL_RenderCopy(renderer, pauseTexture, NULL, &pauseRect);
    }

    // Dessiner un rectangle à moitié vert et à moitié bleu avec un "+" au milieu
    for (int i = 0; i < NUM_BARRIERS; i++) {

        if (barriers[i].health <= 0 && barriers[i].health != -1) {  // Barrière détruite
            SDL_Rect destroyedRect = {
                barriers[i].x + barriers[i].width / 4,  // Position X (centré sur la barrière)
                barriers[i].y + barriers[i].height / 4, // Position Y (centré sur la barrière)
                barriers[i].width / 2,                 // Largeur du rectangle
                barriers[i].visualHeight / 2           // Hauteur du rectangle
            };

            // Dessiner la moitié gauche en vert
            if(barriers[i].health > -11){
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            }else if (barriers[i].health == -20){
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            }
            SDL_Rect leftHalf = {
                destroyedRect.x,
                destroyedRect.y,
                destroyedRect.w / 2,
                destroyedRect.h
            };
            SDL_RenderFillRect(renderer, &leftHalf);

            // Dessiner la moitié droite en bleu
            if(barriers[i].health == 0){
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
            }else if (barriers[i].health == -10 || barriers[i].health == -20){
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            }
            SDL_Rect rightHalf = {
                destroyedRect.x + destroyedRect.w / 2,
                destroyedRect.y,
                destroyedRect.w / 2,
                destroyedRect.h
            };
            SDL_RenderFillRect(renderer, &rightHalf);

            // Dessiner un "+" noir épais au milieu
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Noir
            int centerX = destroyedRect.x + destroyedRect.w / 2;
            int centerY = destroyedRect.y + destroyedRect.h / 2;
            int lineLength = destroyedRect.w / 4;
            int thickness = 3; // Épaisseur du plus

            // Dessiner la ligne horizontale épaisse
            for (int t = -thickness; t <= thickness; t++) {
                SDL_RenderDrawLine(renderer, centerX - lineLength, centerY + t, centerX + lineLength, centerY + t);
            }

            // Dessiner la ligne verticale épaisse
            for (int t = -thickness; t <= thickness; t++) {
                SDL_RenderDrawLine(renderer, centerX + t, centerY - lineLength, centerX + t, centerY + lineLength);
            }
        }
    }
}

void drawBarriers(SDL_Renderer *renderer) {
    for (int i = 0; i < NUM_BARRIERS; i++) {
        if (barriers[i].health > 0) {  // Dessiner seulement si la barrière est en vie
            SDL_Rect barrierRect = {
                barriers[i].x,
                barriers[i].y,
                barriers[i].width,
                barriers[i].visualHeight // Utiliser la hauteur visuelle
            };
            SDL_RenderCopy(renderer, barrierTexture, NULL, &barrierRect);
        }
    }
}

void drawTurrets(SDL_Renderer *renderer) {
    for (int i = 0; i < MAX_TURRETS; i++) {
        if (turrets[i].active) {
            SDL_Rect turretRect = {
                turrets[i].x,
                turrets[i].y,
                turrets[i].width,
                turrets[i].height
            };
            SDL_RenderCopy(renderer, turretTexture, NULL, &turretRect);
        }
    }
}


void drawTurretLasers(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);  // Couleur jaune pour les lasers des tourelles
    for (int i = 0; i < MAX_TURRET_LASERS; i++) {
        if (turretLasers[i].active) {
            SDL_Rect laserRect = {
                turretLasers[i].x,
                turretLasers[i].y,
                LASER_WIDTH,
                LASER_HEIGHT
            };
            SDL_RenderFillRect(renderer, &laserRect);
        }
    }
}