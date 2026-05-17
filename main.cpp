#include <cstddef>
#include <vector>
#include <SDL3/SDL.h>
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>
#include <iostream>
#include "project_structs.h"
#include "phases.h"
#include "dl_imgui_functions.h"



SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
    std::cout << "App initialised\n";
    if(!SDL_InitSubSystem(SDL_INIT_VIDEO)){
        SDL_Log("Failed to init video: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    GameState* game {new GameState};

    SDL_SetHint("SDL_RENDER_VSYNC", "1");
    game->window = SDL_CreateWindow("Ball rolling game", GLOBALGAMESETTINGS.width, GLOBALGAMESETTINGS.height, SDL_WINDOW_RESIZABLE);
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

    SDL_SetRenderLogicalPresentation(game->renderer, GLOBALGAMESETTINGS.width, GLOBALGAMESETTINGS.height, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    SDL_ShowWindow(game->window);

    SDL_RaiseWindow(game->window);

    InitMap(game);
    InitPlayer(game);
    InitImgui(game);
    UpdateCamera(game->camera, game);
    InitTextures(game);

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
                    game->input.uHeld = true;
                }break;
                case (SDL_SCANCODE_RIGHT): {
                    game->input.rHeld = true;
                }break;
                case (SDL_SCANCODE_DOWN): {
                    game->input.dHeld = true;
                }break;
                case (SDL_SCANCODE_LEFT): {
                    game->input.lHeld = true;
                }break;
            }
        } break;
        case (SDL_EVENT_KEY_UP): {
            switch (event->key.scancode) {
                case (SDL_SCANCODE_UP): {
                    game->input.uHeld = false;
                }break;
                case (SDL_SCANCODE_RIGHT): {
                    game->input.rHeld = false;
                }break;
                case (SDL_SCANCODE_DOWN): {
                    game->input.dHeld = false;
                }break;
                case (SDL_SCANCODE_LEFT): {
                    game->input.lHeld = false;
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

    // Get DeltaTime
    Uint64 currentTime {SDL_GetPerformanceCounter()};
    float deltaTime {(currentTime - GLOBALGAMESETTINGS.lastTime) / (float)SDL_GetPerformanceFrequency()};
    GLOBALGAMESETTINGS.lastTime = currentTime;
    switch (game->state) {
        case(GAME_PLAYING): {
            // Update player velocity and position
            UpdatePlayer(game, deltaTime);
            // Update camera matrix
            UpdateCamera(game->camera, game);
            Mat3 cameraMat {GetCameraMatrix(game->camera)};
            // Draw map
            DrawGame(game, cameraMat);
        } break;
        case(GAME_PAUSED): {
            // Update camera matrix
            UpdateCamera(game->camera, game);
            Mat3 cameraMat {GetCameraMatrix(game->camera)};
            // Draw map
            DrawGame(game, cameraMat);
        }
    }

    DrawImgui(game);

    SDL_RenderPresent(game->renderer);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result){
    GameState* game {(GameState*)appstate};
    SDL_DestroyRenderer(game->renderer);
    SDL_DestroyWindow(game->window);

    QuitImgui();
}