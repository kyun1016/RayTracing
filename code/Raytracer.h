#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include "Ray.h"
#include "Light.h"
#include "Sphere.h"
#include "Triangle.h"
#include "Square.h"

#include <vector>

namespace kyun
{
	class Raytracer
	{
		friend class Renderer;
	public:
		std::vector<std::shared_ptr<Object>> mObjects;
		int mWidth, mHeight;
		Light mLight;

		Raytracer(const int& width, const int& height);

		void Render(std::vector<glm::vec4>& pixels) const;
		glm::vec3 TransformScreenToWorld(vec2 posScreen) const;
		Hit FindClosestCollision(Ray& ray) const;
		glm::vec3 traceRay(Ray& ray, const int recurseLevel) const;
		
		Ray FindReflectionRay(const Ray& ray, const Hit& hit) const;
		Ray FindRefractionRay(const Ray& ray, const Hit& hit) const;
		float FindDiffuse(const Hit& hit) const;
		float FindSpecular(const Ray& ray, const Hit& hit) const;
	};
}