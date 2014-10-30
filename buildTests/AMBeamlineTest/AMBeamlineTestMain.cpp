#include <QApplication>

#include "beamline/AMBeamline.h"

int main(int argc, char *argv[])
{
	AMBeamline *beamline = AMBeamline::bl();

	Q_UNUSED(beamline)
	Q_UNUSED(argc)
	Q_UNUSED(argv)

	return 0;
}
