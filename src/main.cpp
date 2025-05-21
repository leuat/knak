
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


void initColors() {
  init_color(COLOR_GREEN, 0, 750, 250);
  init_color(COLOR_RED, 1000, 300, 500);
  init_color(COLOR_BLUE, 100, 300, 1000);
  init_color(COLOR_BLACK, 30, 50, 100);
  init_color(COLOR_WHITE, 500, 500, 600);
}


int main(int argc, char ** argv)
{
    // init screen and sets up screen
    initscr();
    mainWindow.init(stdscr);    

    if (has_colors() == FALSE) {
      endwin();
      printf("Your terminal does not support color\n");
      exit(1);
    }
    mainWindow.loadFile(argv[1]);
    
    start_color();
    initColors();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_BLUE, COLOR_BLACK);
    init_pair(3, COLOR_WHITE, COLOR_BLACK);
    init_pair(4, COLOR_BLACK, COLOR_RED);
    noecho();
    //mvprintw(height-1,width-20,"Hello World");
    bool isDone = false;
    int v;
    bool doRefresh = true;
    curs_set(0);
    while (!isDone) {
      if (refresh) {
	clear();
	mainWindow.printFile();
	doRefresh = false;
      }
      move(mainWindow.m_posy,mainWindow.m_posx);
      attron(COLOR_PAIR(4));
      char c = ' ';
      if (mainWindow.m_posx<mainWindow.m_contents[mainWindow.m_curYpos+mainWindow.m_posy].size())
	c = mainWindow.m_contents[mainWindow.m_curYpos+mainWindow.m_posy][mainWindow.m_posx];
      printw("%c",c);
      refresh();
     
    // refreshes the screen
      int pposx = mainWindow.m_posx;
      int pposy = mainWindow.m_posy;
      v = getch();
      if (v==27) {
	v = getch();
	v = getch();
	if (v == 'B') mainWindow.m_posy++;
	if (v == 'A') mainWindow.m_posy--;
	if (v == 'C') mainWindow.m_posx++;
	if (v == 'D') mainWindow.m_posx--;
	if (v == 27) isDone = true;
      }
      if (mainWindow.m_posy==mainWindow.m_height) {
	mainWindow.m_posy -=1;
	mainWindow.m_curYpos++;
	doRefresh = true;
      }
      if (mainWindow.m_posy<0) {
	mainWindow.m_posy +=1;
	if (mainWindow.m_curYpos!=0)
	  mainWindow.m_curYpos--;
	doRefresh = true;
      }
      //      mvcur(pposy,pposx,posy,posx);
      //      clrtoeol();      //      if (v==KEY_ENTER) isDone = true;
    }
    // pause the screen output

    // deallocates memory and ends ncurses
    endwin();
    printf("\n%d\n",v);
    return 0;
}



