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
        //����: �ﰢ���� ���̰� 0�� ��쿡�� ����� �� ����
        Hit hit = Hit{ -1.0, vec3(0.0), vec3(0.0) };

        hit.mNormal= glm::normalize(glm::cross(mV1 - mV0, mV2 - mV0));
        hit.d = (dot(mV0, hit.mNormal) - dot(ray.start, hit.mNormal)) / (dot(ray.dir, hit.mNormal));
        
        // �ﰢ�� �޸��� �׸��� ���� ���� ��� (Backface culling)
        if (dot(-ray.dir, hit.mNormal) < 0.0f)
            return Hit{ -1.0, vec3(0.0), vec3(0.0) };

        // ���� ������ ���� �ſ� �����ٸ� �浹���� ���ϴ� ������ �Ǵ�
        if (glm::abs(dot(ray.dir, hit.mNormal)) < 1e-2f)
            return Hit{ -1.0, vec3(0.0), vec3(0.0) }; // t ���� 0���� ������ ����

        // ������ ������ ������ �浹�Ѵٸ� �������� �ʿ� ����
        if (hit.d < 0.0f)
            return hit;

        hit.mPoint = ray.start + hit.d * ray.dir; // �浹��

        /* 3. �� �浹 ��ġ�� �ﰢ�� �ȿ� ��� �ִ� �� Ȯ�� */
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

        // Barycentric coordinates ���
        const float area0 = glm::length(cross0) * 0.5f;
        const float area1 = glm::length(cross1) * 0.5f;
        const float area2 = glm::length(cross2) * 0.5f;

        const float areaSum = area0 + area1 + area2;

        const float w0 = area1 / areaSum;
        const float w1 = area2 / areaSum;

        hit.mUV = mUV0 * w0 + mUV1 * w1 + mUV2 * (1.0f - w0 - w1); // �ؽ��� ��ǥ

        return hit;
    }

    // ����: https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/ray-triangle-intersection-geometric-solution
}


