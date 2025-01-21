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
#define MAX_PLAYER_LIVES 3

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
    playerShip.height = screenHeight / 20;
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
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_Rect playerRect = {playerShip.x, playerShip.y, playerShip.width, playerShip.height};
    SDL_RenderFillRect(renderer, &playerRect);

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    for (int i = 0; i < playerShip.lives; i++) {
        SDL_Rect lifeRect = {10 + i * 30, 10, 20, 20};
        SDL_RenderFillRect(renderer, &lifeRect);
    }
}

void drawLasers(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
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
        direction *= -1;
        *offsetY += alienHeight;
        for (int i = 0; i < ALIEN_ROWS; i++) {
            for (int j = 0; j < ALIEN_COLUMNS; j++) {
                aliens[i][j].y += (3*alienHeight) / 4;
            }
        }
        moveDelayDynamic=moveDelayDynamic-10;
    } else {
        for (int i = 0; i < ALIEN_ROWS; i++) {
            for (int j = 0; j < ALIEN_COLUMNS; j++) {
                aliens[i][j].x += direction * 10;
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

int toujours_vivant_toujour_la_patate(){
    int etat = 0;
    for (int i = 0; i < ALIEN_ROWS; i++) {
        for (int j = 0; j < ALIEN_COLUMNS; j++) {
            if(aliens[i][j].alive == true){
                etat=1;
            }
        }
    }
    return etat;
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

    srand(time(NULL));
    initAliens();
    initPlayerShip();

    bool running = true;
    SDL_Event event;
    Uint32 lastMoveTime = SDL_GetTicks();
    int offsetY = 0;

    alienTexture = loadTexture(renderer, "3.BMP");
    if (!alienTexture) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }


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

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        drawAliens(renderer);
        drawPlayerShip(renderer);
        drawLasers(renderer);

        SDL_RenderPresent(renderer);

        if (playerShip.lives <= 0) {
            printf("Game Over!\n");
            running = false;
        } else if (offsetY >= screenHeight - alienHeight) {
            printf("Les aliens ont atteint le bas de l'écran !\n");
            running = false;
        } else if (toujours_vivant_toujour_la_patate()==0){
            printf("Les aliens... Ils sont morts, BRAVO !\n");
            running = false;
        }

        SDL_Delay(10);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(alienTexture);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
