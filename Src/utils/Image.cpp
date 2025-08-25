#include "Image.hpp"

namespace UT {
	Image::Image(const Image& other) {
		image = other.image;
		imageview = other.imageview;
		memory = other.memory;
		size = other.size;
		offset = other.offset;
	}
	Image& Image::operator=(const Image& other) {
		image = other.image;
		imageview = other.imageview;
		memory = other.memory;
		size = other.size;
		offset = other.offset;
		return *this;
	}
}