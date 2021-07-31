//
//  status.c
//  SDLTut
//
//  Created by Andrew Swan on 18/11/2020.
//

#include <stdio.h>
#include "status.h"
#include "main.h"

void init_status_lives(GameState *gameState) {
    
    SDL_Color white={ 255, 255, 255, 255 };
    
    SDL_Surface *tmp = TTF_RenderText_Blended(gameState->font, "Once upon a time...", white);
    gameState->labelW = tmp->w;
    gameState->labelH = tmp->h;
    gameState ->label = SDL_CreateTextureFromSurface(gameState->renderer, tmp);
    
    SDL_FreeSurface(tmp);

}

void draw_status_lives(GameState *gameState) {
    SDL_SetRenderDrawColor(gameState->renderer, 0, 0, 0, 255);

    SDL_RenderClear(gameState->renderer);
    
    SDL_Rect rect = { 280, 300-24, 48, 48 };
    SDL_RenderCopyEx(gameState->renderer, gameState->playerFrames[gameState->player.frame], NULL, &rect, 0, NULL, (gameState->player.facingRight==0));
    
    SDL_SetRenderDrawColor(gameState->renderer, 255, 255, 255, 255);
    
    SDL_Rect textRect = { 120, 240-gameState->labelH, gameState->labelW, gameState->labelH };
    SDL_RenderCopy(gameState->renderer, gameState->label, NULL, &textRect);
}

void shutdown_status_lives(GameState *gameState) {
    SDL_DestroyTexture(gameState->label);
    gameState->label = NULL;
}



