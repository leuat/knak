#pragma once

#include <string>
#include <vector>
#include <ncurses.h>
#include <filesystem>
#include "ini.h"
#include "util.h"

class Data {
 public:
  std::vector<std::string> keywords = { "for", "while", "include", "static", "case", "const", "switch", "constexpr","free","delete","new","return","define" };
  std::vector<std::string> types = { "uint32_t", "int", "size_t", "uint8_t", "auto","void","bool" };
  static Data d;

  static const int COLOR_TEXT = 1;
  static const int COLOR_KEYWORD = 2;
  static const int COLOR_TYPE = 3;
  static const int COLOR_NUMBER = 4;
  static const int COLOR_STRING = 5;
  static const int COLOR_SYMBOL = 6;
  static const int COLOR_CURSOR = 7;
  static const int COLOR_SELECTION = 8;
  
  inline static const std::string s_tab = "   ";

  static void Error(std::string msg) {
    endwin();
    printf("Fatal error: %s\n",msg.c_str());
    exit(1);
  }

  void setColor(int color, std::string inival) {
    if (inival=="")
      return;
    inival = Util::replaceAll(inival,"#","");
    unsigned int rgb = std::stoul(inival, nullptr, 16);
    init_color(color, ((rgb>>16)/1.02)*4, (((rgb>>8)&0xFF)/1.02)*4, ((rgb&0xFF)/1.02)*4);
    //    Error("11");
  }
  
  void Init(std::string inifile) {
    mINI::INIFile file(inifile);
    mINI::INIStructure ini;
    file.read(ini);
    setColor(COLOR_RED,ini.get("CurrentSettings").get("color_keyword"));
    setColor(COLOR_WHITE,ini.get("CurrentSettings").get("color_text"));
    setColor(COLOR_BLUE,ini.get("CurrentSettings").get("color_type"));
    setColor(COLOR_YELLOW,ini.get("CurrentSettings").get("color_selection"));
    setColor(COLOR_BLACK,ini.get("CurrentSettings").get("color_background"));
 
  }
  
  
};


