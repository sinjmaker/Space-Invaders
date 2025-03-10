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
        // Vérifier si la barrière a été remplacée par une tourelle
        bool isTurretActive = (i < MAX_TURRETS && turrets[i].active);

        // Ne dessiner les carrés que si la barrière est détruite et qu'il n'y a pas de tourelle active
        if (barriers[i].health <= 0 && barriers[i].health != -1 && !isTurretActive) {
            SDL_Rect destroyedRect = {
                barriers[i].x + barriers[i].width / 4,  // Position X (centré sur la barrière)
                barriers[i].y + barriers[i].height / 4, // Position Y (centré sur la barrière)
                barriers[i].width / 2,                 // Largeur du rectangle
                barriers[i].visualHeight / 2           // Hauteur du rectangle
            };

            // Dessiner la moitié gauche en vert
            if (barriers[i].health > -11) {
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            } else if (barriers[i].health == -20) {
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
            if (barriers[i].health == 0) {
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
            } else if (barriers[i].health == -10 || barriers[i].health == -20) {
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

void drawScore(SDL_Renderer *renderer, TTF_Font *font) {
    char scoreText[20];
    snprintf(scoreText, sizeof(scoreText), "Score: %d", score);

    // Couleur du texte (blanc)
    SDL_Color textColor = {255, 255, 255, 255};

    // Créer une surface avec le texte
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, scoreText, textColor);
    if (!textSurface) {
        fprintf(stderr, "Erreur de création de la surface de texte: %s\n", TTF_GetError());
        return;
    }

    // Créer une texture à partir de la surface
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);
    if (!textTexture) {
        fprintf(stderr, "Erreur de création de la texture de texte: %s\n", SDL_GetError());
        return;
    }

    // Position du score en haut à droite
    int textWidth = 0, textHeight = 0;
    SDL_QueryTexture(textTexture, NULL, NULL, &textWidth, &textHeight);
    SDL_Rect textRect = {
        screenWidth - textWidth - 20,  // 20 pixels depuis le bord droit
        20,                            // 20 pixels depuis le haut
        textWidth,
        textHeight
    };

    // Dessiner la texture du texte
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    // Libérer la texture
    SDL_DestroyTexture(textTexture);
}

void drawGameOverScreen(SDL_Renderer *renderer, TTF_Font *font) {
    // Effacer l'écran
    // SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Fond noir
    // SDL_RenderClear(renderer);

    // Préparer le texte "Game Over"
    SDL_Color textColor = {255, 255, 255, 255};  // Texte blanc
    SDL_Surface *gameOverSurface = TTF_RenderText_Solid(font, "Game Over", textColor);
    if (!gameOverSurface) {
        fprintf(stderr, "Erreur de création de la surface 'Game Over': %s\n", TTF_GetError());
        return;
    }

    SDL_Texture *gameOverTexture = SDL_CreateTextureFromSurface(renderer, gameOverSurface);
    SDL_FreeSurface(gameOverSurface);
    if (!gameOverTexture) {
        fprintf(stderr, "Erreur de création de la texture 'Game Over': %s\n", SDL_GetError());
        return;
    }

    // Positionner "Game Over" au centre de l'écran
    int gameOverWidth, gameOverHeight;
    SDL_QueryTexture(gameOverTexture, NULL, NULL, &gameOverWidth, &gameOverHeight);
    SDL_Rect gameOverRect = {
        (screenWidth - gameOverWidth) / 2,  // Centré horizontalement
        (screenHeight - gameOverHeight) / 3, // 1/3 de la hauteur de l'écran
        gameOverWidth,
        gameOverHeight
    };

    // Dessiner "Game Over"
    SDL_RenderCopy(renderer, gameOverTexture, NULL, &gameOverRect);

    // Préparer le texte du score
    char scoreText[50];
    snprintf(scoreText, sizeof(scoreText), "Score: %d", score);

    SDL_Surface *scoreSurface = TTF_RenderText_Solid(font, scoreText, textColor);
    if (!scoreSurface) {
        fprintf(stderr, "Erreur de création de la surface du score: %s\n", TTF_GetError());
        SDL_DestroyTexture(gameOverTexture);
        return;
    }

    SDL_Texture *scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
    SDL_FreeSurface(scoreSurface);
    if (!scoreTexture) {
        fprintf(stderr, "Erreur de création de la texture du score: %s\n", SDL_GetError());
        SDL_DestroyTexture(gameOverTexture);
        return;
    }

    // Positionner le score en dessous de "Game Over"
    int scoreWidth, scoreHeight;
    SDL_QueryTexture(scoreTexture, NULL, NULL, &scoreWidth, &scoreHeight);
    SDL_Rect scoreRect = {
        (screenWidth - scoreWidth) / 2,  // Centré horizontalement
        gameOverRect.y + gameOverRect.h + 20, // 20 pixels en dessous de "Game Over"
        scoreWidth,
        scoreHeight
    };

    // Dessiner le score
    SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);

    // Libérer les textures
    SDL_DestroyTexture(gameOverTexture);
    SDL_DestroyTexture(scoreTexture);

    // Mettre à jour l'écran
    SDL_RenderPresent(renderer);
}

void drawTextInputScreen(SDL_Renderer *renderer, TTF_Font *font, const char *playerName, bool showError) {
    // Effacer l'écran
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Fond noir
    SDL_RenderClear(renderer);

    // Afficher le message "Entrez votre nom :"
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface *messageSurface = TTF_RenderText_Solid(font, "Entrez votre nom :", white);
    if (!messageSurface) {
        fprintf(stderr, "Erreur de création de la surface du message: %s\n", TTF_GetError());
        return;
    }

    SDL_Texture *messageTexture = SDL_CreateTextureFromSurface(renderer, messageSurface);
    SDL_FreeSurface(messageSurface);
    if (!messageTexture) {
        fprintf(stderr, "Erreur de création de la texture du message: %s\n", SDL_GetError());
        return;
    }

    // Positionner le message au centre de l'écran
    int messageWidth, messageHeight;
    SDL_QueryTexture(messageTexture, NULL, NULL, &messageWidth, &messageHeight);
    SDL_Rect messageRect = {
        (screenWidth - messageWidth) / 2,  // Centré horizontalement
        screenHeight / 2 - 50,             // 50 pixels au-dessus du centre
        messageWidth,
        messageHeight
    };

    // Dessiner le message
    SDL_RenderCopy(renderer, messageTexture, NULL, &messageRect);

    // Afficher le nom du joueur ou un texte par défaut
    const char *displayText = (strlen(playerName) > 0) ? playerName : "...";
    SDL_Surface *nameSurface = TTF_RenderText_Solid(font, displayText, white);
    if (!nameSurface) {
        fprintf(stderr, "Erreur de création de la surface du nom: %s\n", TTF_GetError());
        SDL_DestroyTexture(messageTexture);
        return;
    }

    SDL_Texture *nameTexture = SDL_CreateTextureFromSurface(renderer, nameSurface);
    SDL_FreeSurface(nameSurface);
    if (!nameTexture) {
        fprintf(stderr, "Erreur de création de la texture du nom: %s\n", SDL_GetError());
        SDL_DestroyTexture(messageTexture);
        return;
    }

    // Positionner le nom du joueur en dessous du message
    int nameWidth, nameHeight;
    SDL_QueryTexture(nameTexture, NULL, NULL, &nameWidth, &nameHeight);
    SDL_Rect nameRect = {
        (screenWidth - nameWidth) / 2,  // Centré horizontalement
        screenHeight / 2,               // Au centre vertical
        nameWidth,
        nameHeight
    };

    // Dessiner le nom du joueur ou le texte par défaut
    SDL_RenderCopy(renderer, nameTexture, NULL, &nameRect);

    // Afficher un message d'erreur si nécessaire
    if (showError) {
        SDL_Color red = {255, 0, 0, 255};  // Couleur rouge pour le message d'erreur
        SDL_Surface *errorSurface = TTF_RenderText_Solid(font, "Veuillez entrer un nom valide !", red);
        if (!errorSurface) {
            fprintf(stderr, "Erreur de création de la surface d'erreur: %s\n", TTF_GetError());
        } else {
            SDL_Texture *errorTexture = SDL_CreateTextureFromSurface(renderer, errorSurface);
            SDL_FreeSurface(errorSurface);
            if (!errorTexture) {
                fprintf(stderr, "Erreur de création de la texture d'erreur: %s\n", SDL_GetError());
            } else {
                // Positionner le message d'erreur en dessous du nom
                int errorWidth, errorHeight;
                SDL_QueryTexture(errorTexture, NULL, NULL, &errorWidth, &errorHeight);
                SDL_Rect errorRect = {
                    (screenWidth - errorWidth) / 2,  // Centré horizontalement
                    nameRect.y + nameRect.h + 10,    // 10 pixels en dessous du nom
                    errorWidth,
                    errorHeight
                };

                // Dessiner le message d'erreur
                SDL_RenderCopy(renderer, errorTexture, NULL, &errorRect);
                SDL_DestroyTexture(errorTexture);
            }
        }
    }

    // Libérer les textures
    SDL_DestroyTexture(messageTexture);
    SDL_DestroyTexture(nameTexture);

    // Mettre à jour l'écran
    SDL_RenderPresent(renderer);
}