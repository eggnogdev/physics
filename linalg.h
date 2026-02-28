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

void vec3_add(vec3 dest, vec3 a, vec3 b) {
    for (int i = 0; i < 3; i++) dest[i] = a[i] + b[i];
}

void vec3_unit(vec3 dest, vec3 v) {
    float lengthInverse = 1.0f / sqrt(pow(v[0], 2.0f) + pow(v[1], 2.0f) + pow(v[2], 2.0f));
    for (int i = 0; i < 3; i++) dest[i] = v[i] * lengthInverse;
}

void vec3_cross(vec3 dest, vec3 a, vec3 b) {
    dest[0] =   (a[1] * b[2] - a[2] * b[1]);
    dest[1] = - (a[0] * b[2] - a[2] * b[1]);
    dest[2] =   (a[0] * b[1] - a[1] * b[0]);
}

void mat4_identity(mat4 dest) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            dest[i][j] = i == j ? 1 : 0;
        }
    }
}

void mat4_translate(mat4 dest, vec3 v) {
    dest[0][3] = v[0];
    dest[1][3] = v[1];
    dest[2][3] = v[2];
}

void mat4_lookat(mat4 dest, vec3 eye, vec3 lookAt, vec3 up) {
    vec3 xAxis, yAxis, zAxis;

    vec3_sub(zAxis, eye, lookAt);
    vec3_unit(zAxis, zAxis);

    vec3_cross(xAxis, up, zAxis);
    vec3_unit(xAxis, xAxis);

    vec3_cross(yAxis, zAxis, xAxis);

    dest[0][0] = xAxis[0];
    dest[0][1] = yAxis[0];
    dest[0][2] = zAxis[0];
    dest[0][3] = 0;
    
    dest[0][0] = xAxis[1];
    dest[0][1] = yAxis[1];
    dest[0][2] = zAxis[1];
    dest[0][3] = 0;

    dest[0][0] = xAxis[2];
    dest[0][1] = yAxis[2];
    dest[0][2] = zAxis[2];
    dest[0][3] = 0;

    dest[0][0] = -vec3_dot(xAxis, eye);
    dest[0][1] = -vec3_dot(yAxis, eye);
    dest[0][2] = -vec3_dot(zAxis, eye);
    dest[0][3] = 1;
}
