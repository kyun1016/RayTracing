#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

namespace kyun
{
	// Bilinear interpolation reference
	// https://www.scratchapixel.com/lessons/mathematics-physics-for-computer-graphics/interpolation/bilinear-filtering

	using namespace glm;

	class Texture
	{
	public:
		Texture(const std::string &filename);
		Texture(const int &width, const int &height, const std::vector<vec3> &pixels);

		vec3 GetWrapped(int i, int j);
		vec3 InterpolateBilinear(const float &dx, const float &dy, const vec3 &c00, const vec3 &c10, const vec3 &c01, const vec3& c11);
		vec3 SamplePoint(const vec2& uv);
		vec3 SampleLinear(const vec2& uv);

	private:
		int mWidth, mHeight, mChannels;
		std::vector<uint8_t> mImage;
	};
}