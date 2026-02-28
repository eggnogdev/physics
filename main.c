#include <alloca.h>
#include "linalg.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "glad/glad.h"
#include <GLFW/glfw3.h>

struct Position {
    float       x;
    float       y;
};

struct Velocity {
    float       x;
    float       y;
};

struct Circle {
    float               radius;
    float               mass;
    struct Velocity     velocity;
    struct Position     position;
};

struct CircleBufferData {
    float       quad[18];
};

struct CircleBufferData circleToBufferData(struct Circle c) {
    struct CircleBufferData bd = {
        .quad = {
            -c.radius, -c.radius, 0.0f,
            -c.radius,  c.radius, 0.0f,
             c.radius,  c.radius, 0.0f,
            -c.radius, -c.radius, 0.0f,
             c.radius,  c.radius, 0.0f,
             c.radius, -c.radius, 0.0f
        }
    };
    
    return bd;
}

void circleToWorldTransform(struct Circle c, mat4 dest) {
    mat4_identity(dest);
    mat4_translate(dest, (vec3){c.position.x, c.position.y, 0.0f});
}

char* readFileToString(const char* file) {
    char* buffer = NULL;
    uint64_t length = 0;
    FILE* filePointer = fopen(file, "rb");

    if (filePointer == NULL) {
        fprintf(stderr, "Failed to open file: %s\n", file);
    } else {
        fseek(filePointer, 0, SEEK_END);
        length = ftell(filePointer);
        fseek(filePointer, 0, SEEK_SET);
        buffer = malloc(length + 1);
        if (buffer == NULL) {
            fprintf(stderr, "Failed to malloc %d bytes\n", length + 1);
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
}

void glfwProcessInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

int main() {
    struct Circle circle = {
        .radius = 0.5f,
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
    char circleWorldBasedModelTransformUniformName[] = "worldBasedModelTransform";
    uint32_t circleWorldBasedModelTransformUL = glGetUniformLocation(circleShaderHandle, circleWorldBasedModelTransformUniformName);

    mat4 viewMatrix;
    /* vec3 cameraEye = { 0.0f, 0.0f, 5.0f }; */
    /* vec3 center = { 0.0f, 0.0f, 0.0f }; */
    /* vec3 up = { 0.0f, 1.0f, 0.0f }; */
    mat4_identity(viewMatrix);
    /* mat4_lookat(viewMatrix, cameraEye, center, up); */
    mat4_translate(viewMatrix, (vec3){0.0f, 0.0f, -5.0f});

    char viewMatrixUniformName[] = "viewMatrix";
    uint32_t viewMatrixUL = glGetUniformLocation(circleShaderHandle, viewMatrixUniformName);
    glUniformMatrix4fv(viewMatrixUL, 1, GL_TRUE, (float *)viewMatrix);

    float lastFrameTime = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        float thisFrameTime = glfwGetTime();
        float deltaTime = thisFrameTime - lastFrameTime;
        lastFrameTime = thisFrameTime;

        glfwProcessInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        applyGravity(deltaTime, &circle);

        glUseProgram(circleShaderHandle);
        glBindVertexArray(VAO);
        struct CircleBufferData circleBd = circleToBufferData(circle);
        mat4 circleTransform;
        circleToWorldTransform(circle, circleTransform);
        glBufferData(GL_ARRAY_BUFFER, sizeof(struct CircleBufferData), &circleBd, GL_STATIC_DRAW);
        glUniformMatrix4fv(circleWorldBasedModelTransformUL, 1, GL_TRUE, (float *)circleTransform);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
