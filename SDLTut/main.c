#include <stdio.h>
#include <time.h>
#include "main.h"
#include "status.h"

#define GRAVITY 0.32f;

void loadGame(GameState *game) {
    SDL_Surface *surface = NULL;

    //Load images and create rendering textures from them
    surface = IMG_Load("/Users/Andrew/workspace/C/XCode/SDLTut/Images/ghost_girl.png");
    if(surface == NULL) {
        printf("Cannot find ghost_girl.png.\n\n");
        SDL_Quit();
        exit(1);
    }
  
    game->playerFrames[0] = SDL_CreateTextureFromSurface(game->renderer, surface);
    SDL_FreeSurface(surface);
    
    surface = IMG_Load("/Users/Andrew/workspace/C/XCode/SDLTut/Images/ghost_girl2.png");
    game->playerFrames[1] = SDL_CreateTextureFromSurface(game->renderer, surface);
    SDL_FreeSurface(surface);
    
    surface = IMG_Load("/Users/Andrew/workspace/C/XCode/SDLTut/Images/grass.png");
    game->platform = SDL_CreateTextureFromSurface(game->renderer, surface);
    SDL_FreeSurface(surface);
    
    surface = IMG_Load("/Users/Andrew/workspace/C/XCode/SDLTut/Images/ghost_girl_death1.png");
    game->playerDeath[0] = SDL_CreateTextureFromSurface(game->renderer, surface);
    SDL_FreeSurface(surface);
    
    surface = IMG_Load("/Users/Andrew/workspace/C/XCode/SDLTut/Images/ghost_girl_death2.png");
    game->playerDeath[1] = SDL_CreateTextureFromSurface(game->renderer, surface);
    SDL_FreeSurface(surface);
    
    surface = IMG_Load("/Users/Andrew/workspace/C/XCode/SDLTut/Images/man.png");
    game->man = SDL_CreateTextureFromSurface(game->renderer, surface);
    SDL_FreeSurface(surface);
    
    //Load True Type Font (Disposable Droid BB)
    game->font = TTF_OpenFont("/Users/Andrew/workspace/C/XCode/SDLTut/Fonts/DisposableDroidBB.ttf", 48);
    if(!game->font) {
        printf("Cannot find Font File.\n");
         SDL_Quit();
        exit(1);
    }
    
    //Load sounds
    game->bgMusic = Mix_LoadWAV("/Users/Andrew/workspace/C/XCode/SDLTut/Sound/ukulele.wav");
    if(game->bgMusic != NULL) {
        Mix_VolumeChunk(game->bgMusic, 8);
    }
    game->jumpSound = Mix_LoadWAV("/Users/Andrew/workspace/C/XCode/SDLTut/Sound/giggle.wav");
    game->moveSound = Mix_LoadWAV("/Users/Andrew/workspace/C/XCode/SDLTut/Sound/swish_swish.wav");
    game->dieSound = Mix_LoadWAV("/Users/Andrew/workspace/C/XCode/SDLTut/Sound/swoosh.wav");
    
    game->label = NULL;
    
    //Init GameState
    game->statusState = STATUS_STATE_LIVES;
    
    init_status_lives(game);
    
    //Init scroll offset
    game->scrollX = 0;

    //Init player
    game->player.lives = 3;
    game->player.x = 280;
    game->player.y = 300-24;
    game->player.dy = 0;
    game->player.dx = 0;
    game->player.onLedge = 0;
    game->player.isDead = 0;
    game->player.frame = 0;
    game->player.facingRight = 1;
    game->player.slowing = 0;

    //Init time
    game->time = 0;
    
    //init men
    for(int i = 0; i < NUM_MEN; i++) {
        game->men[i].x = (i+10)*128;
        game->men[i].y = random()%640;
    }
    
    //init platform
    for(int i = 0; i < 100; i++) {
        game->platforms[i].w = 128;
        game->platforms[i].h = 64;
        game->platforms[i].x = i*128;
        game->platforms[i].y = 420;
    }
    game->platforms[99].x = 350;
    game->platforms[99].y = 200;
    
}

int processEvents(SDL_Window *window, GameState *game) {
    SDL_Event event;
    int done = 0;

    while(SDL_PollEvent(&event)) {
    switch(event.type) {
      case SDL_WINDOWEVENT_CLOSE: {
        if(window) {
          SDL_DestroyWindow(window);
          window = NULL;
          done = 1;
        }
      }
      break;
      case SDL_KEYDOWN: {
        switch(event.key.keysym.sym) {
          case SDLK_ESCAPE:
            done = 1;
          break;
        case SDLK_UP:
        if(game->player.onLedge == 1) {
            game->player.dy = -7;
            game->player.onLedge = 0;
            game->player.slowing = 0;
            Mix_PlayChannel(-1, game->jumpSound, 0);
        }
        break;
        case SDLK_w:
        if(game->player.onLedge == 1) {
            game->player.dy = -7;
            game->player.onLedge = 0;
            game->player.slowing = 0;
            Mix_PlayChannel(-1, game->jumpSound, 0);
        }
        break;
        case SDLK_SPACE:
        if(game->player.onLedge == 1) {
            game->player.dy = -7;
            game->player.onLedge = 0;
            game->player.slowing = 0;
            Mix_PlayChannel(-1, game->jumpSound, 0);
        }
        break;
        }
      }
      break;
      case SDL_QUIT:
        //quit out of the game
        done = 1;
      break;
      }
    }
    
    //Jumping power
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    if(state[SDL_SCANCODE_UP] || state[SDL_SCANCODE_W]  || state[SDL_SCANCODE_SPACE]) {
        game->player.dy -= 0.2f;
    }

    //Player movement
    if(state[SDL_SCANCODE_LEFT] || state[SDL_SCANCODE_A]) {
        //Mix_PlayChannel(0, game->moveSound, 1);
        game->player.facingRight = 0;
        game->player.dx -= 0.5;
        if(game->player.dx < -6) {
            game->player.dx = -6;
        }
        game->player.slowing = 0;
    }
    else if(state[SDL_SCANCODE_RIGHT] || state[SDL_SCANCODE_D]) {
        //Mix_PlayChannel(0, game->moveSound, 1);
        game->player.facingRight = 1;
        game->player.dx += 0.5;
        if(game->player.dx > 6) {
            game->player.dx = 6;
        }
        game->player.slowing = 0;
    }
    else {
        game->player.frame = 0;
        game->player.dx *= 0.8;
        game->player.slowing = 1;
        if(fabsf(game->player.dx) < 0.1f) {
            game->player.dx = 0;
        }
    }

    return done;
}

void doRender(SDL_Renderer *renderer, GameState *game) {
    if(game->statusState == STATUS_STATE_LIVES) {
        draw_status_lives(game);
    } else if(game->statusState == STATUS_STATE_GAME){
        //set the drawing color to blue
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);

        //Clear the screen (to blue)
        SDL_RenderClear(renderer);

        //set the drawing color to white
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        
        for(int i = 0; i < 100; i++) {
            SDL_Rect platformRect = { game->scrollX+game->platforms[i].x, game->platforms[i].y, game->platforms[i].w, game->platforms[i].h};
            SDL_RenderCopy(renderer, game->platform, NULL, &platformRect);
        }

        //draw a player
        SDL_Rect rect = { game->scrollX+game->player.x, game->player.y, 48, 48 };
        SDL_RenderCopyEx(renderer, game->playerFrames[game->player.frame], NULL, &rect, 0, NULL, (game->player.facingRight==0));
      
        //death animation
        if(game->player.isDead) {
            SDL_DestroyTexture(game->playerFrames[0]);
            //SDL_DestroyTexture(game->playerFrames[1]);
            SDL_Rect rect = { game->scrollX+game->player.x, game->player.y, 48, 48 };
            SDL_RenderCopyEx(renderer, game->playerDeath[0], NULL, &rect, 0 , NULL, (game->player.facingRight==0));
            
        }
        
        //draw the man image
        for(int i = 0; i < NUM_MEN; i++) {
            SDL_Rect manRect = { game->scrollX+game->men[i].x, game->men[i].y, 64, 64 };
            SDL_RenderCopy(renderer, game->man, NULL, &manRect);
        }
     
    }
  
      //We are done drawing, "present" or show to the screen what we've drawn
      SDL_RenderPresent(renderer);
}

void process(GameState *game) {
    
    //add time
    game->time++;
    
    if(game->statusState == STATUS_STATE_LIVES) {
        if(game->time > 120) {
            game->statusState = STATUS_STATE_GAME;
            shutdown_status_lives(game);
            game->musicChannel = Mix_PlayChannel(-1, game->bgMusic, -1);
        }
    } else if(game->statusState == STATUS_STATE_GAMEOVER) {
        Mix_HaltChannel(game->musicChannel);
        if(game->time > 190) {
            SDL_Quit();
            exit(0);
        }
    }
    
    if(game->statusState == STATUS_STATE_GAME) {
        if(!game->player.isDead) {
            //player movement
            Player *player = &game->player;
            player->y += player->dy;
            player->x += player->dx;
            player->dy += GRAVITY;
            
            game->scrollX = -player->x+280;
            if(game->scrollX > 0) {
                game->scrollX = 0;
            }
            
            //animate player
            if(player->dx != 0 && !player->slowing) {
                if(game->time % 30 == 0) {
                    if(player->frame == 0 )
                        player->frame = 1;
                    else
                        player->frame = 0;
                }
            }
        } else {
            //init_game_over(game);
            //game->statusState = STATUS_STATE_GAMEOVER;
        }
    }
    
}

//useful utility function to see if two rectangles are colliding
int collide2d(float x1, float y1, float x2, float y2, float wt1, float ht1, float wt2, float ht2) {
    return (!((x1 > (x2+wt2)) || (x2 > (x1+wt1)) || (y1 > (y2+ht2)) || (y2 > (y1+ht1))));
}

void collisionDetect(GameState *game) {
    
    //Check for collision with enemies
    for(int i = 0; i < NUM_MEN; i++) {
        if(collide2d(game->player.x, game->player.y, game->men[i].x, game->men[i].y, 48, 48, 64, 64)) {
            if(!game->player.isDead) {
                game->player.isDead = 1;
                Mix_HaltChannel(game->musicChannel);
                Mix_PlayChannel(-1, game->dieSound, 0);
            }
            break;
        }
    }
    
    //Check for collision with any ledges (brick blocks)
    for(int i = 0; i < 100; i++) {
      float pw = 48, ph = 48;
      float px = game->player.x, py = game->player.y;
      float bx = game->platforms[i].x, by = game->platforms[i].y, bw = game->platforms[i].w, bh = game->platforms[i].h;

      if(px+pw/2 > bx && px+pw/2<bx+bw) {
        //are we bumping our head?
        if(py < by+bh && py > by && game->player.dy < 0) {
          //correct y
          game->player.y = by+bh;
          py = by+bh;
          
          //bumped our head, stop any jump velocity
          game->player.dy = 0;
          game->player.onLedge = 1;
        }
      }
      if(px+pw > bx && px<bx+bw) {
        //are we landing on the ledge
        if(py+ph > by && py < by && game->player.dy > 0) {
          //correct y
          game->player.y = by-ph;
          py = by-ph;
          
          //landed on this ledge, stop any jump velocity
          game->player.dy = 0;
          game->player.onLedge = 1;
        }
      }
    
      if(py+ph > by && py<by+bh) {
        //rubbing against right edge
        if(px < bx+bw && px+pw > bx+bw && game->player.dx < 0) {
          //correct x
          game->player.x = bx+bw;
          px = bx+bw;
          
          game->player.dx = 0;
        }
        //rubbing against left edge
        else if(px+pw > bx && px < bx && game->player.dx > 0) {
          //correct x
          game->player.x = bx-pw;
          px = bx-pw;
          
          game->player.dx = 0;
        }
      }
        
        //put border on the left edge
        if(px < 0) {
            game->player.x = 0;
        }
        
    }
    
}

int main(int argc, char *argv[]) {
    GameState gameState;
    SDL_Window *window = NULL;                    // Declare a window
    SDL_Renderer *renderer = NULL;                // Declare a renderer

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO);              // Initialize SDL2
    
    srandom((int)time(NULL));
  
    //Create an application window with the following settings:
    window = SDL_CreateWindow("Game Window",                     // window title
                            SDL_WINDOWPOS_UNDEFINED,           // initial x position
                            SDL_WINDOWPOS_UNDEFINED,           // initial y position
                            640,                               // width, in pixels
                            480,                               // height, in pixels
                            0                                  // flags
                            );
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    gameState.renderer = renderer;
    
    //Initialise font system
    TTF_Init();
    
    //Initialise sound
    Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096);
  
    loadGame(&gameState);
  
    // The window is open: enter program loop (see SDL_PollEvent)
    int done = 0;
  
    //Event loop
    while(!done) {
        //Check for events
        done = processEvents(window, &gameState);
        
        //Process jump and gravity
        process(&gameState);
        
        //Detect collision
        collisionDetect(&gameState);
        
        //Render display
        doRender(renderer, &gameState);
    }
  

    //Shutdown game and unload all memory
    SDL_DestroyTexture(gameState.man);
    SDL_DestroyTexture(gameState.platform);
    SDL_DestroyTexture(gameState.playerFrames[0]);
    SDL_DestroyTexture(gameState.playerFrames[1]);
    if(gameState.label != NULL) {
        SDL_DestroyTexture(gameState.label);
    }
    TTF_CloseFont(gameState.font);
    
    //Free sound memory
    Mix_FreeChunk(gameState.bgMusic);
    Mix_FreeChunk(gameState.jumpSound);
    Mix_FreeChunk(gameState.moveSound);
    Mix_FreeChunk(gameState.dieSound);

    // Close and destroy the window
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);

    //Quit True Time Font
    TTF_Quit();
    
    // Clean up
    SDL_Quit();
    return 0;
}
