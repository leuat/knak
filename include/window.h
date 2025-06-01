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
#include "document.h"
using namespace std;



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
    if (type==Windows) {
      m_doc = make_shared<DocumentListDocument>();
    }
    else
      m_doc = make_shared<Document>();


    if (m_doc == nullptr)
      exit(1);
    
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

  void printWindowList(bool showSelection);

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
