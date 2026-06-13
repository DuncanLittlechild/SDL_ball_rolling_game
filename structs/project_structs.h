#ifndef PROJECT_STRUCTS_H
#define PROJECT_STRUCTS_H
#include "SDL3/SDL_rect.h"
#include <SDL3/SDL.h>
#include <string>
#include <string_view>

///////////////////////////////////
//           SETTINGS           //
//////////////////////////////////

struct GameSettings {
    int width  {1280};
    int height {720};
    float scale {20.0};
    float playerSizeFactor {0.5f};
    float velocityAdjustment {0.001f};
    float velocityMax {0.003f};
    Uint64 lastTime {0};
    int circleSegments{10};
};

inline GameSettings GLOBALGAMESETTINGS{};

///////////////////////////////////
//              CAMERA          //
//////////////////////////////////
struct Mat3 {
    float m[3][3];

    float& operator[](int row, int col){
        return m[row][col];
    }

    float operator[](int row, int col) const{
        return m[row][col];
    }

    Mat3& operator*=(const Mat3& om){
        *this = {{
            {m[0][0] * om[0,0]+ m[0][1] * om[1,0] + m[0][2] * om[2,0], m[0][0] * om[0,1] + m[0][1] * om[1,1] + m[0][2] * om[2,1], m[0][0] * om[0,2] + m[0][1] * om[1,2] + m[0][2] * om[2,2]},
            {m[1][0] * om[0,0]+ m[1][1] * om[1,0] + m[1][2] * om[2,0], m[1][0] * om[0,1] + m[1][1] * om[1,1] + m[1][2] * om[2,1], m[1][0] * om[0,2] + m[1][1] * om[1,2] + m[1][2] * om[2,2]},
            {m[2][0] * om[0,0]+ m[2][1] * om[1,0] + m[2][2] * om[2,0], m[2][0] * om[0,1] + m[2][1] * om[1,1] + m[2][2] * om[2,1], m[2][0] * om[0,2] + m[2][1] * om[1,2] + m[2][2] * om[2,2]},
        }};
        return *this;
    }

    Mat3 operator*(const Mat3& om){
        Mat3 tmp {*this};
        return tmp *= om;
    }
};

Mat3 GetScaleMat3 (float sX, float sY);

Mat3 GetTranslateMat3 (float tX, float tY);

// Applies a 3x3 matrix to transform a point
SDL_FPoint TransformPointMat3 (const Mat3& mat, float x, float y);

Mat3 InvertMat3(const Mat3& mat);

struct Camera2d {
    Mat3 cameraMat {};
    SDL_FPoint pos {0.0f, 0.0f};
    float screenW {static_cast<float>(GLOBALGAMESETTINGS.width)};
    float screenH {static_cast<float>(GLOBALGAMESETTINGS.height)};
    float zoom {GLOBALGAMESETTINGS.scale};
    float rot {0.0f};
    //Use data stored in the camera strucrt to create a 3x3 camera matrix
    // All transforms set up here will be applied to every point in turn
    void UpdateCameraMatrix() {
        // 1. Move the camera to the origin
        Mat3 moveToOrigin {GetTranslateMat3(-pos.x, -pos.y)};

        // 2. zoom at the origin
        Mat3 zoomMat {GetScaleMat3(zoom, zoom)};

        // 3. move the camera to the centre of the screen
        Mat3 moveToCentreScreen {GetTranslateMat3(screenW / 2.0f, screenH / 2.0f)};

        cameraMat = moveToCentreScreen * zoomMat * moveToOrigin;
    }
};

///////////////////////////////////
//              CELL          //
//////////////////////////////////

enum CellType {
    CTYPE_EMPTY,
    CTYPE_WALL,
    CTYPE_SPAWN,
    CTYPE_EXIT,
    CTYPE_DROP,
    CTYPE_MAX
};

struct Cell {
    CellType tp{};
};

///////////////////////////////////
//              MAP          //
//////////////////////////////////

#include <vector>
// Global vector used to provide a basic starting map prior to setting up file io
inline std::vector<std::vector<char>> startingMap {{
    {{1, 1, 1, 1, 1, 1, 1}},
    {{1 ,0, 0, 0, 0, 0 ,1}},
    {{1 ,0, 0, 0, 0, 0 ,1}},
    {{1 ,0, 0, 0, 0, 0 ,1}},
    {{1 ,0, 0, 0, 0, 0 ,1}},
    {{1 ,0, 0, 0, 0, 0 ,1}},
    {{1, 1, 1, 1, 1, 1, 1}},
}};

std::vector<std::vector<Cell>> CreateStartingMap(int h, int w);

struct GameMap {
    std::vector<std::vector<Cell>> m {};

    std::size_t Height () const {return m.size();}
    std::size_t Width () const {return m[0].size();}
};

void ChangeMapSize(GameMap* map, int h, int w);

///////////////////////////////////
//           PLAYER              //
//////////////////////////////////

struct Player {
    SDL_FPoint pos {};
    SDL_FPoint vel {};
    Uint32 flags {0};
};

///////////////////////////////////
//           INPUT              //
//////////////////////////////////

struct PlayerInput {
    float playerMousePosX {0.0f};
    float playerMousePosY {0.0f};
    bool playerClick {false};
    bool clickActive {false};
    bool lHeld {};
    bool rHeld {};
    bool uHeld {};
    bool dHeld {};
};

///////////////////////////////////
//           DIRECTIONS         //
//////////////////////////////////

enum DirectionI {
    DIRECTION_UP,
    DIRECTION_RIGHT,
    DIRECTION_DOWN,
    DIRECTION_LEFT,
    MAX_DIRECTION
};

///////////////////////////////////
//           GAMEMEMORY         //
//////////////////////////////////

enum GameMode {
    GAME_PLAYING,
    GAME_INITIALISING,
    GAME_LOADING,
    GAME_MENU,
    GAME_PAUSED,
    GAME_QUITTING,
    GAME_MAP_EDIT
};

struct GameState {
    SDL_Window*               window   {nullptr};
    SDL_Renderer*             renderer {nullptr};
    Player                    player   {};
    Camera2d                  camera   {};
    GameMap                   map      {};
    std::vector<SDL_Texture*> textures {};
    PlayerInput               input    {};
    GameMode                  state    {GAME_PLAYING};
};

/////////////////////////////////////
//       IMGUI PARAMETERS          //
////////////////////////////////////
// Struct to hold parameters adjusted by imgui
// Only there to ensure that parameters persist over multiple game loops
constexpr int maxMapNameLength{64};
struct ImGuiParameters {
    GameSettings gameSettings {GLOBALGAMESETTINGS};
    char mapName[maxMapNameLength];
    int mapHeight {0};
    int mapWidth {0};
    bool showMapEditWindow {false};
    int circleSegments{GLOBALGAMESETTINGS.circleSegments};
};
inline ImGuiParameters GLOBALIMGUIPARAMS{};


///////////////////////////////////
//           CONSTANTS           //
//////////////////////////////////
constexpr float FRICTION {0.95f};

constexpr float REBOUNDFRICTION {0.1f};
#endif
