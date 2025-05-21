#pragma once

#include <string>
#include <vector>


class Data {
 public:
  std::vector<std::string> keywords = { "for", "while", "include", "static", "case", "const", "switch", "constexpr","free","delete","new","return","define" };
  std::vector<std::string> types = { "uint32_t", "int", "size_t", "uint8_t", "auto","void" };
  static Data d;
};


