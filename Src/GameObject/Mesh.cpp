#include "Mesh.hpp"

namespace GM {

	Mesh& Mesh::operator=(const Mesh& other)
	{
		count = other.count;
		indicesBuffer = other.indicesBuffer;
		verticesBuffer = other.verticesBuffer;
		return *this;
	}
	Mesh::Mesh(const Mesh& other) :
		count(other.count),
		indicesBuffer(other.indicesBuffer),
		verticesBuffer(other.verticesBuffer)
	{
	}
}