#include <QApplication>

#include "application/TimeEvolutionStripTool/AMTESTAppController.h"

int main(int argc, char *argv[])
{
	QApplication application(argc, argv);
	application.setApplicationName("Time Evolution Strip Tool");

	AMTESTAppController *controller = new AMTESTAppController;

	int exitCode = -1;

	if (controller->startup())
		exitCode = application.exec();

	if (controller->isRunning())
		controller->shutdown();

	delete controller;

	return exitCode;
}
