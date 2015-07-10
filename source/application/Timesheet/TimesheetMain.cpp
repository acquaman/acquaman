#include <QApplication>
#include "TimesheetMainWindow.h"
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	TimesheetMainWindow w;
	w.show();

	return a.exec();
}
