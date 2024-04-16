#pragma once
#include "Object.h"

namespace kyun
{
    class Sphere : public Object
    {
    public:
        Sphere(const vec3& mCenter, const float mRadius, const vec3& color = vec3(1.0f));
        virtual ~Sphere() = default;
        Hit CheckRayCollision(const Ray& ray) const;
    private:
        vec3 mCenter;
        float mRadius;
    };
}
