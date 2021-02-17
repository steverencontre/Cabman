#include "MainWindow.h"
#include <QApplication>


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QCoreApplication::setOrganizationDomain ("rsn-tech.co.uk");
	QCoreApplication::setOrganizationName ("RSN Technology");
	QCoreApplication::setApplicationName ("Cabman");

	MainWindow w;
	w.show();

	return a.exec();
}
