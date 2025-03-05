#include "game.h"
#include "render.h"
#include "input.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// gcc -Wall main.c game.c render.c input.c -o main -lSDL2 -lSDL2_ttf -lSDL2_mixer
// ./main


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

    int tempo = 0;

    while(true){

        if(tempo==0){
            fondTexture = loadTexture(renderer, "fond_pleine.bmp");
        }
        SDL_RenderCopy(renderer, fondTexture, NULL, NULL);

        GameMode gameMode = chooseGameMode(renderer);
        if (gameMode == MODE_KILL_THEM_ALL) {
            printf("Mode choisi : KILL THEM ALL\n");
        } else {
            printf("Mode choisi : FFA\n");
        }

        GameMode mode = chooseGameMode(renderer);
        if (mode == MODE_KILL_THEM_ALL) {
            // Initialisation de SDL_mixer pour la musique du jeu
            if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
                 fprintf(stderr, "Erreur d'initialisation de SDL_mixer: %s\n", Mix_GetError());
            }
            // Charger la musique du jeu
            Mix_Music *gameMusic = Mix_LoadMUS("MusiqueJeu.mp3");
            if (!gameMusic) {
                 fprintf(stderr, "Erreur lors du chargement de la musique du jeu: %s\n", Mix_GetError());
            } else {
                 // Démarrer la musique en boucle infinie
                 if (Mix_PlayMusic(gameMusic, -1) == -1) {
                      fprintf(stderr, "Erreur lors de la lecture de la musique du jeu: %s\n", Mix_GetError());
                 }
            }
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

        if (!tempo){
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
            tempo=1;
        }

        char playerName[51] = "";
        bool isEnterPressed = false;
        bool showError = false;

        while (!isEnterPressed) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    // Quitter si l'utilisateur ferme la fenêtre
                    SDL_DestroyRenderer(renderer);
                    SDL_DestroyWindow(window);
                    TTF_CloseFont(font);
                    TTF_Quit();
                    SDL_Quit();
                    return 0;
                }

                // Gérer la saisie du texte
                handleTextInput(event, playerName, &isEnterPressed);

                // Afficher un message d'erreur si le joueur essaie de valider sans saisir de caractères
                if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN && strlen(playerName) == 0) {
                    showError = true;
                }
            }

            // Afficher l'écran de saisie de texte
            drawTextInputScreen(renderer, font, playerName, showError);
        }
        // Démarrer la partie avec le nom du joueur
        printf("Nom du joueur : %s\n", playerName);

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

        saveHighScore(playerName, score);

        drawGameOverScreen(renderer, font);
        SDL_RenderClear(renderer);
        resetGame();

        SDL_Delay(5000);
        TTF_CloseFont(font);
    }


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