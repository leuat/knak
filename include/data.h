#pragma once

#include <string>
#include <vector>
#include <ncurses.h>
#include <filesystem>

class Data {
 public:
  std::vector<std::string> keywords = { "for", "while", "include", "static", "case", "const", "switch", "constexpr","free","delete","new","return","define" };
  std::vector<std::string> types = { "uint32_t", "int", "size_t", "uint8_t", "auto","void","bool" };
  static Data d;

  static void Error(std::string msg) {
    endwin();
    printf("Fatal error: %s\n",msg.c_str());
    exit(1);

  }
  
};


