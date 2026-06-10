#include "dl_imgui_functions.h"
#include "imgui_impl_sdlrenderer3.h"
#include "phases.h"
#include "project_utils.h"


void InitImgui(GameState* game) {
    // Basic setup
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Set appearance and scaling of ImGui
    //ImGuiStyle& style = ImGui::GetStyle();
    //style.ScaleAllSizes(GameSettings::INITIALSCALE);
    // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    //style.FontScaleDpi = GameSettings::INITIALSCALE;
    ImGui::StyleColorsDark();

    // Enable keyboard navigation in imgui
    ImGuiIO& io {ImGui::GetIO()};
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Initialise ImGui globals for SDL
    ImGui_ImplSDL3_InitForSDLRenderer(game->window, game->renderer);
    ImGui_ImplSDLRenderer3_Init(game->renderer);
}

// Draws Imgui window that displays stats
void DrawImguiStatDisplay(GameState* game) {
    //ImGui::ShowDemoWindow();
    // Create widgets for game settings
    ImGui::Begin("Adjust Settings");

    ImGui::Text("Player position: x = %f; y = %f", game->player.pos.x, game->player.pos.y);
    ImGui::Text("Player Velocity: x = %f; y = %f", game->player.vel.x, game->player.vel.y);
    ImGui::Text("Total Player Velocity: %f", SDL_FPointMagnitude(game->player.vel));

    ImGui::InputFloat("Scale", &GLOBALIMGUIPARAMS.gameSettings.scale);
    ImGui::InputFloat("Player size", &GLOBALIMGUIPARAMS.gameSettings.playerSizeFactor);
    ImGui::InputFloat("Player max velocity", &GLOBALIMGUIPARAMS.gameSettings.velocityMax);
    ImGui::InputFloat("Velocity increment", &GLOBALIMGUIPARAMS.gameSettings.velocityAdjustment);

    if (ImGui::Button("Adjust Settings")) {
        GLOBALGAMESETTINGS = GLOBALIMGUIPARAMS.gameSettings;
    }

    ImGui::End();
}

// Draws imgui window that allows map to be edited
void DrawImguiMapEditor(GameState* game) {
    ImGui::Begin("Adjust Map");
    // Resize the map and wipe its contents
    ImGui::InputInt("New Map Height: %d", &GLOBALIMGUIPARAMS.mapHeight);
    ImGui::InputInt("New Map Width: %d", &GLOBALIMGUIPARAMS.mapWidth);

    // If the button is pressed, the new map is sufficiently large, and the player would be inside it, resize the map
    if (ImGui::Button("Adjust Map Size")
        && GLOBALIMGUIPARAMS.mapWidth > 4
        && GLOBALIMGUIPARAMS.mapHeight > 4
        && (int)(game->player.pos.x + GLOBALGAMESETTINGS.playerSizeFactor) < GLOBALIMGUIPARAMS.mapWidth
        && (int)(game->player.pos.y + GLOBALGAMESETTINGS.playerSizeFactor) < GLOBALIMGUIPARAMS.mapHeight) {
        ChangeMapSize(&game->map, GLOBALIMGUIPARAMS.mapHeight, GLOBALIMGUIPARAMS.mapWidth);
    }
    // Button to pause the game and start the map editor
    if (game->state != GAME_MAP_EDIT && ImGui::Button("Edit Map")) {
        game->state = GAME_MAP_EDIT;
        GLOBALIMGUIPARAMS.showMapEditWindow = true;
    }
    else if (ImGui::Button("Exit edit mode")) {
        game->state = GAME_PLAYING;
        GLOBALIMGUIPARAMS.showMapEditWindow = false;
    }
    ImGui::End();
}

void DrawImgui(GameState* game){
    // Prepare the backend for a new frame
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    // USes the data create in the prior step to prepare
    // ImGui for new widget commands
    ImGui::NewFrame();
    DrawImguiStatDisplay(game);

    DrawImguiMapEditor(game);
    // Popup to display the error caused by trying to place a wall in a player
    if (ImGui::BeginPopupModal("wall_placed_on_player_error", NULL,ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Unable to save new design - player would be inside a wall");
        if (ImGui::Button("Confirm##confirm_wall_placed_on_player_error")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    // Convert the widget calls into a set of gpu-friendly
    // platform agnostic commands
    ImGui::Render();

    // Retrieve and submit the rendered data to the GPU
    ImGui_ImplSDLRenderer3_RenderDrawData(
        ImGui::GetDrawData(), game->renderer
    );
}

void QuitImgui(){
    // Shutdown ImGui
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}