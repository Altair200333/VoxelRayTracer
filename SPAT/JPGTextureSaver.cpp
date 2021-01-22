#include "JPGTextureSaver.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


void JPGTextureSaver::save(ResizeableTexture& texture, const std::string& path)
{
	stbi_write_jpg(path.c_str(), texture.width, texture.height, 3, texture.data(), 100);
}
