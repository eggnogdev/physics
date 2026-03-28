#include "glad/glad.h"

#include <alloca.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <GLFW/glfw3.h>

#include "array.h"
#include "circle.h"
#include "linalg.h"
#include "shader.h"

DEFINE_DYNAMIC_ARRAY(float, float_array);

const uint32_t START_WIDTH = 1080;
const uint32_t START_HEIGHT = 720;
static float aspectRatio = (float)START_WIDTH / (float)START_HEIGHT;

void circleToWorldTransform(struct Circle c, mat4 dest) {
    mat4_identity(dest);
    mat4_translation(dest, (vec3){c.position.x, c.position.y, 0.0f});
}

void applyGravity(float timestep, size_t count, struct Circle* circle) {
    for (size_t i = 0; i < count; i++) {
        circle[i].velocity.y -= (9.815f * timestep);
        circle[i].position.y += (circle[i].velocity.y * timestep);
    }
}

void displayCircle(struct Circle* circle) {
    printf(
        "Position: %f, %f | Velocity: %f, %f | Radius: %f | Mass: %f\n",
        circle->position.x, circle->position.y,
        circle->velocity.x, circle->velocity.y,
        circle->mass, circle->radius
    );
}

void glfwErrorCallback(int error, const char* message) {
    fprintf(stderr, "Error %d: %s\n", error, message);
}

void glfwFramebufferSizeCallback(GLFWwindow* _window, int width, int height) {
    glViewport(0, 0, width, height);
    aspectRatio = width / height;
}

void glfwProcessInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

float rand_float(float min, float max) {
    return ((float)rand() / (float)RAND_MAX) * (max - min) + min;
}

int main() {
    srand(time(NULL));
    struct dynamic_struct_Circle_array *circles_dynamic_array = init_dynamic_struct_Circle_array();
    for (int i = 0; i < 10; i++) {
        struct Circle c = {
            .radius = 0.125f,
            .mass = 5.0f,
            .velocity = {
                .x = 0.0f,
                .y = rand_float(0.0f, 5.0f),
            },
            .position = {
                .x = rand_float(-1.0f, 1.0f),
                .y = 0.0f,
            },
        };

        dynamic_struct_Circle_array_push(&circles_dynamic_array, 1, &c);
    }

    struct dynamic_float_array *circles_transform_buffer = init_dynamic_float_array();
    dynamic_float_array_set_size(&circles_transform_buffer, circles_dynamic_array->length * 16);
    struct dynamic_float_array *circles_quad_buffer = init_dynamic_float_array();
    dynamic_float_array_set_size(&circles_quad_buffer, circles_dynamic_array->length * 18);

    glfwSetErrorCallback(glfwErrorCallback);
    glfwInitHint(GLFW_PLATFORM, GLFW_ANY_PLATFORM);
    if (!glfwInit()) {
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);
    GLFWwindow* window = glfwCreateWindow(START_WIDTH, START_HEIGHT, "Physics", NULL, NULL);
    if (window == NULL) {
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwTerminate();
        return 1;
    }

    glViewport(0, 0, START_WIDTH, START_HEIGHT);
    glfwSetFramebufferSizeCallback(window, glfwFramebufferSizeCallback);

    uint32_t circleShaderHandle = create_shader_program_from_file("circle.vert.glsl", "circle.frag.glsl");

    uint32_t VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    uint32_t circlesVertexPositionVBO;
    glGenBuffers(1, &circlesVertexPositionVBO);
    glBindBuffer(GL_ARRAY_BUFFER, circlesVertexPositionVBO);

    dynamic_struct_Circle_array_to_vertex_buffer(circles_dynamic_array, circles_quad_buffer->elements);
    circles_quad_buffer->length = circles_dynamic_array->length * 18;
    glBufferData(GL_ARRAY_BUFFER, circles_quad_buffer->length * sizeof(float), circles_quad_buffer->elements, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
    glEnableVertexAttribArray(0);

    uint32_t circlesRadiusBuffer;
    glGenBuffers(1, &circlesRadiusBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, circlesRadiusBuffer);

    float circle_radius_array[circles_dynamic_array->length];
    dynamic_struct_Circle_array_to_radius_buffer(circles_dynamic_array, circle_radius_array);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(circle_radius_array), circle_radius_array, GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, circlesRadiusBuffer);

    uint32_t circlesModelToWorldMatrixBuffer;
    glGenBuffers(1, &circlesModelToWorldMatrixBuffer);

    glUseProgram(circleShaderHandle);

    mat4 worldToCameraMatrix;
    vec3 eye = { 0.0f, 0.0f, 2.0f };
    vec3 center = { 0.0f, 0.0f, 0.0f };
    vec3 up = { 0.0f, 1.0f, 0.0f };
    mat4_lookat(worldToCameraMatrix, eye, center, up);

    // TODO: move worldToCameraMatrix and cameraToScreenMatrix to UBO
    char worldToCameraMatrixUniformName[] = "worldToCameraMatrix";
    uint32_t worldToCameraMatrixUL = glGetUniformLocation(circleShaderHandle, worldToCameraMatrixUniformName);
    glUniformMatrix4fv(worldToCameraMatrixUL, 1, GL_TRUE, (float*)worldToCameraMatrix);

    mat4 cameraToScreenMatrix;
    mat4_perspective(cameraToScreenMatrix, 60.0f, aspectRatio, 0.1f, 100.0f);

    // TODO: update cameraToScreenMatrix upon viewport/framebuffer size change
    char cameraToScreenMatrixUniformName[] = "cameraToScreenMatrix";
    uint32_t cameraToScreenMatrixUL = glGetUniformLocation(circleShaderHandle, cameraToScreenMatrixUniformName);
    glUniformMatrix4fv(cameraToScreenMatrixUL, 1, GL_TRUE, (float*)cameraToScreenMatrix);
    
    float lastFrameTime = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        float thisFrameTime = glfwGetTime();
        float deltaTime = thisFrameTime - lastFrameTime;
        lastFrameTime = thisFrameTime;

        glfwProcessInput(window);

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        applyGravity(deltaTime * 0.1f, circles_dynamic_array->length, circles_dynamic_array->elements);
        dynamic_struct_Circle_array_to_transform_buffer(circles_dynamic_array, circles_transform_buffer->elements);
        circles_transform_buffer->length = circles_dynamic_array->length * 16;

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, circlesModelToWorldMatrixBuffer);
        glBufferData(GL_SHADER_STORAGE_BUFFER, circles_transform_buffer->length * sizeof(float), circles_transform_buffer->elements, GL_DYNAMIC_DRAW);  
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, circlesModelToWorldMatrixBuffer);

        glUseProgram(circleShaderHandle);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, circles_quad_buffer->length);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    destroy_dynamic_struct_Circle_array(&circles_dynamic_array);
    destroy_dynamic_float_array(&circles_transform_buffer);
    destroy_dynamic_float_array(&circles_quad_buffer);
    return 0;
}
