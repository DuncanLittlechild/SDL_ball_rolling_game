#ifndef INIT_H
#define INIT_H
#include "project_structs.h"

///////////////////////////////////
//        INITIALISATION        //
//////////////////////////////////

void InitMap(GameState* game);

void InitPlayer(GameState* game);

void InitTextures(GameState* game);

///////////////////////////
//        UPDATE        //
//////////////////////////

void UpdateCamera(Camera2d& camera, GameState* game);

void UpdatePlayer(GameState* game, float deltaTime);

// Updates the map if the player is in map edit mode
void UpdateMap(GameState* game);

/////////////////////////
//        DRAW         //
////////////////////////
void DrawMap (GameState* game, Mat3& cameraMat);

void DrawPlayer (GameState* game, Mat3& cameraMat);

void DrawGame(GameState* game);

/////////////////////////
//        EXIT        //
////////////////////////


#endif