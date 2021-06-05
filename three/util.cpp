#include "util.h"

std::ostream& shows(const int line, const std::string&& s) {
    return std::cout << "[L" << line << "] " << s << std::endl;
}

bool equals(float a, float b, float eps) {
    return abs(a - b) < eps;
}
