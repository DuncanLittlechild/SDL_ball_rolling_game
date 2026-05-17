//
// Created by duncan on 5/17/26.
//

#ifndef SDL_BALL_ROLLING_GAME_PROJECT_UTILS_H
#define SDL_BALL_ROLLING_GAME_PROJECT_UTILS_H
#include <utility>
#include <SDL3/SDL.h>

#include "project_structs.h"
////////////////////////////////////
//        SDLFPOINT UTILS         //
////////////////////////////////////
// Utilities which manipulate SDLFPoint

double SDL_FPointMagnitude(const SDL_FPoint& point);

double SDL_FPointMagnitudeSquared(const SDL_FPoint& point);

SDL_FPoint SDL_FPointNormalise(SDL_FPoint& point);

SDL_FPoint SDL_FPointClamp(SDL_FPoint& point);


bool PlayerInWall(GameState* game);

#endif //SDL_BALL_ROLLING_GAME_PROJECT_UTILS_H
