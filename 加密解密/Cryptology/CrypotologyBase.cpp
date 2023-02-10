#include "Cryptology.h"

Cryptology_::ULL Cryptology_::getLen(std::istream& fin) {
	auto gp = fin.tellg();
	fin.seekg(0, std::ios_base::end);
	Cryptology_::ULL ttsize = fin.tellg() - gp;
	fin.seekg(gp, std::ios_base::beg);
	return ttsize;
}
char* Cryptology_::sstrToCstr(std::ostringstream& osstr) {
	std::string os = osstr.str();
	char* ret = new char[os.size() + 1];
	strcpy_s(ret, os.size() + 1, os.c_str());
	return ret;
}

bool Cryptology_::fileDelete(const char* const file) {
	std::ofstream os;
	os.open(file, std::ios_base::trunc);
	if (!os.is_open()) return false;
	os.close();
	system((std::string("del ") + file).c_str());
	return true;
}