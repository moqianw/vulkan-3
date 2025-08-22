#pragma once
#include <fstream>
#include <string>
#include <vector>
namespace UT {
	class FileReader {
	public:
		FileReader();
		static std::vector<char> read(const std::string& fname);
		~FileReader();
	private:
	protected:

	};
}