// alienTexture1 = loadTexture(renderer, "mouetteDB.BMP");
// alienTexture2 = loadTexture(renderer, "mouetteDH.BMP");
// alienTexture3 = loadTexture(renderer, "mouetteGB.BMP");
// alienTexture4 = loadTexture(renderer, "mouetteGH.BMP");

//gcc -Wall main5.c -o main -lSDL2
//./main

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define ALIEN_ROWS 3
#define ALIEN_COLUMNS 10
#define MOVE_DELAY 100
#define LASER_SPEED 10
#define LASER_WIDTH 5
#define LASER_HEIGHT 20
#define FIRE_CHANCE 0.01
#define PLAYER_LASER_SPEED -15
#define PLAYER_SPEED 10
#define MAX_PLAYER_LIVES 30

typedef struct {
    int x, y;
    bool active;
} Laser;

typedef struct {
    int x, y;
    int width, height;
    bool alive;
} Alien;

typedef struct {
    int x, y;
    int width, height;
    int lives;
} PlayerShip;

Alien aliens[ALIEN_ROWS][ALIEN_COLUMNS];
Laser alienLasers[ALIEN_COLUMNS];
Laser playerLaser = {0, 0, false};
PlayerShip playerShip;
int direction = 1;
int screenWidth, screenHeight;
int alienWidth, alienHeight;
int moveDelayDynamic = MOVE_DELAY;

bool movingLeft = false;
bool movingRight = false;

SDL_Texture *alienTexture = NULL;
SDL_Texture *alienTexture1 = NULL;
SDL_Texture *alienTexture2 = NULL;
SDL_Texture *alienTexture3 = NULL;
SDL_Texture *alienTexture4 = NULL;
int nombre = 1;
SDL_Texture *persoTexture = NULL;
SDL_Texture *fondTexture = NULL;
SDL_Texture *cloudTexture = NULL;

int nombre_de_decente=0;


typedef struct {
    int x, y;
    int width, height;
    bool active;
    int targetColumn;  // Colonne cible
    enum {MOVING_TO_TARGET, REVIVING, LEAVING} state;
} Cloud;

Cloud revivalCloud = {0, 0, 0, 0, false, -1, MOVING_TO_TARGET};


typedef enum {
    MODE_KILL_THEM_ALL,
    MODE_FFA
} GameMode;

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
    return (maxDead > 2) ? targetColumn : -1;  // Au moins 3 morts nécessaires
}

void spawnCloud() {
    if (!revivalCloud.active && (rand() / (double)RAND_MAX) < 0.001) {  // 0.1% de chance par frame
        int targetColumn = findDenseDeadZone();
        if (targetColumn != -1) {
            revivalCloud.x = aliens[1][targetColumn].x;
            revivalCloud.y = -revivalCloud.height;  // Commence en haut de l'écran
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
                if (revivalCloud.y >= (nombre_de_decente * ((3*alienHeight) / 4) + (ALIEN_ROWS * (alienHeight + 10)))) {
                    revivalCloud.state = REVIVING;
                }
                break;

            case REVIVING:
                // Réanimer les oiseaux morts dans la colonne cible
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
        }
    }
}


void drawCloud(SDL_Renderer *renderer) {
    if (revivalCloud.active) {
        SDL_Rect cloudRect = {revivalCloud.x, revivalCloud.y, revivalCloud.width, revivalCloud.height};
        SDL_RenderCopy(renderer, cloudTexture, NULL, &cloudRect);
    }
}



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
        nombre_de_decente++;
        direction *= -1;
        *offsetY += alienHeight;
        for (int i = 0; i < ALIEN_ROWS; i++) {
            for (int j = 0; j < ALIEN_COLUMNS; j++) {
                aliens[i][j].y += (3*alienHeight) / 4;
            }
        }
        if(nombre == 1 || nombre==2){
            nombre=3;
        }else{
            nombre=1;
        }
        moveDelayDynamic=moveDelayDynamic-10;
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

void aliensFire() {
    for (int col = 0; col < ALIEN_COLUMNS; col++) {
        for (int row = ALIEN_ROWS - 1; row >= 0; row--) {
            if (aliens[row][col].alive) {
                if (!alienLasers[col].active && (rand() / (double)RAND_MAX) < FIRE_CHANCE) {
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


GameMode chooseGameMode(SDL_Renderer *renderer) {
    bool modeChoisi = false;
    GameMode mode = MODE_KILL_THEM_ALL;

    // Dimensions des "boutons"
    int buttonWidth = 300;
    int buttonHeight = 80;

    // Position des boutons : centrés, avec un écart vertical
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

    // Simple instructions dans la console
    printf("=== MENU SIMPLE SDL2 ===\n");
    printf("Cliquez sur le rectangle du haut pour KILL THEM ALL\n");
    printf("Cliquez sur le rectangle du bas pour FFA\n");
    while (!modeChoisi) {
        // On récupère les événements SDL
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                // Si on ferme la fenêtre, on quitte
                SDL_Quit();
                exit(0);
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                // Gestion du clic de souris
                int mx = event.button.x;
                int my = event.button.y;
                // Vérifie si on clique sur Kill Them All
                if (mx >= killRect.x && mx <= killRect.x + killRect.w &&
                    my >= killRect.y && my <= killRect.y + killRect.h) {
                    // L'utilisateur a cliqué dans la zone du bouton KILL THEM ALL
                    mode = MODE_KILL_THEM_ALL;
                    modeChoisi = true;
                // Vérifie si on clique sur FFA
                } else if (mx >= ffaRect.x && mx <= ffaRect.x + ffaRect.w &&
                           my >= ffaRect.y && my <= ffaRect.y + ffaRect.h) {
                    // L'utilisateur a cliqué dans la zone du bouton FFA
                    mode = MODE_FFA;
                    modeChoisi = true;
                }
            }
        }

        // Efface l'écran (noir)
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Dessine le bouton KILL (gris clair)
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderFillRect(renderer, &killRect);

        // Dessine le bouton FFA (gris clair)
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderFillRect(renderer, &ffaRect);

        // Affiche le rendu (mise à jour de la fenêtre)
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    return mode;
}


int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Erreur d'initialisation de SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_DisplayMode displayMode;
    if (SDL_GetCurrentDisplayMode(0, &displayMode) != 0) {
        fprintf(stderr, "Erreur d'obtention des dimensions de l'écran: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    screenWidth = displayMode.w;
    screenHeight = displayMode.h;

    SDL_Window *window = SDL_CreateWindow("Space Invaders",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          screenWidth, screenHeight,
                                          SDL_WINDOW_FULLSCREEN);
    if (!window) {
        fprintf(stderr, "Erreur de création de la fenêtre: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "Erreur de création du renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    GameMode gameMode = chooseGameMode(renderer);
    if (gameMode == MODE_KILL_THEM_ALL) {
        printf("Mode choisi : KILL THEM ALL\n");
    } else {
        printf("Mode choisi : FFA\n");
    }

    srand(time(NULL));
    initAliens();
    initPlayerShip();

    bool running = true;
    SDL_Event event;
    Uint32 lastMoveTime = SDL_GetTicks();
    int offsetY = 0;

    cloudTexture = loadTexture(renderer, "nuage.BMP");

    alienTexture1 = loadTexture(renderer, "mouetteDB.BMP");
    alienTexture2 = loadTexture(renderer, "mouetteDH.BMP");
    alienTexture3 = loadTexture(renderer, "mouetteGB.BMP");
    alienTexture4 = loadTexture(renderer, "mouetteGH.BMP");

    persoTexture = loadTexture(renderer, "tire.BMP");

    fondTexture = loadTexture(renderer, "fond_pleine.BMP");

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_q)) {
                running = false;
            }
            handlePlayerInput(event);
        }

        Uint32 currentTime = SDL_GetTicks();
        if (currentTime > lastMoveTime + moveDelayDynamic) {
            updateAliens(&offsetY);
            lastMoveTime = currentTime;
        }

        handlePlayerMovement();
        updateLasers();
        aliensFire();
        spawnCloud();
        updateCloud();

        // SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        // SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, fondTexture, NULL, NULL);

        drawAliens(renderer);
        drawPlayerShip(renderer);
        drawLasers(renderer);
        drawCloud(renderer);

        SDL_RenderPresent(renderer);

        if (playerShip.lives <= 0) {
            printf("Game Over!\n");
            running = false;
        } else if (offsetY >= screenHeight - alienHeight) {
            printf("Les mouettes ont atteint le bas de l'écran !\n");
            running = false;
        } else if (toujours_vivant_toujour_la_patate()==0){
            printf("Les mouettes... Elles sont morts, BRAVO !\n");
            running = false;
        }

        SDL_Delay(11);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(alienTexture);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
