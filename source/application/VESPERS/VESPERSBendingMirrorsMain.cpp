
#include <QApplication>
#include "ui/VESPERS/VESPERSBendingMirrorsView.h"

int main(int argc, char *argv[])
{

	// =================================
	QApplication app(argc, argv);
	app.setApplicationName("VESPERS - Mirror Bending Applications");

	VESPERSBendingMirrorsView *bending = new VESPERSBendingMirrorsView;
	bending->show();

	int retVal = app.exec();

	delete bending;

	return retVal;
}
