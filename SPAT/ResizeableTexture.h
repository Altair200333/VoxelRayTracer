#pragma once
#include <cstdint>
#include <vector>
#include "Vector3.h"

class ResizeableTexture final
{
	std::vector<uint8_t> buffer;
	size_t reservedW;
	size_t reservedH;

public:
	size_t width;
	size_t height;

	[[nodiscard]] uint8_t* data();

	ResizeableTexture(size_t _width, size_t _height, size_t scale = 3);

	ResizeableTexture(uint8_t* image, size_t _width, size_t _height, size_t scale = 3);

	void reset(float val = 255.0f * 0.1f);

	void resize(size_t newW, size_t newH);

	[[nodiscard]] Vector3 get(size_t x, size_t y) const;

	void write(const Vector3& color, size_t x, size_t y);
};
