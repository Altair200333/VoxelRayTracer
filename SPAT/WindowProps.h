#pragma once
#include <string>
#include <utility>

class WindowProps final
{
public:
	std::string name;
	size_t width;
	size_t height;
	WindowProps(std::string windowName, size_t w = 1600, size_t h= 900): name(std::move(windowName)), width(w), height(h)
	{}
};
