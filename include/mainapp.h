#pragma once
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include "window.h"
#include <unistd.h>
#include <chrono>
#include <thread>

using namespace std;

class MainApp {
 public:
  
#define ctrl(x)           ((x) & 0x1f)
  
  Window mainWindow;
  shared_ptr<Window> curWindow = nullptr, lineWindow = nullptr, fileWindow = nullptr, windowWindow = nullptr, editorWindow = nullptr;
  std::vector<shared_ptr<Window>> docs;
  bool isDone = false;
  bool doRefresh = true;
  shared_ptr<Document> build = nullptr;
  
  std::vector<std::shared_ptr<Window>> tabOrder;
  std::vector<std::shared_ptr<Document>> documents;
  
  int curTab = 0;


  void initColorValues();
  bool setDocument(std::string fn);
  void loadDocument(std::string fn);
  void buildProject();
  void blink();
  int moveCursor(Window* w);
  void initColors();
  void init();
  void update();
  





};

