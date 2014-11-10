#include <QApplication>

#include "beamline/AMControlSet.h"

int main(int argc, char *argv[])
{
	AMControlSet *controlSet = new AMControlSet();

	Q_UNUSED(controlSet)
	Q_UNUSED(argc)
	Q_UNUSED(argv)

	return 0;
}
