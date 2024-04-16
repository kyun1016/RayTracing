#include "Raytracer.h"

namespace kyun
{
	Raytracer::Raytracer(const int& width, const int& height)
		: mWidth(width), mHeight(height)
	{
		mLight = Light{ { 0.0f, 0.3f, -0.5f } }; // 화면 뒷쪽
	}


	void Raytracer::Render(std::vector<glm::vec4>& pixels) const
	{
		const glm::vec3 eyePos(0.0f, 0.0f, -1.5f);

#pragma omp parallel for
		for (int j = 0; j < mHeight; j++)
			for (int i = 0; i < mWidth; i++)
			{
				const glm::vec3 pixelPosWorld = TransformScreenToWorld(vec2(i, j));
				Ray pixelRay{ pixelPosWorld, glm::normalize(pixelPosWorld - eyePos) };
				pixels[i + mWidth * j] = vec4(glm::clamp(traceRay(pixelRay, 5), 0.0f, 1.0f), 1.0f);
			}
	}

	// x : [0, mWidth) -> [-1.0f, 1.0f) * aspect
	// y : [0, mHeight) -> [-1.0f, 1.0f)
	// z: 0.0f
	glm::vec3 Raytracer::TransformScreenToWorld(vec2 posScreen) const
	{
		const float xScale = 2.0f / mWidth;
		const float yScale = 2.0f / mHeight;
		const float aspect = float(mWidth) / mHeight;

		// 3차원 공간으로 확장 (z좌표는 0.0)
		return glm::vec3((posScreen.x * xScale - 1.0f) * aspect, -posScreen.y * yScale + 1.0f, 0.0f);
	}


	Hit Raytracer::FindClosestCollision(Ray& ray) const
	{
		float closestD = 1000.0; // inf
		Hit closestHit = Hit{ -1000.0, dvec3(0.0), dvec3(0.0) };

		for (int l = 0; l < mObjects.size(); l++)
		{
			auto hit = mObjects[l]->CheckRayCollision(ray);

			if (hit.d >= 0.0f)
			{
				if (hit.d < closestD)
				{
					closestD = hit.d;
					closestHit = hit;
					closestHit.mObj = mObjects[l];

					// 텍스쳐 좌표
					closestHit.mUV = hit.mUV;
				}
			}
		}

		return closestHit;
	}

	// 광선이 물체에 닿으면 그 물체의 색 반환
	glm::vec3 Raytracer::traceRay(Ray& ray, const int recurseLevel) const
	{
		if (recurseLevel < 0)
			return glm::vec3(0.0f);

		// Render first hit
		const auto hit = FindClosestCollision(ray);
		if (hit.d >= 0.0f)
		{
			glm::vec3 color(0.0f);

			const float totalCoef = hit.mObj->mReflection + hit.mObj->mTransparency;
			float reflection = hit.mObj->mReflection;
			float transparency = hit.mObj->mTransparency;
			if (totalCoef > 1.0f)
			{
				reflection /= totalCoef;
				transparency /= totalCoef;
			}
			
			glm::vec3 phongColor(0.0f);
			// Ambient
			phongColor += hit.mObj->mAmbTexture
				? hit.mObj->mAmb * hit.mObj->mAmbTexture->SampleLinear(hit.mUV)
				: hit.mObj->mAmb;

			// Shadow
			const glm::vec3 dirToLight = glm::normalize(mLight.mPos - hit.mPoint);
			Ray shadowRay = { hit.mPoint + dirToLight * 1e-4f, dirToLight };
			if (FindClosestCollision(shadowRay).d < 0.0f)
			{
				// Diffuse (그림자가 없는 경우)
				const float diff = FindDiffuse(hit);
				phongColor += hit.mObj->mDifTexture
					? diff * hit.mObj->mDif * hit.mObj->mDifTexture->SampleLinear(hit.mUV)
					: diff * hit.mObj->mDif;

				// Specular (그림자가 없는 경우)
				const float specular = FindSpecular(ray, hit);
				phongColor += hit.mObj->mSpeTexture
					? specular * hit.mObj->mSpec * hit.mObj->mSpeTexture->SampleLinear(hit.mUV)
					: specular * hit.mObj->mSpec;
			}

			// Reflection
			if (hit.mObj->mReflection)
			{
				Ray reflectionRay = FindReflectionRay(ray, hit);
				color += traceRay(reflectionRay, recurseLevel - 1) * reflection;
			}

			// Transparency
			if (hit.mObj->mTransparency)
			{
				Ray refractionRay = FindRefractionRay(ray, hit);
				color += traceRay(refractionRay, recurseLevel - 1) * transparency;
			}

			return color + phongColor * glm::clamp(1.0f - totalCoef, 0.0f, 1.0f);
		}

		return glm::vec3(0.0f);
	}

	Ray Raytracer::FindReflectionRay(const Ray& ray, const Hit& hit) const
	{
		const auto reflectedDirection = glm::normalize(2.0f * hit.mNormal * dot(-ray.dir, hit.mNormal) + ray.dir);
		return Ray{ hit.mPoint + reflectedDirection * 1e-4f, reflectedDirection };
	}

	Ray Raytracer::FindRefractionRay(const Ray& ray, const Hit& hit) const
	{
		const float ior = 1.5f; // Index of refraction (유리: 1.5, 물: 1.3)

		float eta; // sinTheta1 / sinTheta2
		glm::vec3 normal;

		if (glm::dot(ray.dir, hit.mNormal) < 0.0f) // 밖에서 안에서 들어가는 경우 (예: 공기->유리)
		{
			eta = ior;
			normal = hit.mNormal;
		}
		else // 안에서 밖으로 나가는 경우 (예: 유리->공기)
		{
			eta = 1.0f / ior;
			normal = -hit.mNormal;
		}

		const float cosTheta1 = glm::dot(normal, -ray.dir);
		const float sinTheta1 = sqrt(1.0f - cosTheta1 * cosTheta1); // cos^2 + sin^2 = 1
		const float sinTheta2 = sinTheta1 / eta;
		const float cosTheta2 = sqrt(1.0f - sinTheta2 * sinTheta2);

		const glm::vec3 m = glm::normalize(dot(normal, -ray.dir) * normal + ray.dir);
		const glm::vec3 a = m * sinTheta2;
		const glm::vec3 b = -normal * cosTheta2;
		const glm::vec3 refractedDirection = glm::normalize(a + b); // transmission

		return Ray{ hit.mPoint + refractedDirection * 1e-4f, refractedDirection };
	}

	float Raytracer::FindDiffuse(const Hit& hit) const
	{
		const glm::vec3 dirToLight = glm::normalize(mLight.mPos - hit.mPoint);
		return glm::max(glm::dot(hit.mNormal, dirToLight), 0.0f);
	}

	float Raytracer::FindSpecular(const Ray& ray, const Hit& hit) const
	{
		const glm::vec3 dirToLight = glm::normalize(mLight.mPos - hit.mPoint);
		const glm::vec3 reflectDir = hit.mNormal * 2.0f * glm::dot(dirToLight, hit.mNormal) - dirToLight;
		return glm::pow(glm::max(glm::dot(-ray.dir, reflectDir), 0.0f), hit.mObj->mAlpha);
	}

}
