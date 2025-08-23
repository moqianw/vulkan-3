#include "Buffer.hpp"
namespace UT {
	Buffer& Buffer::operator=(const Buffer& other) {
		memory = other.memory;
		size = other.size;
		offset = other.offset;
		buffer = other.buffer;
		ptr = other.ptr;
	}
	Buffer::Buffer(const Buffer& other) :
		memory(other.memory),
		size(other.size),
		offset(other.offset),
		buffer(other.buffer),
		ptr(other.ptr)
	{
	}
}