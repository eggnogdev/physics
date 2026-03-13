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

static float aspectRatio = 1.0f;

struct CircleBufferData {
    float       quad[18];
};

struct CircleBufferData circleToBufferData(struct Circle c) {
    float z = 0.0f;
    struct CircleBufferData bd = {
        .quad = {
            -c.radius, -c.radius, z,
            -c.radius,  c.radius, z,
             c.radius,  c.radius, z,
            -c.radius, -c.radius, z,
             c.radius,  c.radius, z,
             c.radius, -c.radius, z
        }
    };
    
    return bd;
}

void circleToWorldTransform(struct Circle c, mat4 dest) {
    mat4_identity(dest);
    mat4_translation(dest, (vec3){c.position.x, c.position.y, 0.0f});
}

char* readFileToString(const char* file) {
    char* buffer = NULL;
    int64_t length = 0;
    FILE* filePointer = fopen(file, "rb");

    if (filePointer == NULL) {
        fprintf(stderr, "Failed to open file: %s\n", file);
    } else {
        fseek(filePointer, 0, SEEK_END);
        length = ftell(filePointer);
        fseek(filePointer, 0, SEEK_SET);
        buffer = malloc(length + 1);
        if (buffer == NULL) {
            fprintf(stderr, "Failed to malloc %ld bytes\n", length + 1);
            return NULL;
        } else {
            // TODO: error handle?
            fread(buffer, 1, length, filePointer);
        }
    }

    fclose(filePointer);
    return buffer;
}

uint32_t compileShader(uint32_t type, const char* source) {
    uint32_t handle = glCreateShader(type);
    glShaderSource(handle, 1, &source, NULL);
    glCompileShader(handle);

    int result;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &length);

        // dynamically allocate onto the stack.
        char* message = (char*)alloca(length * sizeof(char));

        glGetShaderInfoLog(handle, length, &length, message);
        char typeAsString[10];
        switch (type) {
            case GL_VERTEX_SHADER:
                strncpy(typeAsString, "vertex", sizeof(typeAsString));
                break;
            case GL_FRAGMENT_SHADER:
                strncpy(typeAsString, "fragment", sizeof(typeAsString));
                break;
            default:
                strncpy(typeAsString, "unknown", sizeof(typeAsString));
                break;
        }

        fprintf(stderr, "Failed to compile %s shader: %s\n", typeAsString, message);
        glDeleteShader(handle);
        return 0;
    }

    return handle;
}

uint32_t createShader(const char* vertexShader, const char* fragmentShader) {
    uint32_t programHandle = glCreateProgram();
    uint32_t vertexShaderHandle = compileShader(GL_VERTEX_SHADER, vertexShader);
    uint32_t fragmentShaderHandle = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(programHandle, vertexShaderHandle);
    glAttachShader(programHandle, fragmentShaderHandle);
    glLinkProgram(programHandle);
    glValidateProgram(programHandle);

    // can delete shader after it has been linked
    glDeleteShader(vertexShaderHandle);
    glDeleteShader(fragmentShaderHandle);

    return programHandle;
}

uint32_t createShaderFromFile(const char* vertexShaderFile, const char* fragmentShaderFile) {
    const char* vertexShader = readFileToString(vertexShaderFile);
    const char* fragmentShader = readFileToString(fragmentShaderFile);

    return createShader(vertexShader, fragmentShader);
}

void applyGravity(float timestep, struct Circle* circle) {
    circle->velocity.y -= (9.815f * timestep);
    circle->position.y += (circle->velocity.y * timestep);
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

    struct Circle circle = {
        .radius = 0.125f,
        .mass = 5.0f,
        .velocity = {
            .x = 0.0f,
            .y = 5.0f,
        },
        .position = {
            .x = 0.0f,
            .y = 0.0f,
        }
    };

    glfwSetErrorCallback(glfwErrorCallback);
    glfwInitHint(GLFW_PLATFORM, GLFW_ANY_PLATFORM);
    if (!glfwInit()) {
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);
    GLFWwindow* window = glfwCreateWindow(640, 480, "Physics", NULL, NULL);
    if (window == NULL) {
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwTerminate();
        return 1;
    }

    glViewport(0, 0, 640, 480);
    glfwSetFramebufferSizeCallback(window, glfwFramebufferSizeCallback);

    uint32_t circleShaderHandle = createShaderFromFile("circle.vert.glsl", "circle.frag.glsl");

    uint32_t VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    uint32_t circlesVertexPositionVBO;
    glGenBuffers(1, &circlesVertexPositionVBO);
    glBindBuffer(GL_ARRAY_BUFFER, circlesVertexPositionVBO);

    // specify the screenBasedVertexPosition attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
    glEnableVertexAttribArray(0);

    glUseProgram(circleShaderHandle);
    char modelToWorldMatrixUniformName[] = "modelToWorldMatrix";
    uint32_t modelToWorldMatrixUL = glGetUniformLocation(circleShaderHandle, modelToWorldMatrixUniformName);

    mat4 worldToCameraMatrix;
    vec3 eye = { 0.0f, 0.0f, 2.0f };
    vec3 center = { 0.0f, 0.0f, 0.0f };
    vec3 up = { 0.0f, 1.0f, 0.0f };
    mat4_lookat(worldToCameraMatrix, eye, center, up);

    char worldToCameraMatrixUniformName[] = "worldToCameraMatrix";
    uint32_t worldToCameraMatrixUL = glGetUniformLocation(circleShaderHandle, worldToCameraMatrixUniformName);
    glUniformMatrix4fv(worldToCameraMatrixUL, 1, GL_TRUE, (float*)worldToCameraMatrix);

    mat4 cameraToScreenMatrix;
    mat4_perspective(cameraToScreenMatrix, 60.0f, 640.0f / 480.0f, 0.1f, 100.0f);

    char cameraToScreenMatrixUniformName[] = "cameraToScreenMatrix";
    uint32_t cameraToScreenMatrixUL = glGetUniformLocation(circleShaderHandle, cameraToScreenMatrixUniformName);
    glUniformMatrix4fv(cameraToScreenMatrixUL, 1, GL_TRUE, (float*)cameraToScreenMatrix);
    
    uint32_t radiusUL = glGetUniformLocation(circleShaderHandle, "modelRadius");
    glUniform1f(radiusUL, circle.radius);

    float lastFrameTime = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        float thisFrameTime = glfwGetTime();
        float deltaTime = thisFrameTime - lastFrameTime;
        lastFrameTime = thisFrameTime;

        glfwProcessInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        applyGravity(deltaTime, &circle);
        // TODO: apply gravity to all circles in circles_dynamic_array
        // TODO: display all circles in circles_dynamic_array

        glUseProgram(circleShaderHandle);
        glBindVertexArray(VAO);
        struct CircleBufferData circleBd = circleToBufferData(circle);
        mat4 circleTransform;
        circleToWorldTransform(circle, circleTransform);
        glBufferData(GL_ARRAY_BUFFER, sizeof(struct CircleBufferData), &circleBd, GL_STATIC_DRAW);
        glUniformMatrix4fv(modelToWorldMatrixUL, 1, GL_TRUE, (float*)circleTransform);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    destroy_dynamic_struct_Circle_array(&circles_dynamic_array);
    return 0;
}
