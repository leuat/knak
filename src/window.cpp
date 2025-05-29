#include "window.h"
#include <filesystem>

void Window::printCursor() {
  if (m_type!=Editor && m_type!=FileList)
    return;
  wattron(m_window,COLOR_PAIR(Data::COLOR_CURSOR));
  int posy = getYpos();
  if (m_type==FileList) {
    // display entire line
    wmove(m_window, m_posy, hasBorders());
    if (posy<m_contents.size())
      wprintw(m_window,m_contents[posy].c_str());
    
    wattron(m_window,COLOR_PAIR(Data::COLOR_TEXT));
    return;
  }
  
  wmove(m_window, m_posy, m_posx+hasBorders());
  char c = ' ';
  if (posy<m_contents.size() && m_posx<m_contents[posy].size())
    c = m_contents[posy][m_posx];
  
  wprintw(m_window,"%c",c);
  wattron(m_window,COLOR_PAIR(Data::COLOR_TEXT));
}


void Window::loadDir(std::string dn) {
  m_contents.clear();
  m_contents.push_back("../");
  for (const auto & entry : std::filesystem::directory_iterator(dn)) {
    auto s = entry.path();
    //    s = Util::replaceAll(s,"./","");
    m_contents.push_back(s);
  }
  m_currentFile = dn;
  m_posy = 0;
  m_curYpos = 0;
}


void Window::loadFile(std::string fn) {
  if (!std::filesystem::exists(fn))
    Data::Error("File does not exist: "+fn);
  ifstream in(fn);
  std::vector<std::string> ret;
  std::string s;
  while (getline (in, s)) {
    s = Util::replaceAll(s,"\t",Data::s_tab);
    ret.push_back(s);
  }
   
  in.close();
  m_contents = ret;
  m_posx = 0;
  m_posy = 0;
  m_curYpos = 0;
  m_currentFile = fn;
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


void Window::constrainCursor(int diffy) {
  if (m_posy==m_height-hasBorders()) {
    m_posy -=1;
    m_curYpos++;
    
  }
  if (m_curYpos+m_posy>=m_contents.size()) {
    m_posy = m_contents.size()-m_curYpos;
  }
  if (m_posy<hasBorders()) {
    m_posy +=1;
    if (m_curYpos!=0)
      m_curYpos--;
  }
  
  if (m_posx<0) {
    if (m_posy>0)
      m_posy-=1;
    m_posx = getCurrentLine().size();
  }
  int mmax = getCurrentLine().size();
  if (m_posx>=m_width || m_posx>mmax) {
    m_posy+=diffy;
    m_posx = getFirstCharPos();
  }
  
}


void Window::key(int k) {
  if (m_type==FileList)
    return;
  
  if (getYpos()>=m_contents.size())
    return;

  if (k=='\t') {
    m_contents[getYpos()].insert(m_posx,Data::s_tab);
    m_posx+=Data::s_tab.size();
    return;
  }
  

  
  if (k==10) { // enter
      auto bottom = getCurrentLine().substr(m_posx, getCurrentLine().size());
      auto top = getCurrentLine().substr(0, m_posx);
      m_contents[getYpos()] = top;
      m_contents.insert(m_contents.begin()+getYpos()+1,bottom);
      moveCursorDown();
      m_posx = getFirstCharPos();
      
      return;
  }

  if (k==127) { // backspace
    if (m_posx<0)
      return;
    if (m_posx == 0 && getYpos()!=0) {
      // copy back
      auto s = getCurrentLine();
      auto np = m_contents[getYpos()-1].size();
      m_contents[getYpos()-1]+=s;
      m_contents.erase(m_contents.begin() + getYpos());
      moveCursorUp();
      m_posx = np;
      return;
    }
    m_contents[getYpos()].erase(m_posx-1,1);
    m_posx--;
    return;
    
  }
  
  m_contents[getYpos()].insert(m_posx,(char*)&k);
  m_posx++;
  
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
  
  std::string s;
  auto is = istringstream(f);
  while (getline(is, s, ' ')) {
    wattron(m_window,COLOR_PAIR(getColorType(s)));
    wprintw(m_window, s.c_str());
    wprintw(m_window," ");
  }
}

void Window::printFile() {
  werase(m_window);
  int x = hasBorders();
  int y = hasBorders();
  int posy = m_curYpos;
  auto f = m_contents;
  while (y<m_height-hasBorders() && posy<f.size()) {
    wmove(m_window,y,x);
    if (posy>=0 && posy<f.size())
      printLine(f[posy]);
    posy++;
    y++;
  }
}

void Window::printSelection() {
  int x = hasBorders();
  int y = hasBorders();
  int fy = m_starty-m_curYpos;
  int ty = m_endy-m_curYpos;
  if (m_starty>m_endy) swap(fy,ty);
  int posy = m_starty;
  auto f = m_contents;
  y+=fy;
  wattron(m_window,COLOR_PAIR(Data::COLOR_SELECTION));
  while (y<m_height-hasBorders() && posy<f.size()) {
    if (fy<ty && y>=1 && y<m_height-hasBorders())
      {
	wmove(m_window,y,x);
	auto s = f[posy];
	if (posy>=0 && posy<f.size())
	  wprintw(m_window, s.c_str());
      }
    posy++;
    y++;
    fy++;
  }
}

void Window::print() {
  if (m_type == Editor) printFile();
  if (m_type == Linenumbers) printFile();
  if (m_type == FileList) printFile();
  
  printSelection();
  
  for (auto& c : m_children) {
    c->print();
  }
}
