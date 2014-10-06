
#include <QCoreApplication>

#include "beamline/CLS/CLSSR570Coordinator.h"

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	app.setApplicationName("CLS SR570 Coordinator");

	CLSSR570Coordinator *coordinator = new CLSSR570Coordinator("IOC2026-001", "AMP2026-001");

	Q_UNUSED(coordinator)

	return app.exec();
}

