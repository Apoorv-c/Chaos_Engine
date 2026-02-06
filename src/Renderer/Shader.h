#pragma once
#include <string>

class Shader {
public:
    Shader(const char* vertexSrc, const char* fragmentSrc);
    ~Shader();

    void Bind() const;
    void Unbind() const;

private:
    unsigned int m_RendererID;

    unsigned int CompileShader(unsigned int type, const char* source);
};
