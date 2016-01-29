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

/*!
  * An abstract helper class which performs the task of building a tree of actions.
  * Allows AMScanActionControllers to decouple the action of building the tree of
  * actions to be performed from the running and monitoring of those actions. Contains
  * a list of the control axes and detectors which are required for the construction
  * of the action tree.
  *
  * Implementation classes specify a generateActionTreeImplmentation to build the
  * required action tree. Notification signals that the tree of actions have been
  * completed are handled by the base class.
  */
class AMScanActionControllerScanAssembler : public QObject
{
	Q_OBJECT

public:
	/*!
	  * Base constructor for an action controller scan assembler. Creates an assembler
	  * with an empty list of control/axes associations, an empty list of detectors
	  * and a null action tree.
	  */
	AMScanActionControllerScanAssembler(QObject *parent = 0);

	/*!
	  * Default destructor for an action controller scan assembler.
	  */
	virtual ~AMScanActionControllerScanAssembler();

	/*!
	  * Inserts a scan axis associated with a specific control to the provided location
	  * in the list.
	  * \param index ~ The position in the list which the control/axis association
	  * will be inserted.
	  * \param axisControl ~ The control which will be associated with the axis.
	  * \param scanAxis ~ The axis which will be associated with the control.
	  * \returns True if the axis/control association is added to the collection,
	  * false otherwise.
	  */
	bool insertAxis(int index, AMControl *axisControl, AMScanAxis *scanAxis);

	/*!
	  * Appends a scan axis associated with a specific control to the end of the
	  * axis list.
	  * \param axisControl ~ The control which will be associated with the axis.
	  * \param scanAxis ~ The axis which will be associated with the control.
	  */
	bool appendAxis(AMControl *axisControl, AMScanAxis *scanAxis);

	/*!
	  * Adds a detector to the list of those which need to be acquired from at each
	  * point of a scan.
	  * \param detector ~ The detector which is to be added to the list of detectors
	  * to acquire from at each point of a scan.
	  * \returns True if the detector is added to the colleciton, false otherwise.
	  */
	bool addDetector(AMDetector *detector);

	/*!
	  * Generates the tree of actions to perform the scan.
	  * \returns True if the tree of actions can be generated, false otherwise.
	  */
	bool generateActionTree();

signals:
	/*!
	  * Signal indicating that the generation of the action tree has been completed
	  * successfully.
	  * NOTE: The scan action controller scan assembler takes no ownership of the
	  * created action tree.
	  * \param actionTree ~ A reference to the generated action tree.
	  */
	void actionTreeGenerated(AMAction3 *actionTree);

protected:
	/*!
	  * Performs the implementation specific details of generating the action tree.
	  *
	  * In the base class this is a pure virtual funciton.
	  * \returns True if the action tree can be generated, false otherwise.
	  */
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
