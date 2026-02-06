#pragma once

class Time {
public:
    static void Init();
    static void Update();

    static float DeltaTime();
    static float TimeSinceStart();

private:
    static float s_DeltaTime;
    static float s_LastFrameTime;
    static float s_TimeSinceStart;
};
