#include "Sphere.h"

kyun::Sphere::Sphere(const vec3& mCenter, const float mRadius, const vec3& color)
    : mCenter(mCenter), mRadius(mRadius), Object(color)
{
}

// Wikipedia Line–sphere intersection
// https://en.wikipedia.org/wiki/Line%E2%80%93sphere_intersection
kyun::Hit kyun::Sphere::CheckRayCollision(const Ray& ray) const
{
    Hit hit = Hit{ -1.0f, vec3(0.0f), vec3(0.0f) };

    const float b = 2.0f * glm::dot(ray.dir, ray.start - mCenter);
    const float c = dot(ray.start - mCenter, ray.start - mCenter) - mRadius * mRadius;

    const float det = b * b - 4.0f * c;
    if (det >= 0.0f)
    {
        const float d1 = (-b - sqrt(det)) / 2.0f;
        const float d2 = (-b + sqrt(det)) / 2.0f;
        hit.d = glm::min(d1, d2);

        // 물체 안에서 다시 밖으로 나가면서 충돌 가능
        if (hit.d < 0.0f)
            hit.d = glm::max(d1, d2);

        hit.mPoint = ray.start + ray.dir * hit.d;
        hit.mNormal = glm::normalize(hit.mPoint - mCenter);
    }

    return hit;
}
