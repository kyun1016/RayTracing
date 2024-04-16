#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Hit.h"
#include "Ray.h"
#include "Texture.h"

namespace kyun
{
    class Object
    {
        friend class Raytracer;
    public:
        Object(const vec3& color = { 1.0f, 1.0f, 1.0f });
        Object(const vec3& color, const vec3& ambient, const vec3& diffuse, const vec3& specular, const float& alpha, const float& reflection, const float& transparency);
        virtual ~Object() = default;
        virtual Hit CheckRayCollision(const Ray &ray) const = 0;

        void SetAmbient(const vec3& ambient);
        void SetDiffuse(const vec3& diffuse);
        void SetSpecular(const vec3& specular);
        void SetAlpha(const float& alpha);
        void SetReflection(const float& reflection);
        void SetTransparency(const float& transparency);
        void SetRefractive(const float& refractive);
        void SetAmbTexture(std::shared_ptr<Texture> ambTexture);
        void SetDifTexture(std::shared_ptr<Texture> difTexture);
    private:
        // Material
        vec3 mAmb = vec3(0.0f);     // Ambient
        vec3 mDif = vec3(0.0f);     // Diffuse
        vec3 mSpec = vec3(0.0f);    // Specular
        float mAlpha = 1.0f;
        float mReflection = 0.0;    // 반사
        float mTransparency = 0.0;  // 투명성
        float mRefractive = 1.0;    // 물체 굴절률
        
        std::shared_ptr<Texture> mAmbTexture;
        std::shared_ptr<Texture> mDifTexture;
        std::shared_ptr<Texture> mSpeTexture;
    };
}