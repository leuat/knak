
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

Window mainWindow, *curWindow = nullptr;
bool isDone = false;
bool doRefresh = true;


void initColorValues() {
  init_color(COLOR_GREEN, 0, 750, 250);
  init_color(COLOR_RED, 1000, 300, 500);
  init_color(COLOR_BLUE, 100, 300, 1000);
  init_color(COLOR_BLACK, 30, 50, 100);
  init_color(COLOR_WHITE, 500, 500, 600);
}

void moveCursor(Window* w) {
  w->printCursor();
  // refreshes the screen
  int pposx = w->m_posx;
  int pposy = w->m_posy;
  int v = getch();
  if (v==27) {
    v = getch();
    v = getch();
    if (v == 'B') w->m_posy++;
    if (v == 'A') w->m_posy--;
    if (v == 'C') w->m_posx++;
    if (v == 'D') w->m_posx--;
    if (v == 27) isDone = true;
  }
  if (w->m_posy==w->m_height) {
    w->m_posy -=1;
    w->m_curYpos++;
    doRefresh = true;
  }
  if (w->m_posy<0) {
    w->m_posy +=1;
    if (w->m_curYpos!=0)
      w->m_curYpos--;
    doRefresh = true;
  }

}

void initColors() {
  start_color();
  initColorValues();
  init_pair(1, COLOR_RED, COLOR_BLACK);
  init_pair(2, COLOR_BLUE, COLOR_BLACK);
  init_pair(3, COLOR_WHITE, COLOR_BLACK);
  init_pair(4, COLOR_BLACK, COLOR_RED);
}

void init() {
  initscr();
  mainWindow.init(stdscr);    
  curWindow = &mainWindow;

  if (has_colors() == FALSE) {
    endwin();
    printf("Your terminal does not support color\n");
    exit(1);
  }
  noecho();
  initColors();

}

int main(int argc, char ** argv)
{
  // init screen and sets up screen
  //mvprintw(height-1,width-20,"Hello World");
  curs_set(0);
  init();
  mainWindow.loadFile(argv[1]);
  while (!isDone) {
    if (refresh) {
      clear();
      curWindow->printFile();
      doRefresh = false;
    }
    refresh();
    moveCursor(curWindow);
  }
  endwin();
  return 0;
}



