#include <stdio.h>

#define LVN_GMATH_IMPL
#include <levikno/lvn_gmath.h>


void printMat2(LvnMat2 m)
{
    printf("| %.2f, %.2f |\n", m[0][0], m[0][1]);
    printf("| %.2f, %.2f |\n", m[1][0], m[1][1]);
}

void printMat3(LvnMat3 m)
{
    printf("| %.2f, %.2f, %.2f |\n", m[0][0], m[0][1], m[0][2]);
    printf("| %.2f, %.2f, %.2f |\n", m[1][0], m[1][1], m[1][2]);
    printf("| %.2f, %.2f, %.2f |\n", m[2][0], m[2][1], m[2][2]);
}

int main(int argc, char** argv)
{
    LvnVec2 v2a = { 3.0f, 4.0f };
    LvnVec2 v2b = { 3.0f, 4.0f };

    lvn_vec2_addvs(v2a, 1.5f, v2a);

    LvnVec2 v2c;
    lvn_vec2_add(v2a, v2b, v2c);

    printf("v2c: <x:%.2f,y:%.2f>\n", v2c[0], v2c[1]);

    float m[4] = { 5, 6, 7, 8 };

    LvnMat2 mat2a;
    lvn_mat2(m, mat2a);
    printMat2(mat2a);

    LvnMat2 mat2b = { { 1, 2 }, { 3, 4 } };
    printMat2(mat2b);

    LvnMat2 mat2c;
    lvn_mat2_mult(mat2a, mat2b, mat2c);

    printMat2(mat2c);

    lvn_mat2_transpose(mat2c);
    printMat2(mat2c);

    LvnMat3 mat3a;
    lvn_mat3_identity(mat3a);

    printMat3(mat3a);

    return 0;
}
