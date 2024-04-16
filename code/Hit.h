#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

namespace kyun
{
    class Object;

    struct Hit
    {
    public:
        float d;     // 광선의 시작부터 충돌 지점까지의 거리
        glm::vec3 mPoint;  // 충돌한 위치
        glm::vec3 mNormal; // 충돌한 위치에서 표면의 수직 벡터
        glm::vec2 mUV;     // 텍스쳐 좌표

        std::shared_ptr<Object> mObj; // 나중에 물체의 재질 등을 가져오기 위한 포인터
    };
}