#include "game.h"
#include "render.h"
#include "input.h"
#include <SDL2/SDL.h>
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

    pauseTexture = loadTexture(renderer, "pause.BMP");
    // if (!pauseTexture) {
    //     fprintf(stderr, "Erreur de chargement de l'image de pause.\n");
    //     SDL_DestroyRenderer(renderer);
    //     SDL_DestroyWindow(window);
    //     SDL_Quit();
    //     return 1;
    // }

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
        }

        SDL_RenderCopy(renderer, fondTexture, NULL, NULL);
        drawAliens(renderer);
        drawPlayerShip(renderer);
        drawLasers(renderer);
        drawCloud(renderer);

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

    SDL_DestroyTexture(pauseTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(alienTexture);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}