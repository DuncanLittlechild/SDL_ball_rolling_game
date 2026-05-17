#include "phases.h"
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

    // Playersize is the relative size of a player compared to a cell. A cell is of size 1
    float playerSize {GLOBALGAMESETTINGS.playerSizeFactor};
    float halfPlayerSize{playerSize/2.0f};
    SDL_FPoint collisionOffsets[4] {
        {halfPlayerSize, 0.0f}, // North
        {playerSize, halfPlayerSize}, // East
        {halfPlayerSize, playerSize}, // South
        {0.0f, halfPlayerSize} // West
    };
    // check for collisions
    bool yUpdate {true};
    bool xUpdate {true};
    for (auto& offset : collisionOffsets) {
        SDL_FPoint cardinalPoint {newPosition.x + offset.x, newPosition.y + offset.y};
        if (game->map.m[static_cast<std::size_t>(cardinalPoint.y)][static_cast<std::size_t>(cardinalPoint.x)].tp == CTYPE_WALL) {
            if (offset.x == halfPlayerSize) {
                player.vel.y = -(player.vel.y * 0.1);
                yUpdate = false;
            }
            if (offset.y == halfPlayerSize) {
                player.vel.x = -(player.vel.x * 0.1);
                xUpdate = false;
            }
        }
    }
    if (yUpdate) {
        player.pos.y = newPosition.y;
    }
    if (xUpdate) {
        player.pos.x = newPosition.x;
    }
    // Collisions will need to
    // If there are collisions, update position and velocity to reflect this
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
    SDL_FRect target {screenPos.x, screenPos.y, playerSize, playerSize};
    SDL_RenderTexture(game->renderer, game->textures[0], NULL, &target);
}

void DrawGame (GameState* game, Mat3& cameraMat) {
    DrawMap(game, cameraMat);
    DrawPlayer(game, cameraMat);
}

/////////////////////////
//        EXIT        //
////////////////////////

