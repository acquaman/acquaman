#include <QApplication>

#include "beamline/AMControl.h"

int main(int argc, char *argv[])
{
	AMControl *control = new AMControl("Invalid");

	Q_UNUSED(control)
	Q_UNUSED(argc)
	Q_UNUSED(argv)

	return 0;
}
