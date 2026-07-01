#pragma once

class [[nodiscard]] Random final
{
public:
    static float randomFloat(float min, float max, float scale = 100.f)
    {
        return (rand() % static_cast<int>((max - min) * scale)) / scale + min;
    }
};
