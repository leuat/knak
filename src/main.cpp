#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include "window.h"

using namespace std;
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

void initColorValues() {
  init_color(COLOR_GREEN, 0, 750, 250);
  init_color(COLOR_RED, 0, 1000, 500);
  init_color(COLOR_BLUE, 100, 300, 1000);
  init_color(COLOR_BLACK, 30, 50, 100);
  init_color(COLOR_WHITE, 500, 500, 600);
}

bool setDocument(std::string fn) {
  for (auto& d : documents) {
    if (d->m_currentFile == fn) {
      editorWindow->m_doc = d;
      return true;
    }
      
  }
  return false;
    
}

void newDocument(std::string fn) {
  
}


void loadDocument(std::string fn) {
  if (setDocument(fn))
    return;
  auto doc = make_shared<Document>();
  doc->loadFile(fn);
  windowWindow->m_doc->m_contents.push_back(fn);
  editorWindow->m_doc = doc;
  documents.push_back(doc);
  
}

int moveCursor(Window* w) {
  // refreshes the screen
  
  int pposx = w->m_doc->m_posx;
  int pposy = w->m_doc->m_posy;
  //  nonl();
  cbreak();
  raw();
  //    keypad(stdscr, TRUE);
  int v = getch();
           printf("key: %i     \n",v);
  if (v==ctrl('q'))
    isDone = true;
  if (v==ctrl('c')) {
    w->m_doc->copySelection();
    return -1;
  }
  if (v==ctrl(44)) {
    isDone = true;
  }
  if (v==ctrl('v')) {
    w->m_doc->snap();
    w->m_doc->pasteSelection();
    return -1;
  }
  if (v==ctrl('z')) {
    w->m_doc->undo();
    return -1;
  }

  
  if (v==10) { // enter
	       // Load file
    if (curWindow->m_type==Window::FileList) {
      if (Util::isDirectory(fileWindow->m_doc->getCurrentLine()))
	fileWindow->m_doc->loadDir(fileWindow->m_doc->getCurrentLine());
      else
	loadDocument(fileWindow->m_doc->getCurrentLine());
      return -1;
    }
    if (curWindow->m_type==Window::Windows) {
      setDocument(windowWindow->m_doc->m_contents[windowWindow->m_doc->m_posy]);
      return -1;
    }

  }

  if (v==59) { // shift + cursor keys select
      // shift
    auto v1 = getch();
    auto v2 = getch();
    if (v1==50) // shift 
      w->m_doc->moveCursor(v2,true);
    if (v1==53) {// ctrl
      windowWindow->m_doc->moveCursor(v2,true);
      setDocument(windowWindow->m_doc->m_contents[windowWindow->m_doc->m_posy]);
      
    }
    return -1;
    }

  if (v==27) {
    int v1 = getch();
    int v2 = getch();
    //    printf("key: %i %i %i    \n",v,v1,v2);
    //             printf("key27: %i\n",v);
    if (v2==90)  { //Shift+TAB
      curTab = (curTab+1)%tabOrder.size();
      curWindow = tabOrder[curTab];
      return -1;
    }
    if (v2 == 54) { // page down
      w->m_doc->pageDown();
      v1 = getch();
      return -1;
    }
    if (v2 == 53) { // page up
      w->m_doc->pageUp();
      v1 = getch();
      return -1;
    }
    w->m_doc->moveCursor(v2,false);
    if (v2==27) {
      getch();
      w->m_doc->clearSelection();
      return -1;
      }
    //    if (v == 27) isDone = true;
    curWindow->m_doc->constrainCursor();
    return -1;
  }
  //  printf("keyball: %i",v);
  
  w->m_doc->clearSelection();
  curWindow->key(v);
  
  return v;

}

void initColors() {
  start_color();
  initColorValues();
  init_pair(Data::COLOR_KEYWORD, COLOR_RED, COLOR_BLACK);
  init_pair(Data::COLOR_TYPE, COLOR_BLUE, COLOR_BLACK);
  init_pair(Data::COLOR_TEXT, COLOR_WHITE, COLOR_BLACK);
  init_pair(Data::COLOR_CURSOR, COLOR_BLACK, COLOR_RED);
  init_pair(Data::COLOR_SYMBOL, COLOR_GREEN, COLOR_BLACK);
  init_pair(Data::COLOR_SELECTION, COLOR_BLACK, COLOR_YELLOW);
}



void init() {
  initscr();
  initColors();
  mainWindow.Init(stdscr, Window::Empty);

  float fileSplit = 0.2;
  float mainSplitY = 0.1;
  float lineSplit = 0.07;
  
  auto mainw = mainWindow.addChild(Window::Empty, fileSplit, mainSplitY, 1-fileSplit, 1-mainSplitY*2);
  fileWindow = mainWindow.addChild(Window::FileList, 0, mainSplitY, fileSplit, 1-mainSplitY);
  fileWindow->m_hasBorders = true;
  windowWindow = mainWindow.addChild(Window::Windows, fileSplit, 0, 1.0-fileSplit, mainSplitY);
  windowWindow->m_hasBorders = true;
  
  mainw->m_hasBorders = true;
  
  lineWindow = mainw->addChild(Window::Linenumbers, 0.0, 0.0, lineSplit, 1.0);
  editorWindow = mainw->addChild(Window::Editor, lineSplit, 0.0,1-lineSplit, 1.0);
  curWindow = editorWindow;
  
  if (has_colors() == FALSE) {
    endwin();
    printf("Your terminal does not support color\n");
    exit(1);
  }
  noecho();
  curs_set(0);
  Data::d.Init("knak.ini");
  tabOrder.push_back(editorWindow);
  tabOrder.push_back(fileWindow);
  tabOrder.push_back(windowWindow);

  build = make_shared<Document>();
  build->m_doc->m_currentFile = "[build]";
  m_documents.append(build);
  
    

}

int main(int argc, char ** argv)
{
  init();
  loadDocument(argv[1]);
  fileWindow->m_doc->loadDir(".");
  
  while (!isDone) {
    windowWindow->m_doc->m_currentFile = editorWindow->m_doc->m_currentFile;
    if (true) {
      lineWindow->m_doc->fillLines(editorWindow->m_doc->m_curYpos);
      mainWindow.print();
      doRefresh = false;
    }
    curWindow->printCursor();
    //    refresh();
    mainWindow.refresh();
    
    auto key = moveCursor(curWindow.get());
  }
  endwin();
  return 0;
}



