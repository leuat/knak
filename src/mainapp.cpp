#include "mainapp.h"
#include <algorithm>

void MainApp::initColorValues() {
  init_color(COLOR_GREEN, 0, 750, 250);
  init_color(COLOR_RED, 0, 1000, 500);
  init_color(COLOR_BLUE, 100, 300, 1000);
  init_color(COLOR_BLACK, 30, 50, 100);
  init_color(COLOR_WHITE, 500, 500, 600);
}

void MainApp::closeDocument(std::string fn) {
  if (fn==Data::d.buildWindowName)
    return;
  for (int i=0;i<documents.size();i++) {
    if (documents[i]->m_currentFile==fn) {
      documents.erase(documents.begin()+i);
     windowWindow->m_doc->moveCursor(337);
     setDocument(windowWindow->m_doc->m_contents[windowWindow->m_doc->m_posy]);
     windowWindow->m_doc->m_contents.erase(std::remove(windowWindow->m_doc->m_contents.begin(), windowWindow->m_doc->m_contents.end(), fn), windowWindow->m_doc->m_contents.end());
      return;
    }
  }
}

bool MainApp::setDocument(std::string fn) {
  for (auto& d : documents) {
    if (d->m_currentFile == fn) {
      editorWindow->m_doc = d;
      return true;
    }
      
  }
  return false;
}


void MainApp::loadDocument(std::string fn) {
  if (setDocument(fn))
    return;
  auto doc = make_shared<Document>();
  doc->loadFile(fn);
  windowWindow->m_doc->m_contents.push_back(fn);
  editorWindow->m_doc = doc;
  documents.push_back(doc);
  
}

void MainApp::buildProject() {

  setDocument(Data::d.buildWindowName);
  auto stream = popen((Data::d.build_command+std::string(" 2>&1")).c_str(), "r");
  const int max_buffer = 256;
  char buffer[max_buffer];
  build->m_contents.clear();
  if (stream) {
    while (!feof(stream))
      if (fgets(buffer, max_buffer, stream) != NULL) build->m_contents.push_back(std::string(buffer));
    pclose(stream);
  }
}

void MainApp::execute() {
  if (std::filesystem::exists(Data::d.execute_command)) {
    system((Data::d.execute_command + " " + Data::d.execute_parameters).c_str());
    if (Data::d.execute_command == "./knak")
      Data::quit();
  }
}

void MainApp::blink() {
  for (int j=0;j<100;j++) {
    int i = (1000-j*10)*0.2;
    init_color(COLOR_BLACK, i,i,i);
    refresh();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

}
int MainApp::moveCursor(Window* w) {
  // refreshes the screen
  
  int pposx = w->m_doc->m_posx;
  int pposy = w->m_doc->m_posy;
  //  nonl();
  cbreak();
  raw();
  keypad(stdscr, true);  
    
  int v = getch();
  //      printf("key: %i     \n",v);

  if (v==ctrl('q'))
    isDone = true;
  
  if (v==ctrl('c')) {
    w->m_doc->copySelection();
    return -1;
  }
  if (v==ctrl('x')) {
    w->m_doc->copySelection();
    w->m_doc->eraseSelection();
    return -1;
  }
  /*
  if (v==ctrl('y')) {
    curWindow = tabOrder[0];
    return -1;
  }
  if (v==ctrl('t')) {
    curWindow = tabOrder[1];
    return -1;
    }*/
  if (v==353) {
    curTab=(curTab+1)&1;
    curWindow = tabOrder[curTab];
    return -1;
  }
  if (v==ctrl('r')) {
    //    curWindow = tabOrder[2];
    execute();
    return -1;
  }
  if (v==ctrl('w')) {
    closeDocument(editorWindow->m_doc->m_currentFile);
    return -1;
  }
  if (v==ctrl('b')) {
    buildProject();
    return -1;
  }
  if (v==ctrl('c')) {
    w->m_doc->copySelection();
    return -1;
  }
  if (v==ctrl('s')) {
    if (w->m_type == Window::Editor) {
      w->m_doc->save();
      blink();
    }
    return -1;
  }

  if (v==ctrl('v')) {
    w->m_doc->snap();
    w->m_doc->pasteSelection();
    return -1;
  }
  if (v==ctrl('z')) {
    w->m_doc->undo();
    return -1;
  }


  if (v==10) { // enter
    // Load file
    if (curWindow->m_type==Window::FileList) {
      if (Util::isDirectory(fileWindow->m_doc->getCurrentLine()))
	fileWindow->m_doc->loadDir(fileWindow->m_doc->getCurrentLine());
      else
	loadDocument(fileWindow->m_doc->getCurrentLine());
      return -1;
    }
    if (curWindow->m_type==Window::Windows) {
      setDocument(windowWindow->m_doc->m_contents[windowWindow->m_doc->m_posy]);
      return -1;
    }

  }
  if (curWindow->m_doc->moveCursor(v))
    return -1;

  if (v==534) {
    windowWindow->m_doc->moveCursor(337);
    setDocument(windowWindow->m_doc->m_contents[windowWindow->m_doc->m_posy]);
    return -1;
  }
  if (v==575) {
    windowWindow->m_doc->moveCursor(336);
    setDocument(windowWindow->m_doc->m_contents[windowWindow->m_doc->m_posy]);
    return -1;
  }
  if (v==338) {
    w->m_doc->pageDown();
    return -1;
  }
  if (v==339) {
    w->m_doc->pageUp();
    return -1;
  }

  /*  
  if (v==27) {
    int v1 = getch();
    int v2 = getch();
    //    printf("key: %i %i %i    \n",v,v1,v2);
    //             printf("key27: %i\n",v);
    //w->m_doc->moveCursor(v2,false);
    
    if (v2==27) {
      getch();
      w->m_doc->clearSelection();
      return -1;
      }
    curWindow->m_doc->constrainCursor();
    return -1;
  }
  */
  //  printf("keyball: %i",v);
  // don't clear if backspace

  //  std::string ignoreCtrlKeys = "abcdefghijklmnopqrstuvwzyxABCDEFGHIJKLMNOPQRSTUVXYZ123456789+\\|-.,:_*?=)(/&%¤#§!";
  //  std::string ignoreCtrlKeys = "abcdefghiklmnopqrsuvwzyxABCDEFGHIKLNOPQRSUVXYZ+\\|-.,:_?=)(/&%¤#§!";
  /*  std::string ignoreCtrlKeys = "";
  for (char& key: ignoreCtrlKeys) {
    if (v==ctrl(key))
      return -1;
  }
  */

  
  if (v!=127)
    w->m_doc->clearSelection();
  curWindow->key(v);
  
  return v;

}

void MainApp::initColors() {
  start_color();
  initColorValues();
  init_pair(Data::COLOR_KEYWORD, COLOR_RED, COLOR_BLACK);
  init_pair(Data::COLOR_TYPE, COLOR_BLUE, COLOR_BLACK);
  init_pair(Data::COLOR_TEXT, COLOR_WHITE, COLOR_BLACK);
  init_pair(Data::COLOR_CURSOR, COLOR_BLACK, COLOR_RED);
  init_pair(Data::COLOR_SYMBOL, COLOR_GREEN, COLOR_BLACK);
  init_pair(Data::COLOR_SELECTION, COLOR_BLACK, COLOR_YELLOW);
}



void MainApp::init() {
  initscr();
  initColors();
  mainWindow.Init(stdscr, Window::Empty);

  float fileSplit = 0.2;
  float mainSplitY = 0.1;
  float lineSplit = 0.07;

  Data::d.project_path = std::filesystem::current_path();
  
  auto mainw = mainWindow.addChild(Window::Empty, fileSplit, mainSplitY, 1-fileSplit, 1-mainSplitY*2);
  fileWindow = mainWindow.addChild(Window::FileList, 0, mainSplitY, fileSplit, 1-mainSplitY);
  fileWindow->m_hasBorders = true;
  windowWindow = mainWindow.addChild(Window::Windows, fileSplit, 0, 1.0-fileSplit, mainSplitY);
  windowWindow->m_hasBorders = true;
  
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
  tabOrder.push_back(editorWindow);
  tabOrder.push_back(fileWindow);
  tabOrder.push_back(windowWindow);

  build = make_shared<Document>();
  build->m_currentFile = Data::d.buildWindowName;
  build->m_isLocked = true;
  documents.push_back(build);
  windowWindow->m_doc->m_contents.push_back(build->m_currentFile);
 
}

void MainApp::update() {
  windowWindow->m_doc->m_currentFile = editorWindow->m_doc->m_currentFile;
  if (true) {
    lineWindow->m_doc->fillLines(editorWindow->m_doc->m_curYpos);
    mainWindow.print();
    doRefresh = false;
  }
  curWindow->printCursor();
  mainWindow.refresh();
  
  auto key = moveCursor(curWindow.get());
}
