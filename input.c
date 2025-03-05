#include "input.h"

void handlePlayerMovement() {
    if (movingLeft && playerShip.x > 0) {
        playerShip.x -= PLAYER_SPEED;
    }
    if (movingRight && playerShip.x + playerShip.width < screenWidth) {
        playerShip.x += PLAYER_SPEED;
    }
}

void handlePlayerInput(SDL_Event event) {
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_LEFT:
                movingLeft = true;
                break;
            case SDLK_RIGHT:
                movingRight = true;
                break;
            case SDLK_SPACE:
                if (!playerLaser.active) {
                    playerLaser.x = playerShip.x + playerShip.width / 2 - LASER_WIDTH / 2;
                    playerLaser.y = playerShip.y;
                    playerLaser.active = true;
                }
                break;
            case SDLK_p:
                isPaused = !isPaused;
                for (int i = 0; i < NUM_BARRIERS; i++) {
                    if (barriers[i].health == 0) {
                        if(playerShip.ecrous < 3){
                            barriers[i].health = -10;
                            printf("1\n");
                            if (playerShip.ecrous < 0){
                                printf("2\n");
                                barriers[i].health = -20;
                            }
                        }
                    }
                }
                break;
        }
    } else if (event.type == SDL_KEYUP) {
        switch (event.key.keysym.sym) {
            case SDLK_LEFT:
                movingLeft = false;
                break;
            case SDLK_RIGHT:
                movingRight = false;
                break;
        }
    } else if (event.type == SDL_MOUSEBUTTONDOWN) {
        handleMouseInput(event);  // Gérer les clics de souris
    }
}

void handleTextInput(SDL_Event event, char *playerName, bool *isEnterPressed) {
    if (event.type == SDL_TEXTINPUT) {
        // Limiter la longueur du nom à 10 caractères
        if (strlen(playerName) < 10) {
            strcat(playerName, event.text.text);
        }
    } else if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_BACKSPACE && strlen(playerName) > 0) {
            // Supprimer le dernier caractère
            playerName[strlen(playerName) - 1] = '\0';
        } else if (event.key.keysym.sym == SDLK_RETURN) {
            // Valider la saisie uniquement si au moins un caractère a été saisi
            if (strlen(playerName) > 0) {
                *isEnterPressed = true;
            }
        }
    }
}

void handleMouseInput(SDL_Event event) {
    if (event.type == SDL_MOUSEBUTTONDOWN && isPaused) {
        // Récupérer les coordonnées du clic
        int mouseX = event.button.x;
        int mouseY = event.button.y;

        // Réanimer la barrière si le clic est sur un carré vert
        for (int i = 0; i < NUM_BARRIERS; i++) {
            // Vérifier si la barrière a été remplacée par une tourelle
            bool isTurretActive = (i < MAX_TURRETS && turrets[i].active);

            // Ne gérer les clics que si la barrière est détruite et qu'il n'y a pas de tourelle active
            if (barriers[i].health <= 0 && barriers[i].health != -1 && !isTurretActive) {
                SDL_Rect destroyedRect = {
                    barriers[i].x + barriers[i].width / 4,
                    barriers[i].y + barriers[i].height / 4,
                    barriers[i].width / 4,
                    barriers[i].visualHeight / 2
                };

                SDL_Rect blueHalf = {
                    destroyedRect.x + destroyedRect.w,  // Partie droite (bleue)
                    destroyedRect.y,
                    destroyedRect.w,
                    destroyedRect.h
                };

                // Vérifier si le clic est dans le carré vert
                if (mouseX >= destroyedRect.x && mouseX <= destroyedRect.x + destroyedRect.w &&
                    mouseY >= destroyedRect.y && mouseY <= destroyedRect.y + destroyedRect.h && playerShip.ecrous > 0) {
                    barriers[i].health = 10;
                    playerShip.ecrous = playerShip.ecrous - 1;
                    break;
                }

                // Vérifier si le clic est dans la partie bleue
                if (mouseX >= blueHalf.x && mouseX <= blueHalf.x + blueHalf.w &&
                    mouseY >= blueHalf.y && mouseY <= blueHalf.y + blueHalf.h && playerShip.ecrous > 2) {
                    if (!turrets[i].existe) {
                        createTurret(i, barriers[i].x + barriers[i].width / 2, barriers[i].y, barriers[i].width, barriers[i].height);
                    } else {
                        turrets[i].active = true;
                        turrets[i].health = 10;
                    }
                    barriers[i].health = -1; // Marquer la barrière comme remplacée
                    playerShip.ecrous -= 3;
                }
            }
        }
    }
}