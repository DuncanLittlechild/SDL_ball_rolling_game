#ifndef CAMERA_2D_H
#define CAMERA_2D_H
#include "SDL3/SDL_rect.h"
#include <SDL3/SDL.h>
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

struct Camera2d {
    SDL_FPoint pos {0.0f, 0.0f};
    float screenW {1280.0f};
    float screenH {720.0f};
    float zoom {1.0f};
    float rot {0.0f};
};

Mat3 GetScaleMat3 (float sX, float sY);

Mat3 GetTranslateMat3 (float tX, float tY);

// Applies a 3x3 matrix to transform a point
SDL_FPoint TransformPointMat3 (const Mat3& mat, float x, float y);

//Use data stored in the camera strucrt to create a 3x3 camera matrix
// All transforms set up here will be applied to every point in turn
Mat3 GetCameraMatrix(const Camera2d& cam);


#endif
