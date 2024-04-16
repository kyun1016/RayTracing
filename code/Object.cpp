#include "Object.h"

kyun::Object::Object(const vec3& color)
    : mAmb(color), mDif(color), mSpec(color)
{
}

kyun::Object::Object(const vec3& color, const vec3& ambient, const vec3& diffuse, const vec3& specular, const float& alpha, const float& reflection, const float& transparency)
    : mAmb(color), mDif(color), mSpec(color)
{
}

void kyun::Object::SetAmbient(const vec3& ambient)
{
    mAmb = glm::clamp(ambient, 0.0f, 1.0f);
}

void kyun::Object::SetDiffuse(const vec3& diffuse)
{
    mDif = glm::clamp(diffuse, 0.0f, 1.0f);
}

void kyun::Object::SetSpecular(const vec3& specular)
{
    mSpec = glm::clamp(specular, 0.0f, 1.0f);
}

void kyun::Object::SetAlpha(const float& alpha)
{
    mAlpha = glm::clamp(alpha, 0.0f, 1.0f);
}

void kyun::Object::SetReflection(const float& reflection)
{
    mReflection = glm::clamp(reflection, 0.0f, 1.0f);
}

void kyun::Object::SetTransparency(const float& transparency)
{
    mTransparency = glm::clamp(transparency, 0.0f, 1.0f);
}

void kyun::Object::SetRefractive(const float& refractive)
{
    mRefractive = refractive;
}

void kyun::Object::SetAmbTexture(std::shared_ptr<Texture> ambTexture)
{
    mAmbTexture = ambTexture;
}

void kyun::Object::SetDifTexture(std::shared_ptr<Texture> difTexture)
{
    mDifTexture = difTexture;
}
