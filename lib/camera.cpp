#include "camera2d.h"


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

//Use data stored in the camera strucrt to create a 3x3 camera matrix
// All transforms set up here will be applied to every point in turn
Mat3 GetCameraMatrix(const Camera2d& cam){
    // 1. Move the camera to the origin
    Mat3 moveToOrigin {GetTranslateMat3(-cam.pos.x, -cam.pos.y)};

    // 2. zoom at the origin
    Mat3 zoomMat {GetScaleMat3(cam.zoom, cam.zoom)};

    // 3. move the camera to the centre of the screen
    Mat3 moveToCentreScreen {GetTranslateMat3(cam.screenW / 2.0f, cam.screenH / 2.0f)};

    return moveToCentreScreen * zoomMat * moveToOrigin;
}
