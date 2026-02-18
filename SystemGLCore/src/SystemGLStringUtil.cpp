#include "pch.h"
#include "SystemGLStringUtil.h"

std::string trim(std::string s) {
	s.erase(s.begin(), find_if(s.begin(), s.end(), [](unsigned char c) { return !isspace(c); }));
	s.erase(find_if(s.rbegin(), s.rend(), [](unsigned char c) { return !isspace(c); }).base(), s.end());
	return s;
}
std::string extract_between(const std::string& s, const std::string& open, const std::string& close) {
	size_t a = s.find(open);
	if (a == std::string::npos) return "";
	a += open.size();
	size_t b = s.find(close, a);
	if (b == std::string::npos) return "";
	return s.substr(a, b - a);
}