#include "window.h"

void Window::printCursor() {
   
  wmove(m_window, m_posy, m_posx);
  attron(COLOR_PAIR(4));
  char c = ' ';
  if (m_curYpos + m_posy<m_contents.size() && m_posx<m_contents[m_curYpos+m_posy].size())
    c = m_contents[m_curYpos+m_posy][m_posx];
  printw("",c);
}


string Window::replaceAll(string str, const string &from, const string &to)
{
  size_t start_pos = 0;
  while ((start_pos = str.find(from, start_pos)) != string::npos)
    {
      str.replace(start_pos, from.length(), to);
      start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
  return str;
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



uint8_t Window::getColorType(std::string s) {
  s = replaceAll(s, "*","");
  s = replaceAll(s, "#","");
  s = replaceAll(s, "[]","");
  if (std::find(Data::d.keywords.begin(), Data::d.keywords.end(), s) != Data::d.keywords.end()) {
    return 1;
  }
  if (std::find(Data::d.types.begin(), Data::d.types.end(), s) != Data::d.types.end()) {
    return 2;
  }

  return 3;
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
  int x = 0;
  int y = 0;
  int posy = m_curYpos;
  auto f = m_contents;
  while (y<m_height && posy<f.size()) {
    wmove(m_window,y,x);
    printLine(f[posy]);
    posy++;
    y++;
  }
}

void Window::print() {
  if (m_type == Editor) printFile();
  
  for (auto& c : m_children)
    c->print();
}
