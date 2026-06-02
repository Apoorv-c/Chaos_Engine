#pragma once

#include <string>
#include <glad/glad.h>

class Texture2D
{
public:
    Texture2D(const std::string& path);
    ~Texture2D();

    void Bind(unsigned int slot = 0) const;

    unsigned int GetID() const { return m_ID; }

private:
    unsigned int m_ID;
};