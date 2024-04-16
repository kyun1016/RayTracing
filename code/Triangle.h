#pragma once

#include "Object.h"

namespace kyun
{
    class Triangle : public Object
    {
    public:
        Triangle();
        Triangle(vec3 v0, vec3 v1, vec3 v2, vec2 mUV0 = vec2(0.0f), vec2 mUV1 = vec2(0.0f), vec2 mUV2 = vec2(0.0f));
        virtual ~Triangle() = default;
        virtual Hit CheckRayCollision(const Ray& ray) const;
        Hit IntersectRayTriangle(const Ray& ray) const;
    private:
        vec3 mV0, mV1, mV2;
        vec2 mUV0, mUV1, mUV2; // 뒤에서 텍스쳐 좌표계로 사용
    };
} // namespace kyun