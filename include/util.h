#pragma once

#include <string>
#include <filesystem>

using namespace std;

class Util {
 public:
  static string replaceAll(string str, const string &from, const string &to);
  static bool isDirectory(std::string dn) {
    return std::filesystem::is_directory(dn);
  }
 
};
