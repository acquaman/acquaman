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


#ifndef AMGENERICSCANACTIONCONTROLLERASSEMBLER_H
#define AMGENERICSCANACTIONCONTROLLERASSEMBLER_H

#include "acquaman/AMScanActionControllerScanAssembler.h"

#include "acquaman/AMScanConfiguration.h"

/// Builds the list of actions that can run an entire scan.  Has the base code for all simple scans.
class AMGenericScanActionControllerAssembler : public AMScanActionControllerScanAssembler
{
	Q_OBJECT

public:
	/// Constructor.
	/*!
	 * \param allowsReverseDirection specifies whether a scan can run backwards.
	 * \param direction forces the assember to build the actions to ensure a direction of travel.
	 */
	AMGenericScanActionControllerAssembler(bool automaticDirectionAssessment, AMScanConfiguration::Direction direction, QObject *parent = 0);
	/// Destructor.
	virtual ~AMGenericScanActionControllerAssembler() {}

protected:
	/// Does all the work of building the action tree.
	virtual bool generateActionTreeImplmentation();

	/// Method that determines what type of AMScanAxis needs to be created and then passes that off to the appropriate method.
	AMAction3* generateActionTreeForAxis(AMControl *axisControl, AMScanAxis *scanAxis);
	/// Method that builds a very generic start, delta, end, constant dwell time step axis.
	AMAction3* generateActionTreeForStepAxis(AMControl *axisControl, AMScanAxis *stepScanAxis);
	/// Method that builds a section of an action based on the given AMScanAxisRegion.
	AMAction3* generateActionTreeForStepAxisRegion(AMControl *axisControl, AMScanAxisRegion *stepScanAxisRegion, bool isFinalRegion);
	/// Method that builds a continuous move axis with the given control and axis.
	AMAction3* generateActionTreeForContinuousMoveAxis(AMControl *axisControl, AMScanAxis *continuiousMoveScanAxis);
	/// Method that builds a continuous dwell axis with the given control and axis.
	AMAction3* generateActionTreeForContinuousDwellAxis(AMControl *axisControl, AMScanAxis *continuousDwellScanAxis);
	/// Method that determines what kind of acquisition trigger is required and then creates a list to ensure that all detectors are triggered appropriately.
	AMAction3* generateActionListForDetectorAcquisition();
	/// Method that generates a list of dwell triggers to ensure that all detectors are told to acquire.  Uses the prefilled detector list.
	AMAction3* generateActionListForStepDetectorAcquisition();
	/// Method that generates a list of dwell triggers for continuous dwell acquisitions.
	AMAction3* generateActionListForContinuousDetectorAcquisition();
	/// Method that creates the actions that are required to prepare the detectors to be properly triggered.
	AMAction3* generateActionListForDetectorInitialization();
	/// Method that creates the actions that are required to properly clean up the detector to be used for further acquisition.
	AMAction3* generateActionListForDetectorCleanup();
	/// Method that finds all the placeholder actions that are used to build the action tree effectively.
	QList<AMAction3*> findInsertionPoints(AMAction3 *action);
	/// Method which generates a list of all the detectors to be configured for acquisition.
	virtual QList<AMDetector*> generateListOfDetectorsToConfigure() const;

protected:
	/// Flag that holds whether a scan can scan in any direction (allows the scan assembler to figure it out).
	bool automaticDirectionAssessment_;
	/// Flag that holds the required direction of scanning if automatic direction assessment is disabled.
	AMScanConfiguration::Direction direction_;
};

#endif // AMGENERICSCANACTIONCONTROLLERASSEMBLER_H
