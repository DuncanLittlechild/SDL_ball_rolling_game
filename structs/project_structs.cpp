#include "project_structs.h"
#include "phases.h"
///////////////////////////////////
//           GAMEMEMORY         //
//////////////////////////////////
State *GameState::GetCurrentState() {
    return GLOBAL_STATES[states[states.size() - 1]];
}

///////////////////////////////////
//              MAP          //
//////////////////////////////////

bool GameMap::PointInMap(const SDL_FPoint &point) {
    return point.x >= 0 && point.x < m[0].size() && point.y >= 0 && point.y < m.size();
}

///////////////////////////////////
//           STATES             //
//////////////////////////////////
// Dominant state for when the game is being played

void PlayingState::update(GameState* game, float deltaTime) {
    // Update player velocity and position
    UpdatePlayer(game, deltaTime);
}
void PlayingState::render(GameState* game) {
    // Draw map
    DrawGame(game);
}
void PlayingState::onEnter(GameState* game) {

}
void PlayingState::onExit(GameState* game) {

}


// State when the game is being edited
void MapEditState::update(GameState* game, float deltaTime) {
    // Get the position of the mouse in render space
    float screenX, screenY;
    SDL_GetMouseState(&screenX, &screenY);

    float renderX, renderY;
    SDL_RenderCoordinatesFromWindow(game->renderer, screenX, screenY, &renderX, &renderY);

    // Invert the camera matrix to translate the render position into simulated space
    Mat3 camInv {InvertMat3(game->camera.cameraMat)};

    simPos = TransformPointMat3(camInv, renderX, renderY);
    simPosInMap = game->map.PointInMap(simPos);

    // If the player has clicked on a square, change its type.
    if (game->input.playerClick && simPosInMap) {
        auto& targetCell {game->map.m[(int)simPos.y][(int)simPos.x]};
        targetCell.tp = (CellType)((targetCell.tp + 1) % CTYPE_MAX);
    }

    // Reset playerclick to make sure the event does not constantly fire
    game->input.playerClick = false;
}
void MapEditState::render(GameState* game) {
    // If the mouse pointer is in the map, render a box around it
    if (simPosInMap) {
        SDL_FPoint boxScreenPos {TransformPointMat3(game->camera.cameraMat, (int)simPos.x, (int)simPos.y)};
        SDL_FRect box {boxScreenPos.x, boxScreenPos.y, game->camera.zoom, game->camera.zoom};
        SDL_SetRenderDrawColorFloat(game->renderer,1.0f,1.0f,1.0f,1.0f);
        SDL_RenderRect(game->renderer, &box);
        SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    }
}
void MapEditState::onEnter(GameState* game) {

}
void MapEditState::onExit(GameState* game) {

}

///////////////////////////////////
//              CAMERA          //
//////////////////////////////////

Mat3 GetScaleMat3 (float sX, float sY){
    Mat3 scaleMat {{
        {sX,  0, 0},
        { 0, sY, 0},
        { 0,  0, 1}
    }};

    return scaleMat;
}

Mat3 GetTranslateMat3 (float tX, float tY){
    Mat3 transformMat {{
        {1, 0, tX},
        {0, 1, tY},
        {0, 0, 1}
    }};

    return transformMat;
}

Mat3 InvertMat3(const Mat3& m) {
    // Source - https://stackoverflow.com/a/18504573
    // Posted by Cornstalks
    // Retrieved 2026-06-03, License - CC BY-SA 3.0

    // computes the inverse of a matrix m
    float det = m[0, 0] * (m[1, 1] * m[2, 2] - m[2, 1] * m[1, 2]) -
                 m[0, 1] * (m[1, 0] * m[2, 2] - m[1, 2] * m[2, 0]) +
                 m[0, 2] * (m[1, 0] * m[2, 1] - m[1, 1] * m[2, 0]);

    float invdet = 1 / det;

    Mat3 minv{}; // inverse of matrix m
    minv[0, 0] = (m[1, 1] * m[2, 2] - m[2, 1] * m[1, 2]) * invdet;
    minv[0, 1] = (m[0, 2] * m[2, 1] - m[0, 1] * m[2, 2]) * invdet;
    minv[0, 2] = (m[0, 1] * m[1, 2] - m[0, 2] * m[1, 1]) * invdet;
    minv[1, 0] = (m[1, 2] * m[2, 0] - m[1, 0] * m[2, 2]) * invdet;
    minv[1, 1] = (m[0, 0] * m[2, 2] - m[0, 2] * m[2, 0]) * invdet;
    minv[1, 2] = (m[1, 0] * m[0, 2] - m[0, 0] * m[1, 2]) * invdet;
    minv[2, 0] = (m[1, 0] * m[2, 1] - m[2, 0] * m[1, 1]) * invdet;
    minv[2, 1] = (m[2, 0] * m[0, 1] - m[0, 0] * m[2, 1]) * invdet;
    minv[2, 2] = (m[0, 0] * m[1, 1] - m[1, 0] * m[0, 1]) * invdet;

    return minv;
}

// Applies a 3x3 matrix to transform a point
SDL_FPoint TransformPointMat3 (const Mat3& mat, float x, float y){
    float pMat [3]{x, y, 1};
    float output [3] {0.0f};
    for (int i {0}; i < 3; ++i){
        for (int j{0}; j < 3; ++j){
            output[i] += mat[i,j] * pMat[j];
        }
    }
    return {output[0], output[1]};
}

////////////////////////////////
//              MAP          //
///////////////////////////////

void ChangeMapSize (GameMap* map, int h, int w) {
    if (h == map->Height() && w == map->Width()) {
        return;
    }

    // Check if the map needs resizing in either dimension
    if (h < map->Height() || h > map->Height()) {
        map->m.resize(h);
    }
    if (w < map->Width() || w > map->Width()) {
        for (auto& row : map->m) {
            row.resize(w);
        }
    }

    // Go around the edge and check that all edge squares are walls. If they are not, make them so
    for (int y {0}; y < h; ++y) {
        for (int x {0}; x < w; ++x) {
            if (y == 0 || x == 0 || y == h - 1 || x == w - 1) {
                map->m[y][x].tp = static_cast<CellType>(1);
            }
            else {
                map->m[y][x].tp = static_cast<CellType>(0);
            }
        }
    }
}

std::vector<std::vector<Cell>> CreateStartingMap(int h, int w) {
    std::vector<std::vector<Cell>> map {};
    map.reserve(h);
    for (int y {0}; y < h; ++y) {
        std::vector<Cell> row {};
        row.reserve(w);
        for (int x {0}; x < w; ++x) {
            if (y == 0 || x == 0 || y == h - 1 || x == w - 1) {
                row.push_back({static_cast<CellType>(1)});
            }
            else {
                row.push_back({static_cast<CellType>(0)});
            }
        }
        map.push_back(row);
    }
    return map;
}