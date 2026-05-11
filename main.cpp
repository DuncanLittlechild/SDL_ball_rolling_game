#include <SDL3/SDL_events.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_scancode.h>
#include <vector>
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "imgui.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlrenderer3.h"
#include "Vector2d.h"
#include "matrix.h"

struct Player {
    Vec2d pos {};
    Vec2d vel {};
};

struct Camera {
    Matrix3d matrix {};
    Vec2d    pos    {};
    double   zoom   {};
};

struct GameSettings {
    int width  {1280};
    int height {720};
};

enum CellType {
    EMPTY,
    WALL,
    SPAWN,
    EXIT
};

struct Cell {
    CellType type{};
};

std::vector<std::vector<char>> startingMap {{
    {{1, 1, 1, 1, 1, 1, 1}},
    {{1 ,0, 0, 0, 0, 0 ,1}},
    {{1 ,0, 0, 0, 0, 0 ,1}},
    {{1 ,0, 0, 0, 0, 0 ,1}},
    {{1 ,0, 0, 0, 0, 0 ,1}},
    {{1 ,0, 0, 0, 0, 0 ,1}},
    {{1, 1, 1, 1, 1, 1, 1}},
}};

struct GameMap {
    std::vector<std::vector<Cell>> m {};
};

enum InputCodeShift {
    INPUT_UP,
    INPUT_RIGHT,
    INPUT_DOWN,
    INPUT_LEFT
};

struct ExternalInput {
    char buttonsPressed {0};
};

struct GameState {
    GameSettings  settings {};
    SDL_Window*   window   {nullptr};
    SDL_Renderer* renderer {nullptr};
    Player        player   {};
    Camera        camera   {};
    GameMap       map      {};
    ExternalInput input    {};
};

void InitImgui(GameState* game){
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplSDL3_InitForSDLRenderer(game->window, game->renderer);
    ImGui_ImplSDLRenderer3_Init(game->renderer);
}

void DrawImgui(GameState* game){

}

void QuitImgui(){
    // Shutdown ImGui
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}

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
}

void InitPlayer(GameState* game){
    game->player = {2.0,2.0};
}

void UpdateCamera(Camera& camera, GameState* game){
    camera.pos = game->player.pos;
    Matrix3d moveCameraToOrigin {{
        {1.0, 0.0, -camera.pos.x},
        {0.0, 1.0, -camera.pos.y},
        {0.0, 0.0, 1.0}
    }};

    Matrix3d zoomCamera {{
        {camera.zoom, 0.0, 0.0},
        {0.0, camera.zoom, 0.0},
        {0.0, 0.0, 1.0}
    }};

    Matrix3d moveCameraToCentreOfScreen {{
        {1.0, 0.0, (double)(game->settings.width)/2.0},
        {0.0, 1.0, (double)(game->settings.height)/2.0},
        {0.0, 0.0, 1.0}
    }};

    camera.matrix = MultiplyMatrix3d(moveCameraToCentreOfScreen, MultiplyMatrix3d(zoomCamera, moveCameraToOrigin));
}

void DrawMap(GameState* game){
    // Draw cells
    auto map {game->map};
    // Setup rect for use in drawing cells
    SDL_FRect
    for (auto& row : map.m){
        for (auto& c : row){

        }
    }

    // Draw player
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
    if(!SDL_InitSubSystem(SDL_INIT_VIDEO)){
        SDL_Log("Failed to init video: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    GameState* game {new GameState};

    SDL_SetHint("SDL_RENDER_VSYNC", "1");
    game->window = SDL_CreateWindow("Ball rolling game", game->settings.width, game->settings.height, SDL_WINDOW_RESIZABLE);
    if(!game->window){
        SDL_Log("Failed to initialise window: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    game->renderer = SDL_CreateRenderer(game->window, NULL);

    if (!game->renderer){
        SDL_Log("Failed to initialised renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_SetRenderVSync(game->renderer, 1);

    SDL_SetRenderLogicalPresentation(game->renderer, game->settings.width, game->settings.height, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    SDL_ShowWindow(game->window);

    SDL_RaiseWindow(game->window);

    InitMap(game);
    InitPlayer(game);
    UpdateCamera(game->camera, game);

    *appstate = game;

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event){
    GameState* game {(GameState*)appstate};

    ImGui_ImplSDL3_ProcessEvent(event);
    switch (event->type){
        case(SDL_EVENT_KEY_DOWN): {
            switch (event->key.scancode) {
                case (SDL_SCANCODE_UP): {
                    game->input.buttonsPressed |= 1 << INPUT_UP;
                }break;
                case (SDL_SCANCODE_RIGHT): {
                    game->input.buttonsPressed |= 1 << INPUT_RIGHT;
                }break;
                case (SDL_SCANCODE_DOWN): {
                    game->input.buttonsPressed |= 1 << INPUT_DOWN;
                }break;
                case (SDL_SCANCODE_LEFT): {
                    game->input.buttonsPressed |= 1 << INPUT_LEFT;
                }break;
            }
        } break;
        case (SDL_EVENT_KEY_UP): {
            switch (event->key.scancode) {
                case (SDL_SCANCODE_UP): {
                    game->input.buttonsPressed &= ~(1 << INPUT_UP);
                }break;
                case (SDL_SCANCODE_RIGHT): {
                    game->input.buttonsPressed &= ~(1 << INPUT_RIGHT);
                }break;
                case (SDL_SCANCODE_DOWN): {
                    game->input.buttonsPressed &= ~(1 << INPUT_DOWN);
                }break;
                case (SDL_SCANCODE_LEFT): {
                    game->input.buttonsPressed &= ~(1 << INPUT_LEFT);
                }break;
            }
        } break;
        case (SDL_EVENT_QUIT): {
            return SDL_APP_SUCCESS;
        } break;
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate){
    GameState* game {(GameState*)appstate};
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(game->renderer);

    // Update player velocity
    UpdatePlayerVelocity(game);

    // update player position
    UpdatePlayerPosition();

    // Update camera matrix
    UpdateCamera(game->camera, game);

    // Draw map
    DrawGameMap();

    SDL_RenderPresent(game->renderer);

    DrawImgui(game);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result){
    GameState* game {(GameState*)appstate};
    SDL_DestroyRenderer(game->renderer);
    SDL_DestroyWindow(game->window);

    QuitImgui();
}