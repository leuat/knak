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
  int m_curXpos = 0;
  int m_posx = 0;
  int m_posy = 0;
  bool m_isLocked = false;
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
  virtual void constrainCursor(int diffy = 0);
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
      m_startx = m_posx + m_curXpos;
    }
    else {
      m_endy = getYpos();
      m_endx = m_posx + m_curXpos;
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

  void eraseSelection();
    
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
    m_curXpos = 0;
    constrainCursor();
  }

  void moveCursorDown() {
    m_posy++;
    m_curXpos = 0;
    constrainCursor();
  }

  void pageUp() {
    const int d = (m_height - hasBorders()*2)/2;
    m_posy-=d;
    if (m_posy<0) {
      m_curYpos-=d;
      m_posy+=d;
    }
    constrainCursor();
  }

  void pageDown() {
    const int d = (m_height - hasBorders()*2)/2;
    m_posy+=d;
    if (m_posy>m_height-hasBorders()*2) {
      m_curYpos+=d;
      m_posy-=d;
    }
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

class DocumentListDocument : public Document {
public:
  void constrainCursor(int diffy) override {
    m_posx = 0;
    if (m_posy<0) m_posy = 0;
    if (m_posy>=m_contents.size())
      m_posy = m_contents.size()-1;
  
}

};
