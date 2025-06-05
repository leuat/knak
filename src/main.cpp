#include "mainapp.h"

MainApp app;


int main(int argc, char ** argv)
{
  app.init();
  if (argc!=1)
    app.loadDocument(argv[1]);
  app.fileWindow->m_doc->loadDir(".");
  
  while (!app.isDone) {
    app.update();
  }
  endwin();
  return 0;
}



