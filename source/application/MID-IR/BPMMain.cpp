#include <QApplication>
#include "ui/MID-IR/BeamPositionMonitor.h"

int main(int argc, char *argv[])
{

	// =================================
	QApplication app(argc, argv);
	app.setApplicationName("MID-IR Beam Position Monitor");

	BeamPositionMonitor bpm;
	bpm.show();

	return app.exec();
}
