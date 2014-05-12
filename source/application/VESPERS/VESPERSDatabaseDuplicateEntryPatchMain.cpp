
#include <QApplication>
#include "dataman/VESPERS/VESPERSDatabaseDuplicateEntryPatch.h"
#include "ui/VESPERS/VESPERSDatabaseDuplicateEntryPatchView.h"

int main(int argc, char *argv[])
{

	/// Program Startup:
	// =================================
	QApplication app(argc, argv);
	app.setApplicationName("Database Duplicate Entry Patch");


	VESPERSDatabaseDuplicateEntryPatch *patch = new VESPERSDatabaseDuplicateEntryPatch();
	VESPERSDatabaseDuplicateEntryPatchView *patchView = new VESPERSDatabaseDuplicateEntryPatchView(patch);
	patchView->show();

	/// Program Run-loop:
	// =================================
	int retVal = app.exec();

	delete patchView;

	return retVal;
}

