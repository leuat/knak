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

Window mainWindow;
shared_ptr<Window> curWindow = nullptr, lineWindow = nullptr, fileWindow = nullptr, editorWindow = nullptr, windowWindow = nullptr;
bool isDone = false;
bool doRefresh = true;
#define ctrl(x)           ((x) & 0x1f)


void initColorValues() {
  init_color(COLOR_GREEN, 0, 750, 250);
  init_color(COLOR_RED, 0, 1000, 500);
  init_color(COLOR_BLUE, 100, 300, 1000);
  init_color(COLOR_BLACK, 30, 50, 100);
  init_color(COLOR_WHITE, 500, 500, 600);
}

int moveCursor(Window* w) {
  // refreshes the screen
  int pposx = w->m_posx;
  int pposy = w->m_posy;
  //nonl();
  cbreak();
  raw();
  //    keypad(stdscr, TRUE);
    int v = getch();
    //printf("keyballeeeee: %i",v);
  if (v==ctrl('q'))
    isDone = true;
  if (v==ctrl('c')) {
    w->copySelection();
    return -1;
  }
  if (v==ctrl('v')) {
    w->snap();
    w->pasteSelection();
    return -1;
  }
  if (v==ctrl('z')) {
    w->undo();
    return -1;
  }
   
  if (v==10) { // enter
	       // Load file
    if (curWindow->m_type==Window::FileList) {
      if (Util::isDirectory(fileWindow->getCurrentLine()))
	fileWindow->loadDir(fileWindow->getCurrentLine());
      else
	editorWindow->loadFile(fileWindow->getCurrentLine());
      return -1;
    }

  }

  if (v==59) { // shift + cursor keys select
      // shift
    v = getch();
    v = getch();
    w->moveCursor(v,true);
    return -1;
    }

  if (v==27) {
    v = getch();
    v = getch();
    //             printf("key27: %i\n",v);
    if (v==90)  { //Shift+TAB
      if (curWindow == editorWindow)
	curWindow = fileWindow;
      else
	curWindow = editorWindow;
      return -1;
    }
    w->moveCursor(v,false);
    if (v==27) {
      getch();
      w->clearSelection();
      return -1;
      }
    //    if (v == 27) isDone = true;
    curWindow->constrainCursor();
    return -1;
  }
  //  printf("keyball: %i",v);
  
  w->clearSelection();
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
  windowWindow = mainWindow.addChild(Window::Windows, fileSplit, 0, 1, mainSpltY);
  
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
    

}

int main(int argc, char ** argv)
{
  init();
  editorWindow->loadFile(argv[1]);
  windowWindow->m_contents.append(std::string(argv[1]);
  fileWindow->loadDir(".");
  
  while (!isDone) {
    
    if (true) {
      lineWindow->fillLines(editorWindow->m_curYpos);
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



