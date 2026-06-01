#pragma once
#include <string>
#include <glm/glm.hpp>

class Shader {
public:
    Shader(const char* vertexSrc, const char* fragmentSrc);
    ~Shader();

    void Bind() const;
    void Unbind() const;
    void SetMat4(const std::string& name, const glm::mat4& matrix);
    void SetInt(const std::string& name, int value);
    void SetFloat4(const std::string& name, const glm::vec4& value);

private:
    unsigned int m_RendererID;

    unsigned int CompileShader(unsigned int type, const char* source);
};

