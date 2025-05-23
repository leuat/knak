#include "window.h"

void Window::printCursor() {

  if (m_type!=Editor)
    return;
  
  wmove(m_window, m_posy, m_posx);
  wattron(m_window,COLOR_PAIR(Data::COLOR_CURSOR));
  char c = ' ';
  if (m_curYpos + m_posy<m_contents.size() && m_posx<m_contents[m_curYpos+m_posy].size())
    c = m_contents[m_curYpos+m_posy][m_posx];
  
  wprintw(m_window,"%c",c);
}



void Window::loadFile(std::string fn) {
  if (!std::filesystem::exists(fn))
    Data::Error("File does not exist: "+fn);
  ifstream in(fn);
  std::vector<std::string> ret;
  std::string s;
  while (getline (in, s)) 
    ret.push_back(s);
   
  in.close();
  m_contents = ret;
}


void Window::refresh() {
  wbkgd(m_window, COLOR_PAIR(Data::COLOR_TEXT));
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


void Window::constrainCursor() {
  if (m_posy==m_height-m_hasBorders) {
    m_posy -=1;
    m_curYpos++;
    
  }
  if (m_posy<m_hasBorders) {
    m_posy +=1;
    if (m_curYpos!=0)
      m_curYpos--;
  }
  if (m_posx<m_hasBorders) {
    m_posx +=1;
  }
  int mmax = m_width;
  if (m_curYpos + m_posy <m_contents.size()) {
    mmax = m_contents[m_curYpos + m_posy].size();                                    
  }
  if (m_posx>=m_width ||  m_posx>mmax) {
    m_posx =mmax;
  }
}


void Window::key(int k) {
  if (m_curYpos + m_posy>=m_contents.size())
    return;
  if (k==127) { // backspace
    if (m_posx<=0)
      return;
    m_contents[m_curYpos + m_posy].erase(m_posx-1,1);
    m_posx--;
    return;
    
  }
  
  m_contents[m_curYpos + m_posy].insert(m_posx,(char*)&k);
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
  int x = m_hasBorders;
  int y = m_hasBorders;
  int posy = m_curYpos;
  auto f = m_contents;
  while (y<m_height-m_hasBorders && posy<f.size()) {
    wmove(m_window,y,x);
    printLine(f[posy]);
    //    printf(f[posy].c_str());
    posy++;
    y++;
  }
}

void Window::print() {
  if (m_type == Editor) printFile();
  if (m_type == Linenumbers) printFile();
      
  for (auto& c : m_children) {
    c->print();
  }
}
