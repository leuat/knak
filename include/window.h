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
#include "util.h"
#include <filesystem>

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
  bool m_hasBorders = false;
  int m_starty=-1, m_endy=-1, m_startx=-1, m_endx=-1;

  std::vector<shared_ptr<Window>> m_children;
  Window* m_parent = nullptr;
  bool m_isSelecting = false;

  std::string m_currentFile = "";

  enum WindowType {
    Editor,
    FileList,
    TopInfo,
    Linenumbers,
    Empty
  };

  void key(int k);

  WindowType m_type = Empty;

  void constrainCursor(int diffy = 0);

  void unselect() { m_isSelecting = false; }
  
  void select() {
    if (m_isSelecting == false) {
      m_isSelecting = true;
      m_starty = getYpos();
    }
    else
      m_endy = getYpos();

  }
  
  void fillLines(int start) {
    m_contents.clear();
    for (int i=0;i<m_height;i++) {
      std::string s = "  " + std::to_string(i+start) + ":";
      m_contents.push_back(s);
    }
  }

  int getFirstCharPos() {
    int x = 0;
    auto s = getCurrentLine();
    while (x<s.size() && s[x]==' ')
      x++;
    return x;
    
  }
  
  void Init(WINDOW* w, WindowType type) {
    m_window = w;
    getmaxyx(m_window,m_height,m_width);
    //    printf("%i %i \n",m_width, m_height);
    m_type = type;

    
  }

  std::string getCurrentLine() {
    int p = getYpos();
    if (p<m_contents.size() && p>=0)
      return m_contents[p];
    return "";
  }
  
  shared_ptr<Window> addChild(WindowType type, float px, float py, float pw, float ph);

  uint8_t getColorType(std::string s);
  
  void loadFile(std::string fn);

  void loadDir(std::string dn);

  
  void printLine(std::string f);

  void printFile();
  
  void printSelection();

  void print();

  void printCursor();

  void refresh();

  bool hasBorders() {
      bool hasB = m_hasBorders;
      if (m_parent!=nullptr)
	hasB |= m_parent->hasBorders();
      return hasB;

  }

  int getYpos() {
    int p = m_curYpos + m_posy - hasBorders();
    if (p<0) p=0;
    return p;
  }

  void moveCursorDown() {
    m_posy++;
    constrainCursor();
  }
  void moveCursorRight() {
    m_posx++;
    constrainCursor(1);
  }
  void moveCursorLeft() {
    m_posx--;
    constrainCursor(-1);
  }

  void moveCursorUp() {
    m_posy--;
    constrainCursor();
  }
    

};
