#ifndef STRINGFILEUTILS_H
#define STRINGFILEUTILS_H

#include <string>
#include <vector>

std::vector<std::string> splitString(const std::string& input, const std::string& delimiters);
std::vector<std::string> readFile(const std::string& filename);

#endif
