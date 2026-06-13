//
// Created by duncan on 5/17/26.
//

#include "project_utils.h"

#include <cassert>
#include <algorithm>
#include <fstream>
#include <iostream>

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

////////////////////////////////////
//        FILEIO UTILS         //
////////////////////////////////////

// Formats the string chosen for the file name correctly
std::string GetFileName (const char* fileString) {
    int i {0};
    std::string fileName {""};
    while (fileString[i] != '\0') {
        fileName += fileString[i];
        ++i;
    }
    if (fileName == "" ) {
        fileName = startingMapName;
    }
    else {
        fileName += ".txt";
    }
    return fileName;
}

// Iterate over the vector and save it to an external file
// Currently saves map by converting each row into a line of integers, where the
// integer in question is the underlying value of the type enum
bool SaveGameMap(const GameMap& map, const char fileString[]) {
    std::string fileName {GetFileName(fileString)};
    std::ofstream outFile{fileName};
    if (!outFile) {
        std::cerr<< "Error opening file to save\n";
        return false;
    }

    for (int i {0}; i < map.Height(); ++i) {
        for (int j {0}; j < map.Width(); ++j) {
            outFile << map.m[i][j].tp;
        }
        outFile << "\n";
    }
    outFile.close();
    return true;
}

// Loads a game map from a txt file.
// Currently the map is encoded as lines of integers 0 to 9
// with the interger corresponding to the type of the cell in question
// and each line standing in for a row of the map array.
bool LoadGameMap(GameMap& map, const char fileString[]) {
    std::string fileName {GetFileName(fileString)};
    std::ifstream inFile {fileName};
    if (!inFile) {
        std::cerr << "Error opening file to load\n";
        return false;
    }

    // Creates a new map array from the relevant file.
    // If any characters in the file are not in the char range 0-9, then the function
    // returns false.
    std::vector<std::vector<Cell>> newMap{};
    newMap.reserve(map.Height());
    std::string line{""};
    int lineCount {1};
    while (getline(inFile, line)) {
        std::vector<Cell> row {};
        row.reserve(line.size());
        for (int i {0}; i < line.size(); ++i) {
            if (line[i] < '0' || line[i] > '9') {
                std::cerr << "Failed to load map: invalid character "<< line[i] << " in file "
                         << fileName << " at line " << lineCount << ", character " << i <<  ".\n";
                return false;
            }
            row.emplace_back(static_cast<CellType>((line[i] - '0')));
        }
        newMap.push_back(row);
        ++lineCount;
    }

    map.m = newMap;

    return true;
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

bool DetectPlayerCollisions(const SDL_FPoint c, const float r, std::vector<SDL_FPoint>* collisionNorms, const GameMap& map) {
    // Get all solid cells that are within a square drawn around the circle of centre c and radius r
    float rSquared {r * r};
    bool playerCollides {false};

    float playerYMin {c.y - GLOBALGAMESETTINGS.playerSizeFactor};
    float playerXMin {c.x - GLOBALGAMESETTINGS.playerSizeFactor};
    float playerYMax {c.y + GLOBALGAMESETTINGS.playerSizeFactor};
    float playerXMax {c.x + GLOBALGAMESETTINGS.playerSizeFactor};

    std::vector<SDL_FPoint> solidCells {};
    solidCells.reserve(8);

    for (int y = playerYMin; y <= (int)playerYMax; ++y) {
        for (int x = playerXMin; x <= (int)playerXMax; ++x) {
            if (map.m[y][x].tp == CTYPE_WALL) {
                solidCells.emplace_back(x, y);
            }
        }
    }

    // For each solid cell, find the closest point with the clamp method
    for (auto& cell : solidCells) {
        float cellXMax {cell.x + 1.0f};
        float cellYMax {cell.y + 1.0f};
        SDL_FPoint closestPoint {
            std::ranges::max(cell.x, std::ranges::min(cellXMax, c.x)),
            std::ranges::max(cell.y, std::ranges::min(cellYMax, c.y))
        };
        // If the distance to the closest point squared is equal to or less than the radius squared, there is a collision
        float xDist {closestPoint.x - c.x};
        float yDist {closestPoint.y - c.y};
        float distanceSquared {xDist * xDist + yDist * yDist};
        if (distanceSquared < rSquared) {
            playerCollides = true;
            // If collisionNorms is null, DetectCollision is being used just to tell if a collision exists
            if (collisionNorms != nullptr) {
                // Determine the surface normal vector from the collision surface
                // Checks to see if the closest point is on an edge
                // First check to see if the collisionc++ round float down is on the y axis
                if ((closestPoint.y == cell.y || closestPoint.y == cellYMax)
                    && (closestPoint.x > cell.x && closestPoint.x < cellXMax)) {
                    collisionNorms->emplace_back(0.0f, c.y < cell.y ? -1.0f : 1.0f);
                }
                // Then check the x axis
                else if ((closestPoint.x == cell.x || closestPoint.x == cellXMax)
                        && (closestPoint.y > cell.y && closestPoint.y < cellYMax)) {
                    collisionNorms->emplace_back(c.x < cell.x ? -1.0f : 1.0f, 0.0f);
                }
                // If it is heading towards a corner, determine if it is more x or y, then rebound in that direction
                // if ydiff is less than xdiff, it is rebounding in the x axis
                else  {
                    float yNorthDist {std::abs(c.y - cell.y)};
                    float ySouthDist {std::abs(c.y - cellYMax)};
                    float xWestDist {std::abs(c.x - cell.x)};
                    float xEastDist {std::abs(c.x - cellXMax)};
                    float yDiff {std::ranges::min(yNorthDist, ySouthDist)};
                    float xDiff {std::ranges::min(xWestDist, xEastDist)};
                    if (yDiff < xDiff) {
                        collisionNorms->emplace_back(xWestDist > xEastDist ? -1.0f : 1.0f, 0.0f);
                    }
                    else {
                        collisionNorms->emplace_back(0.0f, yNorthDist > ySouthDist ? -1.0f : 1.0f);
                    }
                }

                // if the absolute difference between c.x and either the min or max is less than the equivalent for
                // y, it is rebounding in the y axis. In the opposite case, it is rebounding in the x axis
            }
            else {
                break;
            }
        }
    }
    return playerCollides;
}

void DrawCircle(SDL_Renderer* renderer, SDL_FPoint centre, float radius, SDL_FColor color) {
    // use radius to determine number of segments
    int segments {GLOBALGAMESETTINGS.circleSegments};
    std::vector<SDL_Vertex> vertices (segments + 2, SDL_Vertex{});

    // Create central vertex
    vertices[0] = {
        centre,
        color,
        SDL_FPoint{0.0f, 0.0f}
    };
    // Create fan of outlying vertices using good ole trig
    for (int i {0}; i <= segments; ++i) {
        float angle {2.0f * SDL_PI_F * i / (float)(segments)};
        vertices[i + 1] = {
            SDL_FPoint{SDL_cosf(angle) * radius + centre.x, SDL_sinf(angle) * radius + centre.y},
            color,
            SDL_FPoint{0.0f, 0.0f}
        };
    }
    // Create array of indices to use to draw the circle
    std::vector<int> indices (segments * 3, 0);
    for (int i {0}; i < (segments); ++i) {
        indices[i * 3] = 0;
        indices[i * 3 + 1] = i + 1;
        indices[i * 3 + 2] = i + 2;
    }
    SDL_RenderGeometry(renderer,NULL, vertices.data(), segments + 2, indices.data(), segments * 3);
}


