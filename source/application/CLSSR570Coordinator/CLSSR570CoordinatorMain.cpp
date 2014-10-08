
#include <QCoreApplication>

#include "beamline/CLS/CLSSR570Coordinator.h"

#include <QDebug>

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	app.setApplicationName("CLS SR570 Coordinator");

	if (argc < 3) {
		qDebug() << "Incorrect parameters. Usages: ";
		qDebug() << "CLSSR570Coordinator [IOC NAME] [Base PV name of SR570]";

		return 0;
	}

	CLSSR570Coordinator *coordinator = new CLSSR570Coordinator(argv[1], argv[2]);

	Q_UNUSED(coordinator)

	return app.exec();
}

