#define STB_IMAGE_IMPLEMENTATION
#define GLM_ENABLE_EXPERIMENTAL
#include "stb_image.h"
#include "Texture.h"

namespace kyun
{
	Texture::Texture(const std::string &filename)
	{
		unsigned char *img = stbi_load(filename.c_str(), &mWidth, &mHeight, &mChannels, 0);

		mImage.resize(mWidth * mHeight * mChannels);
		memcpy(mImage.data(), img, mImage.size() * sizeof(uint8_t));

		delete img;
	}

	Texture::Texture(const int &width, const int &height, const std::vector<vec3> &pixels)
		: mWidth(width), mHeight(height), mChannels(3)
	{
		mImage.resize(mWidth * mHeight * mChannels);

		for (int j = 0; j < height; j++)
			for (int i = 0; i < width; i++)
			{
				const auto &color = pixels[i + j * width];

				mImage[(i + mWidth * j) * mChannels + 0] = uint8_t(color.r * 255);
				mImage[(i + mWidth * j) * mChannels + 1] = uint8_t(color.g * 255);
				mImage[(i + mWidth * j) * mChannels + 2] = uint8_t(color.b * 255);
			}
	}
	vec3 Texture::GetWrapped(int i, int j)
	{
		i %= mWidth; // i가 width면 0으로 바뀜
		j %= mHeight;

		if (i < 0)
			i += mWidth; // i가 -1이면 (width-1)로 바뀜
		if (j < 0)
			j += mHeight;

		const float r = mImage[(i + mWidth * j) * mChannels + 0] / 255.0f;
		const float g = mImage[(i + mWidth * j) * mChannels + 1] / 255.0f;
		const float b = mImage[(i + mWidth * j) * mChannels + 2] / 255.0f;

		return vec3(r, g, b);
	}
	vec3 Texture::InterpolateBilinear(const float& dx, const float& dy, const vec3& c00, const vec3& c10, const vec3& c01, const vec3& c11)
	{
		vec3 a = c00 * (1.0f - dx) + c10 * dx;
		vec3 b = c01 * (1.0f - dx) + c11 * dx;
		return a * (1.0f - dy) + b * dy;
	}
	vec3 Texture::SamplePoint(const vec2& uv) // Nearest sampling이라고 부르기도 함
	{
		// 텍스춰 좌표의 범위 mUV [0.0, 1.0] x [0.0, 1.0]
		// 이미지 좌표의 범위 xy [-0.5, width - 1 + 0.5] x [-0.5, height - 1 + 0.5]
		vec2 xy = uv * vec2(mWidth, mHeight) - vec2(0.5f);
		xy += vec2(0.5f);

		int i = int(xy.x);
		int j = int(xy.y);

		return GetWrapped(i, j);
	}
	vec3 Texture::SampleLinear(const vec2& uv)
	{
		// 텍스춰 좌표의 범위 mUV [0.0, 1.0] x [0.0, 1.0]
		// 이미지 좌표의 범위 xy [-0.5, width - 1 + 0.5] x [-0.5, height - 1 + 0.5]
		// std::cout << floor(-0.3f) << " " << int(-0.3f) << std::endl; // -1 0

		const vec2 xy = uv * vec2(mWidth, mHeight) - vec2(0.5f);
		const int i = int(floor(xy.x));
		const int j = int(floor(xy.y));
		const float dx = xy.x - float(i);
		const float dy = xy.y - float(j);

		return InterpolateBilinear(dx, dy, GetWrapped(i, j), GetWrapped(i + 1, j), GetWrapped(i, j + 1), GetWrapped(i + 1, j + 1));
	}
}