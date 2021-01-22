#include "ResizeableTexture.h"

uint8_t* ResizeableTexture::data()
{
	return buffer.data();
}

ResizeableTexture::ResizeableTexture(size_t _width, size_t _height, size_t scale): width(_width), height(_height),
                                                                                   reservedW(_width * scale),
                                                                                   reservedH(_height * scale)
{
	std::fill_n(std::back_inserter(buffer), reservedH * reservedW * 3, 0);
}

ResizeableTexture::ResizeableTexture(uint8_t* image, size_t _width, size_t _height, size_t scale): width(_width), height(_height), reservedW(_width * scale),reservedH(_height * scale)
{
	buffer = std::vector<uint8_t>(image, image + width * height * 3);
	buffer.resize(reservedH * reservedW * 3, 0);
}

void ResizeableTexture::reset(float val)
{
	std::fill(buffer.begin(), buffer.end(), val);
}

void ResizeableTexture::resize(size_t newW, size_t newH)
{
	if (newH > reservedH || newW > reservedW || newH < 100 || newW < 100)
		return;

	width = newW;
	height = newH;
}

Vector3 ResizeableTexture::get(size_t x, size_t y) const
{
	const size_t id = y * width * 3 + x * 3;
	return {static_cast<float>(buffer[id]), static_cast<float>(buffer[id + 1]), static_cast<float>(buffer[id + 2])};
}

void ResizeableTexture::write(const Vector3& color, size_t x, size_t y)
{
	const size_t id = y * width * 3 + x * 3;
	buffer[id + 0] = static_cast<uint8_t>(color.x);
	buffer[id + 1] = static_cast<uint8_t>(color.y);
	buffer[id + 2] = static_cast<uint8_t>(color.z);
}
