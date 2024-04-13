#pragma once

#include <cmath>

class Function {
public:
    Function();
    Function(float a, float f, float d);

    float operator()(float x) const;
    void Set(float a, float f, float d);

private:
    float A;
    float f;
    float d;
};