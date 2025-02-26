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