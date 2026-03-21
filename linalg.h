#ifndef __PHYSICS_LINALG_H
#define __PHYSICS_LINALG_H
#include <math.h>

typedef float   vec3[3];
typedef float   vec4[4];
typedef vec4    mat4[4];

float vec3_dot(vec3 a, vec3 b) {
    float res = 0.0f;
    for (int i = 0; i < 3; i++) res += a[i] * b[i];
    return res;
}

void vec3_sub(vec3 dest, vec3 a, vec3 b) {
    for (int i = 0; i < 3; i++) dest[i] = a[i] - b[i];
}

void vec3_negate(vec3 dest, vec3 v) {
    for (int i = 0; i < 3; i++) dest[i] = -v[i];
}

void vec3_add(vec3 dest, vec3 a, vec3 b) {
    for (int i = 0; i < 3; i++) dest[i] = a[i] + b[i];
}

void vec3_unit(vec3 dest, vec3 v) {
    float lengthInverse = 1.0f / sqrt(pow(v[0], 2.0f) + pow(v[1], 2.0f) + pow(v[2], 2.0f));
    for (int i = 0; i < 3; i++) dest[i] = v[i] * lengthInverse;
}

void vec3_cross(vec3 dest, vec3 a, vec3 b) {
    dest[0] =   (a[1] * b[2] - a[2] * b[1]);
    dest[1] = - (a[0] * b[2] - a[2] * b[0]);
    dest[2] =   (a[0] * b[1] - a[1] * b[0]);
}

void mat4_identity(mat4 dest) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            dest[i][j] = i == j ? 1.0f : 0.0f;
        }
    }
}

void mat4_mult(mat4 dest, mat4 A, mat4 B) {
    // naive method for now...
    dest[0][0] = A[0][0] * B[0][0] + A[0][1] * B[1][0] + A[0][2] * B[2][0] + A[0][3] * B[3][0];
    dest[0][1] = A[0][0] * B[0][1] + A[0][1] * B[1][1] + A[0][2] * B[2][1] + A[0][3] * B[3][1];
    dest[0][2] = A[0][0] * B[0][2] + A[0][1] * B[1][2] + A[0][2] * B[2][2] + A[0][3] * B[3][2];
    dest[0][3] = A[0][0] * B[0][3] + A[0][1] * B[1][3] + A[0][2] * B[2][3] + A[0][3] * B[3][3];

    dest[1][0] = A[1][0] * B[0][0] + A[1][1] * B[1][0] + A[1][2] * B[2][0] + A[1][3] * B[3][0];
    dest[1][1] = A[1][0] * B[0][1] + A[1][1] * B[1][1] + A[1][2] * B[2][1] + A[1][3] * B[3][1];
    dest[1][2] = A[1][0] * B[0][2] + A[1][1] * B[1][2] + A[1][2] * B[2][2] + A[1][3] * B[3][2];
    dest[1][3] = A[1][0] * B[0][3] + A[1][1] * B[1][3] + A[1][2] * B[2][3] + A[1][3] * B[3][3];

    dest[2][0] = A[2][0] * B[0][0] + A[2][1] * B[1][0] + A[2][2] * B[2][0] + A[2][3] * B[3][0];
    dest[2][1] = A[2][0] * B[0][1] + A[2][1] * B[1][1] + A[2][2] * B[2][1] + A[2][3] * B[3][1];
    dest[2][2] = A[2][0] * B[0][2] + A[2][1] * B[1][2] + A[2][2] * B[2][2] + A[2][3] * B[3][2];
    dest[2][3] = A[2][0] * B[0][3] + A[2][1] * B[1][3] + A[2][2] * B[2][3] + A[2][3] * B[3][3];

    dest[3][0] = A[3][0] * B[0][0] + A[3][1] * B[1][0] + A[3][2] * B[2][0] + A[3][3] * B[3][0];
    dest[3][1] = A[3][0] * B[0][1] + A[3][1] * B[1][1] + A[3][2] * B[2][1] + A[3][3] * B[3][1];
    dest[3][2] = A[3][0] * B[0][2] + A[3][1] * B[1][2] + A[3][2] * B[2][2] + A[3][3] * B[3][2];
    dest[3][3] = A[3][0] * B[0][3] + A[3][1] * B[1][3] + A[3][2] * B[2][3] + A[3][3] * B[3][3];
}

void mat4_translation(mat4 dest, vec3 v) {
    mat4_identity(dest);

    dest[0][3] = v[0];
    dest[1][3] = v[1];
    dest[2][3] = v[2];
}

void mat4_translate(mat4 dest, mat4 M, vec3 t) {
    mat4 T;
    mat4_identity(T);
    mat4_translation(T, t);

    mat4_mult(dest, T, M);
}

void mat4_transpose(mat4 dest, mat4 M) {
    // TODO: improve this function
    mat4 T; // temp matrix in case dest is also M
    mat4_identity(T);
    for (size_t i = 0; i < 4; i++) {
        for (size_t j = 0; j < 4; j++) {
            T[j][i] = M[i][j];
        }
    }

    for (size_t i = 0; i < 4; i++) {
        for (size_t j = 0; j < 4; j++) {
            dest[i][j] = T[i][j];
        }
    }
}

void mat4_lookat(mat4 dest, vec3 eye, vec3 center, vec3 up) {
    // https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/gluLookAt.xml
    mat4_identity(dest);

    vec3 f, upUnit, s, u;
    vec3_sub(f, center, eye);
    vec3_unit(f, f);

    vec3_unit(upUnit, up); 

    vec3_cross(s, f, upUnit);

    vec3_unit(s, s);
    vec3_cross(u, s, f);

    mat4 m;
    mat4_identity(m);
    m[0][0] =  s[0];
    m[0][1] =  s[1];
    m[0][2] =  s[2];
    m[1][0] =  u[0];
    m[1][1] =  u[1];
    m[1][2] =  u[2];
    m[2][0] = -f[0];
    m[2][1] = -f[1];
    m[2][2] = -f[2];

    vec3 eyeNegative;
    vec3_negate(eyeNegative, eye);

    mat4_translate(dest, m, eyeNegative);
}

void mat4_perspective(mat4 dest, float fov, float aspect, float near, float far) {
    // https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/gluPerspective.xml
    mat4_identity(dest);
    dest[3][3] = 0;

    float deg2rad = acos(-1.0f) / 180.0f;

    float f = 1.0f / tan((fov * deg2rad) / 2.0f);

    dest[0][0] = f / aspect;
    dest[1][1] = f;
    dest[2][2] = (far + near) / (near - far);
    dest[2][3] = (2.0f * far * near) / (near - far);
    dest[3][2] = -1.0f;
}

#endif // __PHYSICS_LINALG_H
