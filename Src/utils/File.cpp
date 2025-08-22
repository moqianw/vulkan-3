#include "File.hpp"
#include <vector>
namespace UT {
	FileReader::FileReader()
	{

	}
    std::vector<char> FileReader::read(const std::string& fname) {
        std::ifstream fs(fname, std::ios::binary);
        if (!fs.is_open()) {
            throw std::runtime_error("cannot open file " + fname);
        }

        fs.seekg(0, std::ios::end);
        size_t size = fs.tellg();
        std::vector<char> buffer(size);
        fs.seekg(0, std::ios::beg);
        fs.read(buffer.data(), size);

        return buffer;
    }
	FileReader::~FileReader()
	{
	}
}