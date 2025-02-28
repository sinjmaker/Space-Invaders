#include "game.h"
#include <stdlib.h>
#include <time.h>

Alien aliens[ALIEN_ROWS][ALIEN_COLUMNS];
Laser alienLasers[ALIEN_COLUMNS];
Laser playerLaser = {0, 0, false};
PlayerShip playerShip;
int direction = 1;
int screenWidth, screenHeight;
int alienWidth, alienHeight;
int moveDelayDynamic = MOVE_DELAY;
int tempoMove = MOVE_DELAY;
int nombre = 1;
bool movingLeft = false;
bool movingRight = false;
SDL_Texture *alienTexture = NULL;
SDL_Texture *alienTexture1 = NULL;
SDL_Texture *alienTexture2 = NULL;
SDL_Texture *alienTexture3 = NULL;
SDL_Texture *alienTexture4 = NULL;
SDL_Texture *persoTexture = NULL;
SDL_Texture *fondTexture = NULL;
SDL_Texture *cloudTexture = NULL;
SDL_Texture *pauseTexture = NULL;
int nombre_de_decente = 0;
int currentRound = 1;
float nuageChance=0;
int tempoRound = 1;
int tempoPaquet = 1;
float FIRE_CHANCE2 =FIRE_CHANCE;
Cloud revivalCloud = {0, 0, 0, 0, false, -1, MOVING_TO_TARGET};
bool isPaused = false;

void initAliens() {
    alienWidth = screenWidth / (ALIEN_COLUMNS * 2);
    alienHeight = screenHeight / (ALIEN_ROWS * 5);

    for (int i = 0; i < ALIEN_ROWS; i++) {
        for (int j = 0; j < ALIEN_COLUMNS; j++) {
            aliens[i][j].x = j * (alienWidth + 10);
            aliens[i][j].y = i * (alienHeight + 10);
            aliens[i][j].width = alienWidth;
            aliens[i][j].height = alienHeight;
            aliens[i][j].alive = true;
        }
    }

    for (int i = 0; i < ALIEN_COLUMNS; i++) {
        alienLasers[i].active = false;
    }
    playerLaser.active = false;
}

void initPlayerShip() {
    playerShip.width = screenWidth / 15;
    playerShip.height = screenHeight / 10;
    playerShip.x = screenWidth / 2 - playerShip.width / 2;
    playerShip.y = screenHeight - playerShip.height - 10;
    playerShip.lives = MAX_PLAYER_LIVES;
}

void initCloud() {
    revivalCloud.width = screenWidth / 10;
    revivalCloud.height = screenHeight / 15;
    revivalCloud.active = false;
}

int findDenseDeadZone() {
    int maxDead = 0, targetColumn = -1;
    for (int j = 0; j < ALIEN_COLUMNS; j++) {
        int deadCount = 0;
        for (int i = 0; i < ALIEN_ROWS; i++) {
            if (!aliens[i][j].alive) {
                deadCount++;
            }
        }
        if (deadCount > maxDead) {
            maxDead = deadCount;
            targetColumn = j;
        }
    }
    return (maxDead > 2) ? targetColumn : -1;
}

void spawnCloud() {
    if (!revivalCloud.active && (rand() / (double)RAND_MAX) < nuageChance) {
        int targetColumn = findDenseDeadZone();
        if (targetColumn != -1) {
            revivalCloud.x = aliens[1][targetColumn].x;
            revivalCloud.y = -revivalCloud.height;
            revivalCloud.width = screenWidth / 10;
            revivalCloud.height = screenHeight / 15;
            revivalCloud.active = true;
            revivalCloud.targetColumn = targetColumn;
            revivalCloud.state = MOVING_TO_TARGET;
        }
    }
}

void updateCloud() {
    if (revivalCloud.active) {
        revivalCloud.x = aliens[1][revivalCloud.targetColumn].x;
        switch (revivalCloud.state) {
            case MOVING_TO_TARGET:
                // Déplacement vers la colonne cible
                revivalCloud.y += 5;  // Descend lentement
                if (revivalCloud.y >= (nombre_de_decente * ((3 * alienHeight) / 4) + (ALIEN_ROWS * (alienHeight + 10)))) {
                    revivalCloud.state = REVIVING;
                }
                break;

            case REVIVING:
                // Réanimer les aliens morts dans la colonne cible
                for (int i = 0; i < ALIEN_ROWS; i++) {
                    if (!aliens[i][revivalCloud.targetColumn].alive) {
                        aliens[i][revivalCloud.targetColumn].alive = true;
                    }
                }
                revivalCloud.state = LEAVING;
                break;

            case LEAVING:
                // Le nuage repart vers le haut
                revivalCloud.y -= 5;  // Remonte lentement
                if (revivalCloud.y + revivalCloud.height < 0) {
                    revivalCloud.active = false;
                }
                break;

            default:
                // Gestion des cas non prévus (optionnel)
                break;
        }
    }
}

void updateAliens(int *offsetY) {
    bool needToDescend = false;
    for (int i = 0; i < ALIEN_ROWS; i++) {
        for (int j = 0; j < ALIEN_COLUMNS; j++) {
            if (aliens[i][j].alive) {
                if ((direction == 1 && aliens[i][j].x + alienWidth >= screenWidth) || (direction == -1 && aliens[i][j].x <= 0)) {
                    needToDescend = true;
                }
            }
        }
    }

    if (needToDescend) {
        if(nombre == 1 || nombre==2){
            nombre=3;
        }else{
            nombre=1;
        }
        nombre_de_decente++;
        direction *= -1;
        if (tempoRound == currentRound){
            *offsetY += alienHeight;
        } else{
            tempoRound = currentRound;
            *offsetY =0;
        }
        for (int i = 0; i < ALIEN_ROWS; i++) {
            for (int j = 0; j < ALIEN_COLUMNS; j++) {
                aliens[i][j].y += (3*alienHeight) / 4;
            }
        }
        switch (currentRound % 3) {
            case 1:
                if(tempoMove <= MOVE_DELAY){
                    moveDelayDynamic -= 10;
                }
                break;
            case 2:
                if(tempoMove <= MOVE_DELAY - 40){
                    moveDelayDynamic -= 10;
                }
                break;
            case 0:
                if(tempoMove <= MOVE_DELAY - 80){
                    moveDelayDynamic -= 10;
                }
                break;
        }
        tempoMove -= 10;
    } else {
        for (int i = 0; i < ALIEN_ROWS; i++) {
            for (int j = 0; j < ALIEN_COLUMNS; j++) {
                aliens[i][j].x += direction * 10;
            }
        }
        if(direction == 1){
            if (nombre == 1){
                alienTexture = alienTexture2;
                nombre = 2;
            }else if (nombre == 2){
                alienTexture = alienTexture1;
                nombre = 1;
            }
        }else if(direction == -1){
            if (nombre == 3){
                alienTexture = alienTexture4;
                nombre = 4;
            }else if (nombre == 4){
                alienTexture = alienTexture3;
                nombre = 3;
            }
        }
    }
}

void updateLasers() {
    for (int i = 0; i < ALIEN_COLUMNS; i++) {
        if (alienLasers[i].active) {
            alienLasers[i].y += LASER_SPEED;
            if (alienLasers[i].y > screenHeight) {
                alienLasers[i].active = false;
            } else if (
                alienLasers[i].x < playerShip.x + playerShip.width &&
                alienLasers[i].x + LASER_WIDTH > playerShip.x &&
                alienLasers[i].y < playerShip.y + playerShip.height &&
                alienLasers[i].y + LASER_HEIGHT > playerShip.y) {
                alienLasers[i].active = false;
                playerShip.lives--;
            }
        }
    }

    if (playerLaser.active) {
        playerLaser.y += PLAYER_LASER_SPEED;
        if (playerLaser.y < 0) {
            playerLaser.active = false;
        } else {
            for (int i = 0; i < ALIEN_ROWS; i++) {
                for (int j = 0; j < ALIEN_COLUMNS; j++) {
                    if (aliens[i][j].alive &&
                        playerLaser.x < aliens[i][j].x + aliens[i][j].width &&
                        playerLaser.x + LASER_WIDTH > aliens[i][j].x &&
                        playerLaser.y < aliens[i][j].y + aliens[i][j].height &&
                        playerLaser.y + LASER_HEIGHT > aliens[i][j].y) {
                        aliens[i][j].alive = false;
                        playerLaser.active = false;
                    }
                }
            }
        }
    }
}

void aliensFire() {
    for (int col = 0; col < ALIEN_COLUMNS; col++) {
        for (int row = ALIEN_ROWS - 1; row >= 0; row--) {
            if (aliens[row][col].alive) {
                if (!alienLasers[col].active && (rand() / (double)RAND_MAX) < FIRE_CHANCE2) {
                    alienLasers[col].x = aliens[row][col].x + alienWidth / 2 - LASER_WIDTH / 2;
                    alienLasers[col].y = aliens[row][col].y + alienHeight;
                    alienLasers[col].active = true;
                }
                break;
            }
        }
    }
}

int toujours_vivant_toujour_la_patate() {
    for (int i = 0; i < ALIEN_ROWS; i++) {
        for (int j = 0; j < ALIEN_COLUMNS; j++) {
            if (aliens[i][j].alive) {
                return 1;
            }
        }
    }
    return 0;
}

GameMode chooseGameMode(SDL_Renderer *renderer) {
    bool modeChoisi = false;
    GameMode mode = MODE_KILL_THEM_ALL;

    int buttonWidth = 300;
    int buttonHeight = 80;

    SDL_Rect killRect;
    killRect.w = buttonWidth;
    killRect.h = buttonHeight;
    killRect.x = (screenWidth / 2) - (buttonWidth / 2);
    killRect.y = (screenHeight / 2) - 100;

    SDL_Rect ffaRect;
    ffaRect.w = buttonWidth;
    ffaRect.h = buttonHeight;
    ffaRect.x = (screenWidth / 2) - (buttonWidth / 2);
    ffaRect.y = (screenHeight / 2) + 20;

    SDL_Event event;

    printf("=== MENU SIMPLE SDL2 ===\n");
    printf("Cliquez sur le rectangle du haut pour KILL THEM ALL\n");
    printf("Cliquez sur le rectangle du bas pour FFA\n");
    while (!modeChoisi) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                SDL_Quit();
                exit(0);
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mx = event.button.x;
                int my = event.button.y;
                if (mx >= killRect.x && mx <= killRect.x + killRect.w &&
                    my >= killRect.y && my <= killRect.y + killRect.h) {
                    mode = MODE_KILL_THEM_ALL;
                    modeChoisi = true;
                } else if (mx >= ffaRect.x && mx <= ffaRect.x + ffaRect.w &&
                           my >= ffaRect.y && my <= ffaRect.y + ffaRect.h) {
                    mode = MODE_FFA;
                    modeChoisi = true;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderFillRect(renderer, &killRect);

        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderFillRect(renderer, &ffaRect);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    return mode;
}

// void resetAliensForNewRound() {
//     for (int i = 0; i < ALIEN_ROWS; i++) {
//         for (int j = 0; j < ALIEN_COLUMNS; j++) {
//             aliens[i][j].x = j * (alienWidth + 10);
//             aliens[i][j].y = i * (alienHeight + 10);
//             aliens[i][j].alive = true;
//         }
//     }

//     // Augmenter la difficulté (par exemple, réduire le délai de mouvement)
//     moveDelayDynamic = MOVE_DELAY - (currentRound * 10); // Les aliens deviennent plus rapides
//     if (moveDelayDynamic < 50) {
//         moveDelayDynamic = 50; // Vitesse maximale
//     }

//     // Réinitialiser le nuage
//     revivalCloud.active = false;

//     direction = 1;
//     // int moveDelayDynamic = MOVE_DELAY;
//     nombre = 1;
//     nombre_de_decente = 0;

//     // Incrémenter le numéro de la manche
//     currentRound++;
// }

void resetAliensForNewRound() {
    for (int i = 0; i < ALIEN_ROWS; i++) {
        for (int j = 0; j < ALIEN_COLUMNS; j++) {
            aliens[i][j].x = j * (alienWidth + 10);
            aliens[i][j].y = i * (alienHeight + 10);
            aliens[i][j].alive = true;
        }
    }

    currentRound++;

    switch (currentRound % 3) {
        case 1:
            moveDelayDynamic = MOVE_DELAY;
            break;
        case 2:
            moveDelayDynamic = MOVE_DELAY - 40;
            break;
        case 0:
            moveDelayDynamic = MOVE_DELAY - 80;
            break;
    }

    if (moveDelayDynamic < 50) {
        moveDelayDynamic = 50;
    }

    switch (currentRound % 3) {
        case 1:
            nuageChance = 0.0;
            break;
        case 2:
            nuageChance = 0.001;
            break;
        case 0:
            nuageChance = 0.003;
            break;
    }
    if ((6*tempoPaquet)>currentRound && currentRound>(3*tempoPaquet)){
        FIRE_CHANCE2 = FIRE_CHANCE2*1.25;
        tempoPaquet++;
    }
    printf("%f\n",FIRE_CHANCE2);

    revivalCloud.active = false;

    direction = 1;
    nombre = 1;
    nombre_de_decente = 0;
    tempoMove = MOVE_DELAY;
}