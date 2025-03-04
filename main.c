#include "game.h"
#include "render.h"
#include "input.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

    if (TTF_Init() == -1) {
        fprintf(stderr, "Erreur d'initialisation de SDL_ttf: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    // Charger une police
    TTF_Font *font = TTF_OpenFont("arial.ttf", 24);
    if (!font) {
        fprintf(stderr, "Erreur de chargement de la police: %s\n", TTF_GetError());
        TTF_Quit();
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
    initBarriers();
    initTurrets();
    initTurretLasers();

    bool running = true;
    SDL_Event event;
    Uint32 lastMoveTime = SDL_GetTicks();
    int offsetY = 0;

    cloudTexture = loadTexture(renderer, "nuage.bmp");
    alienTexture1 = loadTexture(renderer, "mouetteDB.bmp");
    alienTexture2 = loadTexture(renderer, "mouetteDH.bmp");
    alienTexture3 = loadTexture(renderer, "mouetteGB.bmp");
    alienTexture4 = loadTexture(renderer, "mouetteGH.bmp");
    persoTexture = loadTexture(renderer, "tire.bmp");
    fondTexture = loadTexture(renderer, "fond_pleine.bmp");
    pauseTexture = loadTexture(renderer, "pause.bmp");
    barrierTexture = loadTexture(renderer, "barrier.bmp");
    turretTexture = loadTexture(renderer, "ruche.bmp");
    ecrousTexture = loadTexture(renderer, "ecrous.bmp");

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_q)) {
                running = false;
            }
            handlePlayerInput(event);
        }

        if (!isPaused) {
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
            updateTurrets();
            updateTurretLasers();
        }

        SDL_RenderCopy(renderer, fondTexture, NULL, NULL);
        drawAliens(renderer);
        drawPlayerShip(renderer);
        drawLasers(renderer);
        drawCloud(renderer);
        drawBarriers(renderer);
        drawTurrets(renderer);
        drawTurretLasers(renderer);
        drawScore(renderer, font);  // Afficher le score avec SDL_ttf

        if (isPaused) {
            drawPauseMenu(renderer);
        }

        SDL_RenderPresent(renderer);

        // Vérifier si tous les aliens sont morts
        if (toujours_vivant_toujour_la_patate() == 0) {
            printf("Manche %d terminée ! Préparation de la manche %d...\n", currentRound, currentRound + 1);
            resetAliensForNewRound(); // Réinitialiser pour la nouvelle manche
        }

        // Vérifier les conditions de défaite
        if (playerShip.lives <= 0) {
            printf("Game Over! Vous avez atteint la manche %d.\n", currentRound);
            running = false;
        } else if (offsetY >= screenHeight - alienHeight) {
            printf("Les aliens ont atteint le bas de l'écran ! Vous avez atteint la manche %d.\n", currentRound);
            running = false;
        }

        SDL_Delay(11);
    }
    
    SDL_Color white = {255, 255, 255, 255};
    char scoreText[100];
    snprintf(scoreText, sizeof(scoreText), "Score final : %d", score);
    SDL_Surface *scoreSurface = TTF_RenderText_Solid(font, scoreText, white);
    SDL_Texture *scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
    SDL_Rect scoreRect = {screenWidth / 2 - scoreSurface->w / 2, screenHeight / 2 - scoreSurface->h / 2, scoreSurface->w, scoreSurface->h};

    SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);
    SDL_RenderPresent(renderer);
    SDL_Delay(5000);

    SDL_FreeSurface(scoreSurface);
    SDL_DestroyTexture(scoreTexture);
    TTF_CloseFont(font);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(alienTexture1);
    SDL_DestroyTexture(alienTexture2);
    SDL_DestroyTexture(alienTexture3);
    SDL_DestroyTexture(alienTexture4);
    SDL_DestroyTexture(persoTexture);
    SDL_DestroyTexture(cloudTexture);
    SDL_DestroyTexture(fondTexture);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
  
    
    return 0;
}