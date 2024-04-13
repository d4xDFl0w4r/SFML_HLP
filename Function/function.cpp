#include "function.hpp"

Function::Function() {
    this->Set(0.f, 0.f, 0.f);
}

Function::Function(float a, float f, float d) {
    this->Set(a, f, d);
}

float Function::operator()(float x) const {
    return A * sin(f * x - d);
}

void Function::Set(float a, float f, float d) {
    this->A = a;
    this->f = f;
    this->d = d;
}