#include "document.h"


void Document::loadDir(std::string dn) {
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


void Document::loadFile(std::string fn) {
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


void Document::constrainCursor(int diffy) {
  if (m_curYpos<0) {
    m_curYpos = 0;
    m_posy = 0;
  }
  if (m_curYpos>=m_contents.size()-m_height)
    m_curYpos = m_contents.size()-m_height-1;
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


void Document::key(int k) {
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
void Document::undo() {
  if (m_snaps.size() == 0)
    return;
  auto s = m_snaps.back();
  m_contents = s.m_contents;
  m_posx = s.m_posx;
  m_posy = s.m_posy;
  m_curYpos = s.m_curYpos;
  m_snaps.pop_back();
}

void Document::snap() {
  m_snaps.push_back(Snap(m_contents, m_posx, m_posy, m_curYpos));
  if (m_snaps.size()>MAX_UNDO) {
    m_snaps.erase(m_snaps.begin()+0);
  }
}

void Document::pasteSelection() {
  if (m_selection.size()==0)
    return;
  int posy = getYpos();
  int posx = m_posx;
  for (auto &s : m_selection) {
    if (posx!=0)
      m_contents[posy].insert(posx,s);
    else m_contents.insert(m_contents.begin() + posy,s);
    if (posx==0) moveCursorDown();
    posx = 0;
    posy++;
  }
}

void Document::copySelection() {
  m_selection.clear();
  if (m_starty==-1 || m_endy == -1)
    return;
  int sy = m_starty;
  int ey = m_endy;
  if (sy>ey)
    swap(sy,ey);
  if (sy==ey)
    ey++;
  
  for (int posy = sy; posy<ey;posy++) {
    auto s = m_contents[posy];
    int sx1 = 0;
    int sx2 = s.size();
    if (posy == m_starty)
      sx1 = m_startx;
    if (posy == m_endy)
      sx2 = m_endx;
    if (sx2<sx1)
      swap(sx1,sx2);
    m_selection.push_back(s.substr(sx1,sx2-sx1));
  }
    
  
}
