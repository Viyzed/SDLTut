//
//  main.h
//  SDLTut
//
//  Created by Andrew Swan on 18/11/2020.
//

#ifndef main_h
#define main_h

#define STATUS_STATE_LIVES 0
#define STATUS_STATE_GAME 1
#define STATUS_STATE_GAMEOVER 2
#define STATUS_STATE_WIN 3

#define NUM_MEN 80

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

typedef struct {
    float x, y;
    float dx, dy;
    short lives;
    char *name;
    int onLedge, isDead;
    int frame, facingRight, slowing;
} Player;

typedef struct {
    int x, y;
} Man;

typedef struct {
    int x, y, w, h;
} Platform;

typedef struct{
    //Scrolling offset
    float scrollX;
    
    //Players
    Player player;

    //Man
    Man men[NUM_MEN];
    
    //Platforms
    Platform platforms[100];

    //Images
    SDL_Texture *playerFrames[2];
    SDL_Texture *playerDeath[2];
    SDL_Texture *man;
    SDL_Texture *platform;
    SDL_Texture *label;
    int labelW, labelH;
    
    //Fonts
    TTF_Font *font;
    
    //Frames passed
    int time;
    int statusState;
    
    //Sounds
    int musicChannel;
    Mix_Chunk *bgMusic, *jumpSound, *moveSound, *dieSound;
    
    //Renderer
    SDL_Renderer *renderer;
} GameState;

//Prototypes
void process(GameState *game);

#endif /* main_h */
