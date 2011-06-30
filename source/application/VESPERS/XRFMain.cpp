#include <QApplication>
#include "ui/VESPERS/XRFViewer.h"

int main(int argc, char *argv[])
{

	// =================================
	QApplication app(argc, argv);
	app.setApplicationName("VESPERS XRF Spectra Viewer");

	XRFViewer viewer;
	viewer.show();

	return app.exec();
}

