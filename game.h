#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>

#define ALIEN_ROWS 3
#define ALIEN_COLUMNS 10
#define MOVE_DELAY 100
#define LASER_SPEED 10
#define LASER_WIDTH 5
#define LASER_HEIGHT 20
#define FIRE_CHANCE 0.005  //0.01
#define PLAYER_LASER_SPEED -15
#define PLAYER_SPEED 10
#define MAX_PLAYER_LIVES 5
#define NUM_BARRIERS 4
#define MAX_TURRETS 4
#define MAX_TURRET_LASERS 20
#define MAX_HIGH_SCORES 5

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
    int ecrous;
} PlayerShip;

typedef struct {
    int x, y;
    int width, height;
    bool active;
    int targetColumn;
    enum {MOVING_TO_TARGET, REVIVING, LEAVING} state;
} Cloud;

typedef enum {
    MODE_KILL_THEM_ALL,
    MODE_FFA
} GameMode;

typedef struct {
    int x, y;
    int width, height;
    int visualHeight;
    int health;
} Barrier;

typedef struct {
    int x, y;
    int width, height;
    bool active;
    Uint32 lastShotTime;
    int health;
    bool existe;
} Turret;

typedef struct {
    int x, y;
    bool active;
    int speed;
} TurretLaser;

typedef struct {
    char name[51];
    int score;
} HighScore;

extern Alien aliens[ALIEN_ROWS][ALIEN_COLUMNS];
extern Laser alienLasers[ALIEN_COLUMNS];
extern Laser playerLaser;
extern PlayerShip playerShip;
extern int direction;
extern int screenWidth, screenHeight;
extern int alienWidth, alienHeight;
extern int moveDelayDynamic;
extern bool movingLeft, movingRight;
extern SDL_Texture *alienTexture;
extern SDL_Texture *alienTexture1;
extern SDL_Texture *alienTexture2;
extern SDL_Texture *alienTexture3;
extern SDL_Texture *alienTexture4;
extern SDL_Texture *persoTexture;
extern SDL_Texture *fondTexture;
extern SDL_Texture *cloudTexture;
extern SDL_Texture *pauseTexture;
extern SDL_Texture *barrierTexture;
extern SDL_Texture *turretTexture;
extern SDL_Texture *ecrousTexture;
extern int nombre_de_decente;
extern Cloud revivalCloud;
extern int currentRound;
extern bool isPaused;
extern Barrier barriers[NUM_BARRIERS];
extern Turret turrets[MAX_TURRETS];
extern TurretLaser turretLasers[MAX_TURRET_LASERS];
extern int score;


void initAliens();
void initPlayerShip();
void initCloud();
void updateAliens(int *offsetY);
void updateLasers();
void aliensFire();
void spawnCloud();
void updateCloud();
int findDenseDeadZone();
int toujours_vivant_toujour_la_patate();
GameMode chooseGameMode(SDL_Renderer *renderer);
void resetAliensForNewRound();
void initBarriers();
void reviveBarrier(int mouseX, int mouseY);
void initTurrets();
void createTurret(int barrierIndex, int x, int y, int width, int height);
void updateTurrets();
void initTurretLasers();
void createTurretLaser(int x, int y);
void updateTurretLasers();
void saveHighScore(const char* name, int score);
void loadHighScores(HighScore scores[]);
void resetGame();

#endif // GAME_H