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


#ifndef AMSCANACTIONCONTROLLERSCANASSEMBLER_H
#define AMSCANACTIONCONTROLLERSCANASSEMBLER_H

#include "beamline/AMDetectorSet.h"
#include "beamline/AMControlSet.h"
#include "dataman/AMScanAxis.h"

/// This class defines the basics for a scan action based controller.  Subclasses must implement the buildActionTreeImplementation to create the scan they desire.
class AMScanActionControllerScanAssembler : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	AMScanActionControllerScanAssembler(QObject *parent = 0);
	/// Destructor.
	virtual ~AMScanActionControllerScanAssembler();

	/// Inserts a scan axis associated with a specific control at a particular location in the axis list.
	bool insertAxis(int index, AMControl *axisControl, AMScanAxis *scanAxis);
	/// Appends a scan axis associated with a specific control to the end of the axis list.
	bool appendAxis(AMControl *axisControl, AMScanAxis *scanAxis);
	/// Adds a detector to the list of detectors that need to be acquired at each point of a scan.
	bool addDetector(AMDetector *detector);

	/// Does all the work of generating a scan action tree.  Calls the generateActionTreeImplementation for all specific behaviour.
	bool generateActionTree();

signals:
	/// Notifier that an action tree was generated.  Passes a pointer of the tree.
	void actionTreeGenerated(AMAction3 *actionTree);

protected:
	/// Does all the work of building the action tree.  Must be implemented by subclasses.
	virtual bool generateActionTreeImplmentation() = 0;

	/// The list of detectors.
	AMDetectorSet *detectors_;
	/// The list  of controls for each axis.
	AMControlSet *controls_;
	/// The list of axes.
	QList<AMScanAxis*> axes_;
	/// The scan controller action tree.
	AMAction3 *actionTree_;
};

#endif // AMSCANACTIONCONTROLLERSCANASSEMBLER_H
