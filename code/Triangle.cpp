#include "Triangle.h"

namespace kyun
{
    Triangle::Triangle()
        : mV0(vec3(0.0f)), mV1(vec3(0.0f)), mV2(vec3(0.0f)), mUV0(vec2(0.0f)), mUV1(vec2(0.0f)), mUV2(vec2(0.0f))
    {
    }

    Triangle::Triangle(vec3 v0, vec3 v1, vec3 v2, vec2 mUV0, vec2 mUV1, vec2 mUV2)
        : mV0(v0), mV1(v1), mV2(v2), mUV0(mUV0), mUV1(mUV1), mUV2(mUV2)
    {
    }

    Hit Triangle::CheckRayCollision(const Ray& ray) const
    {
        return IntersectRayTriangle(ray);
    }

    Hit Triangle::IntersectRayTriangle(const Ray& ray) const
    {
        //주의: 삼각형의 넓이가 0일 경우에는 계산할 수 없음
        Hit hit = Hit{ -1.0, vec3(0.0), vec3(0.0) };

        hit.mNormal= glm::normalize(glm::cross(mV1 - mV0, mV2 - mV0));
        hit.d = (dot(mV0, hit.mNormal) - dot(ray.start, hit.mNormal)) / (dot(ray.dir, hit.mNormal));
        
        // 삼각형 뒷면을 그리고 싶지 않은 경우 (Backface culling)
        if (dot(-ray.dir, hit.mNormal) < 0.0f)
            return Hit{ -1.0, vec3(0.0), vec3(0.0) };

        // 평면과 광선이 수평에 매우 가깝다면 충돌하지 못하는 것으로 판단
        if (glm::abs(dot(ray.dir, hit.mNormal)) < 1e-2f)
            return Hit{ -1.0, vec3(0.0), vec3(0.0) }; // t 계산시 0으로 나누기 방지

        // 광선의 시작점 이전에 충돌한다면 렌더링할 필요 없음
        if (hit.d < 0.0f)
            return hit;

        hit.mPoint = ray.start + hit.d * ray.dir; // 충돌점

        /* 3. 그 충돌 위치가 삼각형 안에 들어 있는 지 확인 */
        //const vec3 cross0 = glm::cross(hit.mPoint - mV2, mV1 - mV2); // 2, 1, point
        //const vec3 cross1 = glm::cross(hit.mPoint - mV0, mV2 - mV0); // 0, 2, point
        //const vec3 cross2 = glm::cross(mV1 - mV0, hit.mPoint - mV0); // 1, 0, point
        const vec3 cross0 = glm::cross(mV1 - mV0, hit.mPoint - mV0); // 1, 0, point
        const vec3 cross1 = glm::cross(mV2 - mV1, hit.mPoint - mV1); // 2, 1, point
        const vec3 cross2 = glm::cross(mV0 - mV2, hit.mPoint - mV2); // 0, 2, point

        if (dot(cross0, hit.mNormal) < 0.0f)
            return Hit{ -1.0, vec3(0.0), vec3(0.0) };
        if (dot(cross1, hit.mNormal) < 0.0f)
            return Hit{ -1.0, vec3(0.0), vec3(0.0) };
        if (dot(cross2, hit.mNormal) < 0.0f)
            return Hit{ -1.0, vec3(0.0), vec3(0.0) };

        // Barycentric coordinates 계산
        const float area0 = glm::length(cross0) * 0.5f;
        const float area1 = glm::length(cross1) * 0.5f;
        const float area2 = glm::length(cross2) * 0.5f;

        const float areaSum = area0 + area1 + area2;

        const float w0 = area1 / areaSum;
        const float w1 = area2 / areaSum;

        hit.mUV = mUV0 * w0 + mUV1 * w1 + mUV2 * (1.0f - w0 - w1); // 텍스쳐 좌표

        return hit;
    }

    // 참고: https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/ray-triangle-intersection-geometric-solution
}


