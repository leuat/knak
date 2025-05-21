#pragma once

#include "data.h"
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

using namespace std;

class Window {
 public:
  WINDOW* m_window = nullptr;
  int m_width = -1;
  int m_height = 0;
  int m_curYpos = 0;
  int m_posx = 0;
  int m_posy = 0;
  std::vector<string> m_contents;

  
  void init(WINDOW* w) {
    m_window = w;
    getmaxyx(stdscr,m_height,m_width);
    
  }

  uint8_t getColorType(std::string s);
  
  string replaceAll(string str, const string &from, const string &to);

  void loadFile(std::string fn);

  void printLine(std::string f);

  void printFile();

  void printCursor();

};
