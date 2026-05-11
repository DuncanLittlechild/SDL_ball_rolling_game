#ifndef MATRIX_H
#define MATRIX_H
#include <array>
#include "Vector2d.h"
struct Matrix3d {
    double mat [3][3] = {
        {0.0,0.0,0.0},
        {0.0,0.0,0.0},
        {0.0,0.0,0.0}
    };
};

Matrix3d MultiplyMatrix3d (const Matrix3d& l, const Matrix3d& r) {
    Matrix3d tmp {};
    for (int row {0}; row < 3; ++row){
        for (int col {0}; col < 3; ++col){
            double cellTotal{0.0};
            for (int i {0}; i < 3; ++i){
                cellTotal += l.mat[row][i] * r.mat[i][col];
            }
            tmp.mat[row][col] = cellTotal;
        }
    }
    return tmp;
}

Vec2d MultiplyMatrix3dVec2d (const Matrix3d& l, const Vec2d& r){
    return Vec2d {
        l.mat[0][0] * r.x + l.mat[0][1] * r.y,
        l.mat[1][0] * r.x + l.mat[1][0] * r.y
    };
} 


#endif