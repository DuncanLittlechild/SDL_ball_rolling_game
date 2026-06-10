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

////////////////////////////////////
//        FILEIO UTILS         //
////////////////////////////////////

void SaveGameMap(const GameMap& map);

void LoadGameMap(const GameMap& map);

bool PlayerInWall(GameState* game);

// Returns true if any collisions occured, and adds the direction in which the collision occured to collisionDirs.
// If collisionDirs is NULL, it will return the moment a single collision is detected. Otherwise, it will look for all
// possible collisions
bool DetectPlayerCollisions(const SDL_FPoint c, const float r, std::vector<SDL_FPoint>* collisionNorms, const GameMap& map);

// Uses triangle fan to draw a circle. If texture is set, colour modifies the
void DrawCircle(SDL_Renderer* renderer, SDL_FPoint centre, float radius, SDL_FColor color = {1.0f,1.0f,1.0f,1.0f});

#endif //SDL_BALL_ROLLING_GAME_PROJECT_UTILS_H
