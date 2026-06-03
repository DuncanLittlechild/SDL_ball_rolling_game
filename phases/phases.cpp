#include "phases.h"

#include <iostream>
#include <ostream>
#include <cmath>

#include "project_utils.h"

///////////////////////////////////
//        INITIALISATION        //
//////////////////////////////////

void InitMap(GameState* game){
    std::vector<std::vector<Cell>> tmp {};
    tmp.reserve(startingMap.size());
    for (auto row : startingMap){
        std::vector<Cell> r {};
        r.reserve(startingMap[0].size());
        for (auto c : row){
            r.push_back(Cell{static_cast<CellType>(c)});
        }
        tmp.push_back(r);
    }
    game->map.m = tmp;
}

void InitPlayer(GameState* game){
    game->player = {2.5f,2.5f};
}

void InitTextures(GameState* game){
    // Create player texture
    SDL_Texture* playerTexture{
        SDL_CreateTexture(game->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 1, 1)
    };
    SDL_SetRenderTarget(game->renderer, playerTexture);
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(game->renderer);

    // Create cell texture
    SDL_Texture* cellTexture {
        SDL_CreateTexture(game->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 1, 1)
    };
    SDL_SetRenderTarget(game->renderer, cellTexture);
    SDL_SetRenderDrawColor(game->renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(game->renderer);

    // Reset renderer target to the main window
    SDL_SetRenderTarget(game->renderer, NULL);

    // Add textures to the game state
    game->textures.push_back(playerTexture);
    game->textures.push_back(cellTexture);
}

///////////////////////////
//        UPDATE        //
//////////////////////////

// Update Camera is also used to initialise the camera
void UpdateCamera(Camera2d& camera, GameState* game){
    camera.pos = game->player.pos;
    camera.zoom = GLOBALGAMESETTINGS.scale;
    if (camera.screenH != GLOBALGAMESETTINGS.height){
        camera.screenH = GLOBALGAMESETTINGS.height;
    }
    if (camera.screenW != GLOBALGAMESETTINGS.width){
        camera.screenW = GLOBALGAMESETTINGS.width;
    }

    camera.UpdateCameraMatrix();
}



void UpdatePlayer(GameState* game, float deltaTime) {
    // get player
    auto& player {game->player};
    const auto& input {game->input};
    // Use inputs to update player velocity
    SDL_FPoint velocityUpdate {0.0f, 0.0f};
    float vAdjustment {GLOBALGAMESETTINGS.velocityAdjustment * deltaTime};
    if (input.lHeld) {
        velocityUpdate.x -= vAdjustment;
    }
    if (input.rHeld) {
        velocityUpdate.x += vAdjustment;
    }
    if (input.uHeld) {
        velocityUpdate.y -= vAdjustment;
    }
    if (input.dHeld) {
        velocityUpdate.y += vAdjustment;
    }

    // Normalise and clamp velocity
    if (velocityUpdate.x > 1e-6 || velocityUpdate.x < -1e-6 || velocityUpdate.y > 1e-6 || velocityUpdate.y < -1e-6) {
        SDL_FPointNormalise (velocityUpdate);
        player.vel.x += velocityUpdate.x;
        player.vel.y += velocityUpdate.y;
    }

    SDL_FPointClamp(player.vel);

    // Use velocity to update player position
    SDL_FPoint newPosition {player.pos.x + (player.vel.x / deltaTime), player.pos.y + (player.vel.y / deltaTime)};
    float halfPlayerSize {(GLOBALGAMESETTINGS.playerSizeFactor / 2.0f)};
    std::vector<SDL_FPoint> collisionNorms {};
    bool playerCollides {DetectPlayerCollisions(
        {newPosition.x + halfPlayerSize, newPosition.y + halfPlayerSize},
        halfPlayerSize,
        &collisionNorms,
        game->map
    )};

    // If the player collides in its new position, adjust velocity away from the collisionDirs
    if (playerCollides) {
        std::vector<SDL_FPoint> newVelocities (collisionNorms.size(), player.vel);
        // Adjust velocity by using the dot product of the collision norms
        // the formula is velocity - 2 * dot product of velocity and normal * normal
        for (auto i {0uz}; i < collisionNorms.size(); ++i) {
            float dotProd {newVelocities[i].x * collisionNorms[i].x + newVelocities[i].y * collisionNorms[i].y};
            newVelocities[i].x -= 2.0f * dotProd * collisionNorms[i].x;
            newVelocities[i].y -= 2.0f * dotProd * collisionNorms[i].y;
        }
        // Get the average of the new velocities, and set player velocity to that
        SDL_FPoint total {newVelocities[0]};
        for (auto i {1uz}; i < newVelocities.size(); ++i) {
            total.x += newVelocities[i].x;
            total.y += newVelocities[i].y;
        }
        total.x /= newVelocities.size();
        total.y /= newVelocities.size();
        player.vel.x = total.x;
        player.vel.y = total.y;
    }
    else {
        player.pos.x = newPosition.x;
        player.pos.y = newPosition.y;
    }
    // Collisions will need to
    // If there are collisions, update position and velocity to reflect this
}

void UpdateMap(GameState *game) {
    // Get the position of the mouse in render space
    float screenX, screenY;
    SDL_GetMouseState(&screenX, &screenY);

    float renderX, renderY;
    SDL_RenderCoordinatesFromWindow(game->renderer, screenX, screenY, &renderX, &renderY);

    // Invert the camera matrix to translate the render position into simulated space
    Mat3 camInv {InvertMat3(game->camera.cameraMat)};

    SDL_FPoint simPos {TransformPointMat3(camInv, renderX, renderY)};

    // If the simulated point is within the map matrix, then operations can proceed
    if (simPos.x >= 0 && simPos.x < game->map.m[0].size() && simPos.y >= 0 && simPos.y < game->map.m.size()) {
        // If the player has clicked on a square, change its type. Otherwise, highlight the square to be targetted
        if (game->input.playerClick) {
            auto& targetCell {game->map.m[(int)simPos.y][(int)simPos.x]};
            targetCell.tp = (CellType)((targetCell.tp + 1) % CTYPE_MAX);
        }
        else {
            SDL_FPoint boxScreenPos {TransformPointMat3(game->camera.cameraMat, (int)simPos.x, (int)simPos.y)};
            SDL_FRect box {boxScreenPos.x, boxScreenPos.y, game->camera.zoom, game->camera.zoom};
            SDL_SetRenderDrawColorFloat(game->renderer,1.0f,1.0f,1.0f,1.0f);
            SDL_RenderRect(game->renderer, &box);
            SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        }
    }

    // Reset playerclick to make sure the event does not constantly fire
    game->input.playerClick = false;
}

/////////////////////////
//        DRAW         //
////////////////////////

void DrawMap(GameState* game, Mat3& cameraMat){
    // Draw cells
    const auto& map {game->map};

    // Setup rect for use in drawing cells
    const std::size_t mapHeight {map.m.size()};
    const std::size_t mapWidth {map.m[0].size()};
    SDL_FRect target {0.0f, 0.0f, game->camera.zoom, game->camera.zoom};
    for (int y {0}; y < mapHeight; ++y){
        for (int x {0}; x < mapWidth; ++x){
            if (map.m[y][x].tp == CTYPE_WALL){
                SDL_FPoint screenPos {TransformPointMat3(cameraMat, x, y)};
                target.x = screenPos.x;
                target.y = screenPos.y;
                SDL_RenderTexture(game->renderer, game->textures[1], NULL, &target);
            }
        }
    }
}

void DrawPlayer (GameState* game, Mat3& cameraMat) {
    const auto& player {game->player};
    SDL_FPoint screenPos{TransformPointMat3(cameraMat, player.pos.x, player.pos.y)};
    float playerSize {GLOBALGAMESETTINGS.playerSizeFactor * GLOBALGAMESETTINGS.scale};
    float halfPlayerSize {playerSize/2.0f};
    SDL_FPoint centre {screenPos.x + halfPlayerSize, screenPos.y + halfPlayerSize};
    DrawCircle(game->renderer, centre, halfPlayerSize);
    //SDL_FRect target {screenPos.x, screenPos.y, playerSize, playerSize};
    //SDL_RenderTexture(game->renderer, game->textures[0], NULL, &target);
}

void DrawGame (GameState* game) {
    UpdateCamera(game->camera, game);
    Mat3 cameraMat {game->camera.cameraMat};

    DrawMap(game, cameraMat);
    DrawPlayer(game, cameraMat);
}

/////////////////////////
//        EXIT        //
////////////////////////

