
#include <QApplication>
#include "ui/VESPERS/VESPERSBendingMirrors.h"

int main(int argc, char *argv[])
{

	// =================================
	QApplication app(argc, argv);
	app.setApplicationName("VESPERS - Mirror Bending Applications");

	VESPERSBendingMirrors bending;
	bending.show();

	return app.exec();
}
