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
shared_ptr<Window> curWindow = nullptr, lineWindow = nullptr, fileWindow = nullptr, editorWindow = nullptr;
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
  if (v==ctrl('s'))
    exit(1);

  
  if (v==27) {
    v = getch();
    v = getch();
    //          printf("key: %i\n",v);
    if (v==90)  { //Shift+TAB
      if (curWindow == editorWindow)
	curWindow = fileWindow;
      else
	curWindow = editorWindow;
      return -1;
    }
    if (v == 'B') w->m_posy++;
    if (v == 'A') w->m_posy--;
    if (v == 'C') w->m_posx++;
    if (v == 'D') w->m_posx--;
    if (v == 27) isDone = true;
    curWindow->constrainCursor();
    return -1;
  }
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



