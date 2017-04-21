#include "MainWindow.h"
#include <QApplication>

#include <iostream>

int main(int argc, char *argv[])
{
  std::cout << "C++ version " << __cplusplus << std::endl;

  QApplication a(argc, argv);
  MainWindow w;
  w.show();

  return a.exec();
}
