#include "Square.h"

namespace kyun {
    Square::Square(vec3 v0, vec3 v1, vec3 v2, vec3 v3, vec2 uv0, vec2 uv1, vec2 uv2, vec2 uv3)
        : mTriangle1(v0, v1, v2, uv0, uv1, uv2)
        , mTriangle2(v0, v2, v3, uv0, uv2, uv3)
    { }

    Hit Square::CheckRayCollision(const Ray& ray) const
    {
        auto hit1 = mTriangle1.CheckRayCollision(ray);
        auto hit2 = mTriangle2.CheckRayCollision(ray);

        if (hit1.d >= 0.0f && hit2.d >= 0.0f)
        {
            return hit1.d < hit2.d ? hit1 : hit2;
        }
        else if (hit1.d >= 0.0f)
        {
            return hit1;
        }
        else
        {
            return hit2;
        }
    }
}

