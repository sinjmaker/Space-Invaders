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
SDL_Texture *barrierTexture = NULL;
SDL_Texture *turretTexture = NULL;
SDL_Texture *ecrousTexture = NULL;
int nombre_de_decente = 0;
int currentRound = 1;
float nuageChance=0;
int tempoRound = 1;
int tempoPaquet = 1;
float FIRE_CHANCE2 =FIRE_CHANCE;
Cloud revivalCloud = {0, 0, 0, 0, false, -1, MOVING_TO_TARGET};
bool isPaused = false;
Barrier barriers[NUM_BARRIERS];
Turret turrets[MAX_TURRETS];
TurretLaser turretLasers[MAX_TURRET_LASERS];
int score = 0;

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
    playerShip.ecrous = 10;
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
    // Tirs des aliens
    for (int i = 0; i < ALIEN_COLUMNS; i++) {
        if (alienLasers[i].active) {
            alienLasers[i].y += LASER_SPEED;

            // Collision avec les barrières
            for (int j = 0; j < NUM_BARRIERS; j++) {
                if (barriers[j].health > 0 &&
                    alienLasers[i].x < barriers[j].x + barriers[j].width &&
                    alienLasers[i].x + LASER_WIDTH > barriers[j].x &&
                    alienLasers[i].y < barriers[j].y + barriers[j].height &&
                    alienLasers[i].y + LASER_HEIGHT > barriers[j].y) {
                    alienLasers[i].active = false;
                    barriers[j].health--;
                }
            }

            for (int j = 0; j < MAX_TURRETS; j++) {
                if (turrets[j].active &&
                    alienLasers[i].x < turrets[j].x + turrets[j].width &&
                    alienLasers[i].x + LASER_WIDTH > turrets[j].x &&
                    alienLasers[i].y < turrets[j].y + turrets[j].height &&
                    alienLasers[i].y + LASER_HEIGHT > turrets[j].y) {
                    alienLasers[i].active = false;
                    turrets[j].health--;

                    // Désactiver la tourelle si elle n'a plus de points de vie
                    if (turrets[j].health <= 0) {
                        turrets[j].active = false;
                    }
                }
            }

            // Collision avec le joueur
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

    // Tir du joueur
    if (playerLaser.active) {
        playerLaser.y += PLAYER_LASER_SPEED;

        // Collision avec les barrières
        for (int j = 0; j < NUM_BARRIERS; j++) {
            if (barriers[j].health > 0 &&
                playerLaser.x < barriers[j].x + barriers[j].width &&
                playerLaser.x + LASER_WIDTH > barriers[j].x &&
                playerLaser.y < barriers[j].y + barriers[j].height &&
                playerLaser.y + LASER_HEIGHT > barriers[j].y) {
                playerLaser.active = false;
                barriers[j].health--;
            }
        }

        for (int j = 0; j < MAX_TURRETS; j++) {
            if (turrets[j].active &&
                playerLaser.x < turrets[j].x + turrets[j].width &&
                playerLaser.x + LASER_WIDTH > turrets[j].x &&
                playerLaser.y < turrets[j].y + turrets[j].height &&
                playerLaser.y + LASER_HEIGHT > turrets[j].y) {
                playerLaser.active = false;
                turrets[j].health--;

                // Désactiver la tourelle si elle n'a plus de points de vie
                if (turrets[j].health <= 0) {
                    turrets[j].active = false;
                }
            }
        }

        // Collision avec les aliens
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
                        score += 10;
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
    // Initialisation de SDL_ttf
    if (TTF_Init() == -1) {
        fprintf(stderr, "Erreur d'initialisation de SDL_ttf: %s\n", TTF_GetError());
        exit(1);
    }
    // Initialisation de SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        fprintf(stderr, "Erreur d'initialisation de SDL_mixer: %s\n", Mix_GetError());
        exit(1);
    }
    // Chargement de la musique du menu
    Mix_Music *menuMusic = Mix_LoadMUS("MusiqueMenu.mp3");
    if (!menuMusic) {
        fprintf(stderr, "Erreur lors du chargement de la musique: %s\n", Mix_GetError());
        exit(1);
    }
    // Démarrage de la musique en boucle (-1)
    if (Mix_PlayMusic(menuMusic, -1) == -1) {
        fprintf(stderr, "Erreur lors de la lecture de la musique: %s\n", Mix_GetError());
    }
    
    // Chargement de la police pour les boutons (taille 24)
    TTF_Font *font = TTF_OpenFont("arial.ttf", 24);
    if (!font) {
        fprintf(stderr, "Erreur lors du chargement de la police: %s\n", TTF_GetError());
        exit(1);
    }
    // Chargement de la police pour le titre (taille 48)
    TTF_Font *fontTitle = TTF_OpenFont("arial.ttf", 100);
    if (!fontTitle) {
        fprintf(stderr, "Erreur lors du chargement de la police pour le titre: %s\n", TTF_GetError());
        exit(1);
    }
    
    // Création de la texture pour le titre
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface *titleSurface = TTF_RenderText_Blended(fontTitle, "SEAGULL INVADERS", white);
    if (!titleSurface) {
        fprintf(stderr, "Erreur lors du rendu du titre: %s\n", TTF_GetError());
        exit(1);
    }
    SDL_Texture *titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
    SDL_FreeSurface(titleSurface);
    int titleWidth, titleHeight;
    SDL_QueryTexture(titleTexture, NULL, NULL, &titleWidth, &titleHeight);
    // Positionner le titre centré en haut (20 pixels du haut)
    SDL_Rect titleRect = { (screenWidth - titleWidth) / 2, 100, titleWidth, titleHeight };
    
    bool modeChoisi = false;
    GameMode mode = MODE_KILL_THEM_ALL;
    int buttonWidth = 300;
    int buttonHeight = 80;
    
    SDL_Rect killRect = { (screenWidth / 2) - (buttonWidth / 2), (screenHeight / 2) - 100, buttonWidth, buttonHeight };
    SDL_Rect QuitterRect = { (screenWidth / 2) - (buttonWidth / 2), (screenHeight / 2) + 20, buttonWidth, buttonHeight };
    
    SDL_Event event;
    int mouseX, mouseY;
    
    // Création des textures pour le texte des boutons
    SDL_Surface *killSurface = TTF_RenderText_Blended(font, "JOUER", white);
    if (!killSurface) {
        fprintf(stderr, "Erreur lors du rendu du texte: %s\n", TTF_GetError());
    }
    SDL_Texture *killTextTexture = SDL_CreateTextureFromSurface(renderer, killSurface);
    SDL_FreeSurface(killSurface);
    int killTextWidth, killTextHeight;
    SDL_QueryTexture(killTextTexture, NULL, NULL, &killTextWidth, &killTextHeight);
    SDL_Rect killTextRect = { killRect.x + (buttonWidth - killTextWidth) / 2,
                              killRect.y + (buttonHeight - killTextHeight) / 2,
                              killTextWidth, killTextHeight };
    
    SDL_Surface *QuitterSurface = TTF_RenderText_Blended(font, "QUITTER", white);
    if (!QuitterSurface) {
        fprintf(stderr, "Erreur lors du rendu du texte: %s\n", TTF_GetError());
    }
    SDL_Texture *QuitterTextTexture = SDL_CreateTextureFromSurface(renderer, QuitterSurface);
    SDL_FreeSurface(QuitterSurface);
    int QuitterTextWidth, QuitterTextHeight;
    SDL_QueryTexture(QuitterTextTexture, NULL, NULL, &QuitterTextWidth, &QuitterTextHeight);
    SDL_Rect QuitterTextRect = { QuitterRect.x + (buttonWidth - QuitterTextWidth) / 2,
                                 QuitterRect.y + (buttonHeight - QuitterTextHeight) / 2,
                                 QuitterTextWidth, QuitterTextHeight };
    
    while (!modeChoisi) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                Mix_HaltMusic();
                Mix_FreeMusic(menuMusic);
                Mix_CloseAudio();
                SDL_Quit();
                exit(0);
            } else if (event.type == SDL_MOUSEMOTION) {
                SDL_GetMouseState(&mouseX, &mouseY);
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mx = event.button.x;
                int my = event.button.y;
                if (mx >= killRect.x && mx <= killRect.x + killRect.w &&
                    my >= killRect.y && my <= killRect.y + killRect.h) {
                    mode = MODE_KILL_THEM_ALL;
                    modeChoisi = true;
                    // Arrêter la musique lorsque le jeu commence
                    Mix_HaltMusic();
                    Mix_FreeMusic(menuMusic);
                    Mix_CloseAudio();
                } else if (mx >= QuitterRect.x && mx <= QuitterRect.x + QuitterRect.w &&
                           my >= QuitterRect.y && my <= QuitterRect.y + QuitterRect.h) {
                    // Arrêter la musique et quitter
                    Mix_HaltMusic();
                    Mix_FreeMusic(menuMusic);
                    Mix_CloseAudio();
                    SDL_Quit();
                    exit(0);
                }
            }
        }
        
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, fondTexture, NULL, NULL);
        
        // Affichage du titre en haut au centre
        SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);
        
        bool hoverKill = false;
        bool hoverQuitter = false;
        SDL_GetMouseState(&mouseX, &mouseY);
        if (mouseX >= killRect.x && mouseX <= killRect.x + killRect.w &&
            mouseY >= killRect.y && mouseY <= killRect.y + killRect.h) {
            hoverKill = true;
        }
        if (mouseX >= QuitterRect.x && mouseX <= QuitterRect.x + QuitterRect.w &&
            mouseY >= QuitterRect.y && mouseY <= QuitterRect.y + QuitterRect.h) {
            hoverQuitter = true;
        }
        
        if (hoverKill) {
            SDL_SetRenderDrawColor(renderer, 255, 100, 100, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 200, 50, 50, 255);
        }
        SDL_RenderFillRect(renderer, &killRect);
        
        if (hoverQuitter) {
            SDL_SetRenderDrawColor(renderer, 100, 255, 100, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 50, 200, 50, 255);
        }
        SDL_RenderFillRect(renderer, &QuitterRect);
        
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &killRect);
        SDL_RenderDrawRect(renderer, &QuitterRect);
        
        SDL_RenderCopy(renderer, killTextTexture, NULL, &killTextRect);
        SDL_RenderCopy(renderer, QuitterTextTexture, NULL, &QuitterTextRect);
        
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    
    SDL_DestroyTexture(titleTexture);
    SDL_DestroyTexture(killTextTexture);
    SDL_DestroyTexture(QuitterTextTexture);
    TTF_CloseFont(font);
    TTF_CloseFont(fontTitle);
    TTF_Quit();
    
    return mode;
}

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
    playerShip.ecrous++;
}

void initBarriers() {
    int barrierWidth = screenWidth / 20;  // Largeur d'une barrière
    int barrierSpacing = screenWidth / (NUM_BARRIERS + 1); // Espacement entre les barrières

    for (int i = 0; i < NUM_BARRIERS; i++) {
        barriers[i].x = barrierSpacing * (i + 1) - barrierWidth / 2;
        barriers[i].y = screenHeight - 200; // Position Y des barrières (au-dessus du joueur)
        barriers[i].width = barrierWidth;
        barriers[i].height = 50; // Hauteur de la zone active pour les collisions
        barriers[i].visualHeight = screenHeight - barriers[i].y; // Hauteur visuelle jusqu'au bas de l'écran
        barriers[i].health = 10; // Points de vie de chaque barrière
    }
}

void reviveBarrier(int mouseX, int mouseY) {
    for (int i = 0; i < NUM_BARRIERS; i++) {
        if (barriers[i].health <= 0) {  // Vérifier si la barrière est détruite
            SDL_Rect destroyedRect = {
                barriers[i].x + barriers[i].width / 4,  // Position X du carré vert
                barriers[i].y + barriers[i].height / 4, // Position Y du carré vert
                barriers[i].width / 2,                 // Largeur du carré vert
                barriers[i].visualHeight / 2           // Hauteur du carré vert
            };

            // Vérifier si le clic est dans le carré vert
            if (mouseX >= destroyedRect.x && mouseX <= destroyedRect.x + destroyedRect.w &&
                mouseY >= destroyedRect.y && mouseY <= destroyedRect.y + destroyedRect.h) {
                barriers[i].health = 10;  // Réanimer la barrière
                break;  // Sortir de la boucle après avoir réanimé une barrière
            }
        }
    }
}

void initTurrets() {
    for (int i = 0; i < MAX_TURRETS; i++) {
        turrets[i].active = false;
        turrets[i].health = 10;
        turrets[i].existe = false;
    }
}

void initTurretLasers() {
    for (int i = 0; i < MAX_TURRET_LASERS; i++) {
        turretLasers[i].active = false;  // Désactiver tous les lasers au départ
    }
}

void createTurretLaser(int x, int y) {
    for (int i = 0; i < MAX_TURRET_LASERS; i++) {
        if (!turretLasers[i].active) {  // Trouver un laser inactif
            turretLasers[i].x = x;
            turretLasers[i].y = y;
            turretLasers[i].active = true;
            turretLasers[i].speed = -10;  // Vitesse du laser (vers le haut)
            break;
        }
    }
}

void createTurret(int barrierIndex, int x, int y, int width, int height) {
    if (barrierIndex < 0 || barrierIndex >= MAX_TURRETS) return;

    turrets[barrierIndex].x = x - width / 2;
    turrets[barrierIndex].y = y;
    turrets[barrierIndex].width = width;
    turrets[barrierIndex].height = screenHeight - y;
    turrets[barrierIndex].active = true;
    turrets[barrierIndex].lastShotTime = SDL_GetTicks();
    turrets[barrierIndex].existe = true;
}

void updateTurrets() {
    Uint32 currentTime = SDL_GetTicks();

    for (int i = 0; i < MAX_TURRETS; i++) {
        if (turrets[i].active) {
            // Vérifier si la tourelle peut tirer
            if (currentTime - turrets[i].lastShotTime > 1000) {  // Toutes les 1 seconde
                // Trouver l'alien le plus proche dans la colonne de la tourelle
                int targetX = turrets[i].x + turrets[i].width / 2;  // Centre de la tourelle
                int targetY = -1;

                // Parcourir les aliens de bas en haut dans la colonne correspondante
                for (int row = ALIEN_ROWS - 1; row >= 0; row--) {
                    int col = (targetX - aliens[row][0].x) / (alienWidth + 10);  // Trouver la colonne de l'alien
                    if (col >= 0 && col < ALIEN_COLUMNS && aliens[row][col].alive) {
                        targetY = aliens[row][col].y + aliens[row][col].height / 2;
                        break;
                    }
                }

                // Tirer un laser si un alien est trouvé
                if (targetY != -1) {
                    createTurretLaser(turrets[i].x + turrets[i].width / 2, turrets[i].y);
                    turrets[i].lastShotTime = currentTime;  // Mettre à jour le temps du dernier tir
                }
            }
        }
        else if(barriers[i].health == -1 && !turrets[i].active){
            barriers[i].health = 0;
        }
    }
}

void updateTurretLasers() {
    for (int i = 0; i < MAX_TURRET_LASERS; i++) {
        if (turretLasers[i].active) {
            // Déplacer le laser
            turretLasers[i].y += turretLasers[i].speed;

            // Vérifier les collisions avec les aliens
            for (int row = 0; row < ALIEN_ROWS; row++) {
                for (int col = 0; col < ALIEN_COLUMNS; col++) {
                    if (aliens[row][col].alive &&
                        turretLasers[i].x < aliens[row][col].x + aliens[row][col].width &&
                        turretLasers[i].x + LASER_WIDTH > aliens[row][col].x &&
                        turretLasers[i].y < aliens[row][col].y + aliens[row][col].height &&
                        turretLasers[i].y + LASER_HEIGHT > aliens[row][col].y) {
                        aliens[row][col].alive = false;  // Détruire l'alien
                        turretLasers[i].active = false;  // Désactiver le laser
                        break;
                    }
                }
            }

            // Désactiver le laser s'il sort de l'écran
            if (turretLasers[i].y < 0) {
                turretLasers[i].active = false;
            }
        }
    }
}


void saveHighScore(const char* name, int score) {
    FILE* file = fopen("scores.dat", "a+");
    if (!file) return;

    // Lire les scores existants
    HighScore scores[MAX_HIGH_SCORES + 1] = {0};
    int count = 0;
    
    while (count < MAX_HIGH_SCORES && fscanf(file, "%50s %d", 
           scores[count].name, &scores[count].score) == 2) {
        count++;
    }
    
    // Ajouter le nouveau score
    strncpy(scores[count].name, name, 50);
    scores[count].score = score;
    count++;

    // Trier par ordre décroissant
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (scores[j].score < scores[j + 1].score) {
                HighScore temp = scores[j];
                scores[j] = scores[j + 1];
                scores[j + 1] = temp;
            }
        }
    }

    // Réécrire les 5 premiers
    freopen("scores.dat", "w", file);
    for (int i = 0; i < (count > MAX_HIGH_SCORES ? MAX_HIGH_SCORES : count); i++) {
        fprintf(file, "%s %d\n", scores[i].name, scores[i].score);
    }
    fclose(file);
}

void loadHighScores(HighScore scores[]) {
    FILE* file = fopen("scores.dat", "r");
    if (!file) return;

    for (int i = 0; i < MAX_HIGH_SCORES; i++) {
        if (fscanf(file, "%50s %d", scores[i].name, &scores[i].score) != 2) {
            strcpy(scores[i].name, "---");
            scores[i].score = 0;
        }
    }
    fclose(file);
}

void resetGame() {
    // Réinitialiser les aliens
    initAliens();

    // Réinitialiser le vaisseau du joueur
    initPlayerShip();

    // Réinitialiser les barrières
    initBarriers();

    // Réinitialiser les tourelles
    initTurrets();

    // Réinitialiser les lasers des tourelles
    initTurretLasers();

    // Réinitialiser le score et le round
    score = 0;
    currentRound = 1;

    // Réinitialiser le nuage de réanimation
    revivalCloud.active = false;

    // Réinitialiser les variables de mouvement
    direction = 1;
    nombre = 1;
    nombre_de_decente = 0;
    moveDelayDynamic = MOVE_DELAY;
    tempoMove = MOVE_DELAY;

    // Réinitialiser les états de pause
    isPaused = false;

    // Réinitialiser les textures (si nécessaire)
    // (Assurez-vous que les textures sont déjà chargées, sinon rechargez-les)
}