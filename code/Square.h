#pragma once

#include "Object.h"
#include "Triangle.h"

namespace kyun
{
    class Square : public Object
    {
    public:
        Square(vec3 v0, vec3 v1, vec3 v2, vec3 v3, vec2 uv0 = vec2(0.0f, 0.0f), vec2 uv1 = vec2(1.0f, 0.0f), vec2 uv2 = vec2(1.0f, 1.0f), vec2 uv3 = vec2(0.0f, 1.0f));
        virtual ~Square() = default;
        virtual Hit CheckRayCollision(const Ray& ray) const;
    private:
        Triangle mTriangle1, mTriangle2;
    };
}