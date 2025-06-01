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


class Snap {
public:
  std::vector<std::string> m_contents;
  int m_posx, m_posy, m_curYpos;
  Snap(std::vector<std::string>& contents, int posx, int posy, int curYpos):m_contents(contents), m_posx(posx), m_posy(posy), m_curYpos(curYpos) {
  }
  
};


class Document {
public:
  int m_width = -1;
  int m_height = 0;
  static const int MAX_UNDO = 1000;
  std::vector<Snap> m_snaps;
  int m_curYpos = 0;
  int m_posx = 0;
  int m_posy = 0;
  int m_starty=-1, m_endy=-1, m_startx=-1, m_endx=-1;
  std::vector<string> m_contents, m_selection;
  bool m_isSelecting = false;

  std::string m_currentFile = "";
  
  bool m_hasBorders = 0;

  bool hasBorders() {
    return m_hasBorders;
  }
  

  void key(int k);
  void copySelection();
  void pasteSelection();
  void snap();
  void undo();
  void constrainCursor(int diffy = 0);
  void loadFile(std::string fn);

  void loadDir(std::string dn);

  void unselect() { m_isSelecting = false; }

  void fillLines(int start) {
      m_contents.clear();
    for (int i=0;i<m_height;i++) {
      std::string s = "  " + std::to_string(i+start) + ":";
      m_contents.push_back(s);
    }
    }
  
  void select() {
    if (m_isSelecting == false) {
      m_isSelecting = true;
      m_starty = getYpos();
      m_startx = m_posx;
    }
    else {
      m_endy = getYpos();
      m_endx = m_posx;
    }

  }
  int getFirstCharPos() {
    int x = 0;
    auto s = getCurrentLine();
    while (x<s.size() && s[x]==' ')
      x++;
    return x;
    
  }
  std::string getCurrentLine() {
    int p = getYpos();
    if (p<m_contents.size() && p>=0)
      return m_contents[p];
    return "";
  }
  int getYpos() {
    int p = m_curYpos + m_posy - hasBorders();
    if (p<0) p=0;
    return p;
  }
  void clearSelection() {
    m_starty = -1;
    m_endy = -1;
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
  void moveCursor(int v, bool is_select) {
      if (is_select) {
	if (v == 'B') { select(); moveCursorDown();select();}
	if (v == 'A') { select(); moveCursorUp();select(); }
	if (v == 'C') { select(); moveCursorRight();select();}
	if (v == 'D') { select(); moveCursorLeft();select();}
      }
      else {
	if (v == 'B') { unselect(); moveCursorDown(); }
	if (v == 'A') { unselect(); moveCursorUp();}
	if (v == 'C') { unselect(); moveCursorRight();}
	if (v == 'D') { unselect(); moveCursorLeft();}

      }
  }

};

class Window {
 public:
  WINDOW* m_window = nullptr;
  int m_width = -1;
  int m_height = 0;
  bool m_hasBorders = false;
  shared_ptr<Document> m_doc;

  std::vector<shared_ptr<Window>> m_children;
  Window* m_parent = nullptr;



  enum WindowType {
    Editor,
    FileList,
    TopInfo,
    Linenumbers,
    Windows,
    Empty
    
  };


  WindowType m_type = Empty;
  
  void key(int k);

  
  void Init(WINDOW* w, WindowType type) {
    m_window = w;
    getmaxyx(m_window,m_height,m_width);
    m_doc = make_shared<Document>();
    m_doc->m_height = m_height;
    m_doc->m_width = m_width;
    //    printf("%i %i \n",m_width, m_height);
    m_type = type;

    
  }

  
  shared_ptr<Window> addChild(WindowType type, float px, float py, float pw, float ph);

  uint8_t getColorType(std::string s);
  

  
  void printLine(std::string f);

  void printFile();
  
  void printSelection();

  void printWindowList();

  void print();

  void printCursor();

  void refresh();

  bool hasBorders() {
      bool hasB = m_hasBorders;
      if (m_parent!=nullptr)
	hasB |= m_parent->hasBorders();
      return hasB;

  }




};
