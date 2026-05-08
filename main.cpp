#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

struct Vec2d {
    double x {};
    double y {};
};

struct GameSettings {
    int width  {1280};
    int height {720};
};

struct GameState {
    GameSettings settings  {};
    SDL_Window*   window   {nullptr};
    SDL_Renderer* renderer {nullptr};
};

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

    SDL_RaiseWindow(game->window);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event){
    switch (event->type){
        case (SDL_EVENT_QUIT): {
            return SDL_APP_SUCCESS;
        } break;
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate){
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result){
    GameState* game {(GameState*)appstate};
    SDL_DestroyRenderer(game->renderer);
    SDL_DestroyWindow(game->window);
}