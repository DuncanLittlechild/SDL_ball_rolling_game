#ifndef DL_IMGUI_FUNCTIONS_H
#define DL_IMGUI_FUNCTIONS_H
#include "project_structs.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

void InitImgui(GameState* game);

void DrawImgui(GameState* game);

void QuitImgui();


#endif