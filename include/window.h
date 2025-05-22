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

  std::vector<shared_ptr<Window>> m_children;
  Window* m_parent = nullptr;

  enum WindowType {
    Editor,
    Filelist,
    TopInfo,
    Empty
  };

  WindowType m_type = Empty;
  
  void Init(WINDOW* w, WindowType type) {
    m_window = w;
    getmaxyx(m_window,m_height,m_width);
    printf("%i %i \n",m_width, m_height);
    m_type = type;
    
  }

  shared_ptr<Window> addChild(WindowType type, float px, float py, float pw, float ph) {
    int h = ph*m_height;
    int w = pw*m_width;
    int sx = px*m_width;
    int sy = py*m_height;

    WINDOW* nw = newwin(h,w,sy,sx);
    shared_ptr<Window> win = make_shared<Window>();
    win->Init(nw, type);
    win->m_parent = this;
    m_children.push_back(win);
    return win;
  }

  uint8_t getColorType(std::string s);
  
  string replaceAll(string str, const string &from, const string &to);

  void loadFile(std::string fn);

  void printLine(std::string f);

  void printFile();

  void print();

  void printCursor();

  void refresh() {
    wrefresh(m_window);
    for (auto& c: m_children) {
      c->refresh();
    }
  }

};
