#include "Core/Time.h"
#include <GLFW/glfw3.h>

float Time::s_DeltaTime = 0.0f;
float Time::s_LastFrameTime = 0.0f;
float Time::s_TimeSinceStart = 0.0f;

void Time::Init() {
    s_LastFrameTime = (float)glfwGetTime();
}

void Time::Update() {
    float currentTime = (float)glfwGetTime();
    s_DeltaTime = currentTime - s_LastFrameTime;
    s_LastFrameTime = currentTime;
    s_TimeSinceStart = currentTime;
}

float Time::DeltaTime() {
    return s_DeltaTime;
}

float Time::TimeSinceStart() {
    return s_TimeSinceStart;
}
