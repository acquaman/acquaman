/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "acquaman.h"
#include "dataman/database/AMDbObject.h"
#include "dataman/database/AMConstDbObject.h"

/*!

  \mainpage Acquaman: The Comic-book Hero of Synchrotron Science since 2011

  \section intro What is Acquaman?

  Acquaman is a framework used to develop applications the <em>acqu</em>isition and <em>man</em>agement of scientific data.  It is used initially on the SGM and REIXS beamlines at the Canadian Light Source.

  \section userDocIntro Introduction For Beamline users...

Acquaman is used in various forms to build applications with different functionality for each beamline.  Consult your beamline staff for user manuals specific to your version of Acquaman.


  \section programmerDocIntro Introduction For Programmers...

  For an introductory workshop to the Acquaman Framework, see these <a href="http://acquaman.github.com/acquaman/doc/html/AcquamanOrientationWorkshop_March2011.pdf">slides from March 2011</a>, which cover:


	- Core principles and programming paradigms
	- Support systems: How to get, build, use, and learn about Acquaman
	- Features and Functionality: What it can do
	- Application examples for each module / core feature: Where to start

  \subsection architecture Architecture and Modules
  \image html architecture.png "Figure 1: Architecture of the Acquaman Framework"

  The Acquaman framework is divided into 6 modules that group related functionality.  The dependency of the modules is shown in Figure 1: each block makes use of the components directly below it.

  \b Modules
  - \c beamline : Control interface to the actual beamline hardware
  - \c acquaman : Manages setting up and executing scans
  - \c dataman : Responsible for the representation, storage and retrieval of experimental data, including a persistent-object database ORM.
  - \c workflow/actions : A program-wide system for scheduling and running a series of "user-level" actions
  - \c ui : Contains all user-interface widgets. These may depend on code in other modules, but all of the other modules are free of any GUI code, and can therefore be used on systems without a GUI.
  */


namespace AM {
	/// Register acquaman-defined types with the QMetaType system. Call on application startup before using these types.
	void registerTypes() {
		qRegisterMetaType<AMIntList>();
		qRegisterMetaType<AMDoubleList>();
		qRegisterMetaType<AMDbObjectList>();
		qRegisterMetaType<AMConstDbObjectList>();
		qRegisterMetaType<AMDbObject*>();
		qRegisterMetaType<AMHighPrecisionDateTime>();
		qRegisterMetaType<AMConstDbObject*>();
		qRegisterMetaType<AMQVector3DVector>();
	}
}
 AMAcqEvent::~AMAcqEvent(){}
 AMAcqErrorEvent::~AMAcqErrorEvent(){}
// AMHighPrecisionDateTime::~AMHighPrecisionDateTime(){}
