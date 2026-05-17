//
// Created by duncan on 5/17/26.
//

#include "project_utils.h"

#include <cassert>

#include "project_structs.h"

////////////////////////////////////
//        SDLFPOINT UTILS         //
////////////////////////////////////
// Utilities which manipulate SDLFPoint

double SDL_FPointMagnitude(const SDL_FPoint& point) {
    return SDL_sqrt(point.x * point.x + point.y * point.y);
}

double SDL_FPointMagnitudeSquared(const SDL_FPoint& point) {
    return point.x * point.x + point.y * point.y;
}

SDL_FPoint SDL_FPointNormalise(SDL_FPoint& point) {
    double pointMagnitude {SDL_FPointMagnitude(point)};
    assert(pointMagnitude > 0 && "SDL_FPoint with magnitude of 0 passed to SDL_FPointNormalise");
    if (pointMagnitude > GLOBALGAMESETTINGS.velocityMax) {
        point.x /= pointMagnitude;
        point.y /= pointMagnitude;
    }
    return point;
}

SDL_FPoint SDL_FPointClamp(SDL_FPoint& point) {
    double pointMagnitude {SDL_FPointMagnitude(point)};
    if (pointMagnitude > GLOBALGAMESETTINGS.velocityMax) {
        point.x = (point.x * GLOBALGAMESETTINGS.velocityMax) / pointMagnitude;
        point.y = (point.y * GLOBALGAMESETTINGS.velocityMax) / pointMagnitude;
    }
    return point;
}

bool PlayerInWall(GameState* game) {
    bool isInWall {false};
    const auto& player {game->player};
    float playerSize {GLOBALGAMESETTINGS.playerSizeFactor};
    float halfPlayerSize{playerSize/2.0f};
    SDL_FPoint collisionOffsets[4] {
        {halfPlayerSize, 0.0f}, // North
        {playerSize, halfPlayerSize}, // East
        {halfPlayerSize, playerSize}, // South
        {0.0f, halfPlayerSize} // West
    };
    // check for collisions
    for (auto& offset : collisionOffsets) {
        SDL_FPoint cardinalPoint {player.pos.x + offset.x, player.pos.y + offset.y};
        if (game->map.m[static_cast<std::size_t>(cardinalPoint.y)][static_cast<std::size_t>(cardinalPoint.x)].tp == CTYPE_WALL) {
            isInWall = true;
            break;
        }
    }
    return isInWall;
}