/*
 * For SR570, when the value of sens_put PV is changed, the values of
 * sens_num and sens_unit are not updated accordingly.
 * This CLSSR570Coordinator is designed to update sens_num/sens_unit
 * when the value change of sens_put is changed.To reach this goal,
 * we have to disable the sens_num/sens_unit PVs before we write the value.
 * Otherwise, the write operation will trigger an update of sens_put PV.
 *
 * To disable a PV, we should set .DISA and .DISV same value;
 * To enable a PV, we should set .DISA and .DISV differt values and put a non-zero value to .PROC
 *
 * CLSSR570Coordinator can monitor mutiple SR570s at the same time by passing in the base PV name of each SR570
 *
 * Usages:
 *     CLSR570Coordinator IOC_Name Base_PV_name_of_SR570-1 [Base_PV_name_of_SR570]*
 *
 */

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
		qDebug() << "CLSSR570Coordinator IOC_Name Base_PV_name_of_SR570-1 [Base_PV_name_of_SR570]*";

		return 0;
	}

	QVector<CLSSR570Coordinator *> clsSR570Coordinator;
	for (int i = 2; i < argc; i++) {
		CLSSR570Coordinator *coordinator = new CLSSR570Coordinator(argv[1], argv[i]);
		clsSR570Coordinator.append(coordinator);
	}

	return app.exec();
}

