/*!

  \mainpage Acquaman: The Comic-book Hero of Synchrotron Science since 2011

  \section intro What is Acquaman?

  Acquaman is a fausome framework used for the <em>acqu</em>isition and <em>man</em>agement of experimental data.  It is used initially on the SGM and REIXS beamlines at the Canadian Light Source.

  \section userDocIntro For Beamline users...


  \section programmerDocIntro For Programmers...

  \subsection architecture Architecture and Modules
  \image html architecture.png "Figure 1: Architecture of the Acquaman Framework"

  The Acquaman framework is divided into 6 modules that group related functionality.  The dependency of the modules is shown in Figure 1: each block makes use of the components directly below it.

  \b Modules
  - \c beamline : Control interface to the actual beamline hardware
  - \c acquaman : Manages setting up and executing scans
  - \c dataman : Responsible for the representation (top layer), and storage and retrieval (bottom layer) of experimental data.
  - \c workflow : A program-wide system for scheduling and running a series of "user-level" actions
  - \c script : Provides the advanced user with powerful and direct access to the beamline, acquaman, and dataman layers
  - \c ui : Contains all user-interface widgets

  Some rules:
	- Any classes from the QtGui Qt module are used only in the ui module.
	- ...


  \section miscJunk For Boots and Dave...
  \subsection todo Misc. Todos

  \todo check out QFlags and figure out where to standardize flags

  \todo cooler notifications: http://blog.mpathirage.com/2009/09/12/using-libnotify-in-ubuntu-9-04/

  */



#include <QApplication>
#include "AMAppController.h"


int main(int argc, char *argv[])
{

	/// Program Startup:
	// =================================
	QApplication app(argc, argv);
	app.setApplicationName("Acquaman");


	AMAppController* appController = new AMAppController();


	/// Program Run-loop:
	// =================================
	int retVal = -1;
	if(appController->startup())
		retVal = app.exec();

	/// Program Shutdown:
	// =================================
	delete appController;

	return retVal;
}

