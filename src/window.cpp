#include "window.h"
#include <filesystem>
#include <algorithm>

void Window::printCursor() {
  if (!m_doc)
    return;
  
  if (m_type!=Editor && m_type!=FileList && m_type!=Windows)
    return;
  
  wattron(m_window,COLOR_PAIR(Data::COLOR_CURSOR));
  int posy = m_doc->getYpos();
  if (m_type==FileList) {
    // display entire line
    wmove(m_window, m_doc->m_posy, hasBorders());
    if (posy<m_doc->m_contents.size())
      wprintw(m_window,m_doc->m_contents[posy].c_str());
    
    wattron(m_window,COLOR_PAIR(Data::COLOR_TEXT));
    return;
  }
  if (m_type==Windows) {

    printWindowList(true);
    wattron(m_window,COLOR_PAIR(Data::COLOR_TEXT));
    return;
  }
  
  wmove(m_window, m_doc->m_posy, m_doc->m_posx+hasBorders());
  char c = ' ';
  if (posy<m_doc->m_contents.size() && m_doc->m_posx<m_doc->m_contents[posy].size())
    c = m_doc->m_contents[posy][m_doc->m_posx+m_doc->m_curXpos];
  
  wprintw(m_window,"%c",c);
  wattron(m_window,COLOR_PAIR(Data::COLOR_TEXT));
}


void Window::refresh() {
  wbkgd(m_window, COLOR_PAIR(Data::COLOR_TEXT));
  wattron(m_window,COLOR_PAIR(Data::COLOR_TEXT));
  if (m_hasBorders)
    box(m_window,0,0);
  
  wrefresh(m_window);
  for (auto& c: m_children) {
    c->refresh();
  }
}


shared_ptr<Window> Window::addChild(WindowType type, float px, float py, float pw, float ph) {
  int h = ph*m_height;
  int w = pw*m_width;
  int sx = px*m_width;
  int sy = py*m_height;
  
  WINDOW* nw = derwin(m_window,h,w,sy,sx);
  shared_ptr<Window> win = make_shared<Window>();
  win->Init(nw, type);
  win->m_parent = this;
  m_children.push_back(win);
  return win;
}




void Window::key(int k) {
  if (m_type!=Editor)
    return;
  if (!m_doc)
    return;
  
  if (m_doc->getYpos()>=m_doc->m_contents.size())
    return;

  m_doc->snap();
  m_doc->key(k);
  
}



uint8_t Window::getColorType(std::string s) {
  s = Util::replaceAll(s, "*","");
  s = Util::replaceAll(s, "#","");
  s = Util::replaceAll(s, "[]","");
  if (std::find(Data::d.keywords.begin(), Data::d.keywords.end(), s) != Data::d.keywords.end()) {
    return Data::COLOR_KEYWORD;
  }
  if (std::find(Data::d.types.begin(), Data::d.types.end(), s) != Data::d.types.end()) {
    return Data::COLOR_TYPE;
  }

  return Data::COLOR_TEXT;
}


void Window::printLine(std::string f) {
  if (m_type==FileList) {
    if (Util::isDirectory(f))
      wattron(m_window,COLOR_PAIR(Data::COLOR_KEYWORD));
    else
      wattron(m_window,COLOR_PAIR(Data::COLOR_TEXT));
    wprintw(m_window, f.c_str());
    return;
  }
  if (m_type==Windows) {
    wprintw(m_window, f.c_str());
    return;
  }
  
  std::string s;
  if (m_doc->m_curXpos<f.size())
    f = f.substr(m_doc->m_curXpos, std::min((int)m_width-hasBorders()*2, (int)f.size()));
  else
    f = "";
  auto is = istringstream(f);
  while (getline(is, s, ' ')) {
    wattron(m_window,COLOR_PAIR(getColorType(s)));
    wprintw(m_window, s.c_str());
    wprintw(m_window," ");
  }
}

void Window::printFile() {
  if (!m_doc)
    return;
  werase(m_window);
  int x = hasBorders();
  int y = hasBorders();
  int posy = m_doc->m_curYpos;
  auto f = m_doc->m_contents;
  while (y<m_height-hasBorders() && posy<f.size()) {
    wmove(m_window,y,x);
    if (posy>=0 && posy<f.size())
      printLine(f[posy]);
    posy++;
    y++;
  }
}

void Window::printWindowList(bool showSelection) {
  werase(m_window);
  int x = hasBorders();
  int y = hasBorders();
  int posy = 0;
  auto f = m_doc->m_contents;
  while (x<m_width-hasBorders() && posy<f.size()) {
    wattron(m_window,COLOR_PAIR(Data::COLOR_TEXT));
    if (f[posy] == m_doc->m_currentFile)
      wattron(m_window,COLOR_PAIR(Data::COLOR_KEYWORD));
    
    if (showSelection && posy == m_doc->m_posy) {
      wattron(m_window,COLOR_PAIR(Data::COLOR_CURSOR));
    }
    
    wmove(m_window,y,x);
    printLine(f[posy]);
    posy++;
    x+=f[posy].size()-4;
  }
}
void Window::printSelection() {
  if (!m_doc)
    return;

  
  
  int x = hasBorders();
  int y = hasBorders();
  int posy = m_doc->m_curYpos;
  auto f = m_doc->m_contents;
  wattron(m_window,COLOR_PAIR(Data::COLOR_SELECTION));
  
  int sy = m_doc->m_starty;
  int ey = m_doc->m_endy;
  int sx = m_doc->m_startx;
  int ex = m_doc->m_endx;
  if (sy==-1 || ey == -1)
    return;
  
  if (sy>ey) {
    swap(sy,ey);
    swap(sx,ex);
  }
  if (sy==ey && sx>ex)
    swap(sx,ex);

  int cp = m_doc->m_curXpos;
  
  while (y<m_height-hasBorders() && posy<f.size()) {
    int dx = 0;
    int size = f[posy].size();

    if (posy==sy)
      dx += sx - cp;
    
    if (posy==ey)
      size = ex-dx - cp;

    int pdx = dx;
    // cut lhs
    if (x+dx<hasBorders())
      {
	dx = -x+hasBorders();
	size -= cp-sx; 
    }
    // cut rhs
    if (x+dx+size>m_width)
      {
	size = (m_width-hasBorders())-(x+dx); 
    }

    wmove(m_window,y,x+dx);
    if (posy>=0 && posy<f.size() && posy>=sy && posy<=ey && size>0) {
      std::string s = f[posy];
      if (cp<f.size()) {
	s = s.substr(m_doc->m_curXpos+dx, std::min((int)m_width-hasBorders()*2, size));
	wprintw(m_window, s.c_str());
      }
      
    }
    posy++;
    y++;
  }
  
}

/*
void Window::printSelection() {
  int cx = m_doc->m_curXpos;
  int x = hasBorders()-cx;
  int y = hasBorders();
  int fy = m_doc->m_starty-m_doc->m_curYpos;
  int ty = m_doc->m_endy-m_doc->m_curYpos;
  if (m_doc->m_starty>m_doc->m_endy) swap(fy,ty);
  int posy = fy+m_doc->m_curYpos;
  auto f = m_doc->m_contents;
  y+=fy;
  wattron(m_window,COLOR_PAIR(Data::COLOR_SELECTION));
  while (y<m_height-hasBorders() && posy<f.size()) {
    if (fy<=ty && y>=hasBorders() && y<m_height-hasBorders())
      {
	auto s = f[posy];
	int sx1 = std::max(0-cx,0);
	int sx2 = std::max(0-cx,(int)s.size());
	
	if (posy == m_doc->m_starty)
	  sx1 = m_doc->m_startx;
	if (posy == m_doc->m_endy)
	  sx2 = m_doc->m_endx;
	if (sx2<sx1)
	  swap(sx1,sx2);
	int cap = 0;
	if (x+sx1<hasBorders()) {
	  sx1=-x + hasBorders();
	}
	if (sx2-sx1>m_width-hasBorders()*2)
	  sx2 = m_width-hasBorders()*2 +m_doc->m_curXpos;
	wmove(m_window,y,x+sx1);
	if (posy>=0 && posy<f.size()) {
	  string f = s.substr(sx1,sx2-sx1);
	  if (m_doc->m_curXpos<f.size())
	    f = f.substr(m_doc->m_curXpos, std::min((int)m_width-hasBorders()*2, (int)f.size()));
	  
	  wprintw(m_window, f.c_str());
	}
      }
    posy++;
    y++;
    fy++;
  }
}
*/

void Window::print() {
  if (!m_doc)
    return;
  m_doc->m_height = m_height;
  m_doc->m_width = m_width;
  if (hasBorders())
    m_doc->m_hasBorders = true;
   
  if (m_type == Editor) printFile();
  if (m_type == Linenumbers) printFile();
  if (m_type == FileList) printFile();
  if (m_type == Windows) printWindowList(false);

  if (m_type == Editor)
    printSelection();
  
  for (auto& c : m_children) {
    c->print();
  }
}
