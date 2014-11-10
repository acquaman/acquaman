#include <QApplication>

#include "beamline/AMPVControl.h"

int main(int argc, char *argv[])
{
	AMPVControl *pvControl = new AMPVControl("Invalid", "Invalid", "Invalid");

	Q_UNUSED(pvControl)
	Q_UNUSED(argc)
	Q_UNUSED(argv)

	return 0;
}
