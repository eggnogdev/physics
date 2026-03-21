#ifndef __PHYSICS_SHADER_H
#define __PHYSICS_SHADER_H

char* read_file_to_char_buffer(const char* file) {
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

uint32_t compile_shader(uint32_t type, const char* source) {
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

uint32_t create_shader_program(const char* vertex_shader, const char* fragment_shader) {
    uint32_t program_handle = glCreateProgram();
    uint32_t vertex_shader_handle = compile_shader(GL_VERTEX_SHADER, vertex_shader);
    uint32_t fragment_shader_handle = compile_shader(GL_FRAGMENT_SHADER, fragment_shader);

    glAttachShader(program_handle, vertex_shader_handle);
    glAttachShader(program_handle, fragment_shader_handle);
    glLinkProgram(program_handle);
    glValidateProgram(program_handle);

    // can delete shader after it has been linked
    glDeleteShader(vertex_shader_handle);
    glDeleteShader(fragment_shader_handle);

    return program_handle;
}

uint32_t create_shader_program_from_file(const char* vertex_shader_file, const char* fragment_shader_file) {
    char* vertex_shader = read_file_to_char_buffer(vertex_shader_file);
    char* fragment_shader = read_file_to_char_buffer(fragment_shader_file);

    uint32_t program_handle = create_shader_program(vertex_shader, fragment_shader);

    free(vertex_shader);
    free(fragment_shader);

    return program_handle;
}

#endif // __PHYSICS_SHADER_H
