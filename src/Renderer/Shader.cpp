#include "Renderer/Shader.h"
#include "Core/Log.h"
#include <glad/glad.h>

static unsigned int CreateShader(const char* vs, const char* fs) {
    unsigned int program = glCreateProgram();
    unsigned int vert = glCreateShader(GL_VERTEX_SHADER);
    unsigned int frag = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vert, 1, &vs, nullptr);
    glCompileShader(vert);

    int success;
    glGetShaderiv(vert, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info[512];
        glGetShaderInfoLog(vert, 512, nullptr, info);
        Log::Error(info);
    }

    glShaderSource(frag, 1, &fs, nullptr);
    glCompileShader(frag);

    glGetShaderiv(frag, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info[512];
        glGetShaderInfoLog(frag, 512, nullptr, info);
        Log::Error(info);
    }

    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);

    glDeleteShader(vert);
    glDeleteShader(frag);

    return program;
}

Shader::Shader(const char* vertexSrc, const char* fragmentSrc) {
    m_RendererID = CreateShader(vertexSrc, fragmentSrc);
}

Shader::~Shader() {
    glDeleteProgram(m_RendererID);
}

void Shader::Bind() const {
    glUseProgram(m_RendererID);
}

void Shader::Unbind() const {
    glUseProgram(0);
}
