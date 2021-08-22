#ifndef _UTIL_H
#define _UTIL_H

#include <iostream>
#include <cstring>

#define DEBUG 1

#ifdef DEBUG
#  define LOG(...) show(__LINE__, #__VA_ARGS__, __VA_ARGS__)
#  define LOGS(...) shows(__LINE__, __VA_ARGS__)
#else
#  define LOG(...)
#  define LOGS(...)
#endif

template<typename H1>
std::ostream& show(std::ostream& out, const char* label, H1&& value) {
  return out << label << ": " << std::forward<H1>(value) << '\n';
}

template<typename H1, typename ...T>
std::ostream& show(std::ostream& out, const char* label, H1&& value, T&&... rest) {
  const char* pcomma = strchr(label, ',');
  return show(out.write(label, pcomma - label) << ": "
                                               << std::forward<H1>(value)
                                               << ',',
              pcomma + 1,
              std::forward<T>(rest)...);
}

template<typename ...T>
std::ostream& show(const int line, const char* label, T&&... rest) {
  std::cout << "[L" << line << "] ";
  return show(std::cout, label, rest...);
}

std::ostream& shows(const int line, const std::string&& s);

bool equals(float a, float b, float eps = 1e-6);

#endif

