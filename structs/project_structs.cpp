#include "project_structs.h"

///////////////////////////////////
//              CAMERA          //
//////////////////////////////////

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

Mat3 InvertMat3(const Mat3& m) {
    // Source - https://stackoverflow.com/a/18504573
    // Posted by Cornstalks
    // Retrieved 2026-06-03, License - CC BY-SA 3.0

    // computes the inverse of a matrix m
    float det = m[0, 0] * (m[1, 1] * m[2, 2] - m[2, 1] * m[1, 2]) -
                 m[0, 1] * (m[1, 0] * m[2, 2] - m[1, 2] * m[2, 0]) +
                 m[0, 2] * (m[1, 0] * m[2, 1] - m[1, 1] * m[2, 0]);

    float invdet = 1 / det;

    Mat3 minv{}; // inverse of matrix m
    minv[0, 0] = (m[1, 1] * m[2, 2] - m[2, 1] * m[1, 2]) * invdet;
    minv[0, 1] = (m[0, 2] * m[2, 1] - m[0, 1] * m[2, 2]) * invdet;
    minv[0, 2] = (m[0, 1] * m[1, 2] - m[0, 2] * m[1, 1]) * invdet;
    minv[1, 0] = (m[1, 2] * m[2, 0] - m[1, 0] * m[2, 2]) * invdet;
    minv[1, 1] = (m[0, 0] * m[2, 2] - m[0, 2] * m[2, 0]) * invdet;
    minv[1, 2] = (m[1, 0] * m[0, 2] - m[0, 0] * m[1, 2]) * invdet;
    minv[2, 0] = (m[1, 0] * m[2, 1] - m[2, 0] * m[1, 1]) * invdet;
    minv[2, 1] = (m[2, 0] * m[0, 1] - m[0, 0] * m[2, 1]) * invdet;
    minv[2, 2] = (m[0, 0] * m[1, 1] - m[1, 0] * m[0, 1]) * invdet;

    return minv;
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

////////////////////////////////
//              MAP          //
///////////////////////////////

std::vector<std::vector<Cell>> CreateStartingMap(int dim) {
    std::vector<std::vector<Cell>> map {};
    map.reserve(dim);
    for (int y {0}; y < dim; ++y) {
        std::vector<Cell> row {};
        row.reserve(dim);
        for (int x {0}; x < dim; ++x) {
            if (y == 0 || x == 0 || y == dim - 1 || x == dim - 1) {
                row.push_back({static_cast<CellType>(1)});
            }
            else {
                row.push_back({static_cast<CellType>(0)});
            }
        }
        map.push_back(row);
    }
    return map;
}