
#include <QCoreApplication>

#include "beamline/CLS/CLSSR570Coordinator.h"

#include <QDebug>
#include <QVector>

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	app.setApplicationName("CLS SR570 Coordinator");

	if (argc < 3) {
		qDebug() << "Incorrect parameters. Usages: ";
		qDebug() << "CLSSR570Coordinator IOC_NAME Base_PV_name_of_SR570-1 [Base_PV_name_of_SR570]*";

		return 0;
	}

	QVector<CLSSR570Coordinator *> clsSR570Coordinator;
	for (int i = 2; i < argc; i++) {
		CLSSR570Coordinator *coordinator = new CLSSR570Coordinator(argv[1], argv[i]);
		clsSR570Coordinator.append(coordinator);
	}

	return app.exec();
}

