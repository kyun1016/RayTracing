#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
namespace kyun
{
    struct Ray
    {
    public:
        glm::vec3 start; // start position of the ray
        glm::vec3 dir;   // direction of the ray
    };
}