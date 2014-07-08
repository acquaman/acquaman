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


#ifndef SGMADDONSCOORDINATOR_H
#define SGMADDONSCOORDINATOR_H

#include "beamline/AMControlSet.h"

class SGMAddOnsCoordinator : public QObject
{
Q_OBJECT
public:
	/// Simple constructor for class
 	virtual ~SGMAddOnsCoordinator();
	SGMAddOnsCoordinator(QObject *parent = 0);

protected slots:
	/// Handles changes coming from the old SGM grating and propogating those (when necessary) to the AddOns version
	void onSGMGratingControlChanged(double sgmGrating);
	/// Handles changes coming from the AddOns grating and propogating those (when necessary) to the old SGM version
	void onAddOnsGratingControlChanged(double addOnsGrating);

	/// Handles changes comgin from the old SGM harmonic and propogating those (when necessary) to the AddOns version
	void onSGMHarmonicControlChanged(double sgmHarmonic);
	/// Handles changes coming from the AddOns harmonic and propogating those (when necessary) to the old SGM version
	void onAddOnsHarmonicControlChanged(double addOnsHarmonic);

	/// Handles changes coming from the old SGM exit slit gap and propogating those (when necessary) to the AddOns version
	void onSGMExitSlitGapControlChanged(double sgmExitSlitGap);
	/// Handles changes coming from the AddOns exit slit gap and propogating those (when necessary) to the old SGM version
	void onAddOnsExitSlitGapControlChanged(double addOnsExitSlitGap);
	/// Handles changes coming from the SGM exit slit gap status and propograting those (almost always) to the AddOns version
	void onSGMExitSlitGapStatusControlChanged(double sgmExitSlitGapStatus);
	/// Handles changes in the AddOnsGapMoving
	void onAddOnsExitSlitGapAddOnsMoving(double addOnsExitSlotGapAddOnsMoving);

	/// Handles the backlash correction and monitors when the exit slit gap hits fully open
	void onSGMGratingControlFullyOpened(double sgmExitSlitGap);

	/// Handles watching for when the controls actually connect
	void onAllControlsConnected(bool connected);

	/// Handles requests to force a check of the error status of the application
	void onAddOnsErrorCheckValueChanged(double errorCheck);

	/// Handles changes to the MDEL value for the old SGM exit slit gap
	void onAddOnErrorSGMExitSlitStatusMDELValueChanged();
	/// Handles changes to the MDEL value for the new AddOns exit slit gap
	void onAddOnErrorAddOnsExitSlitStatusMDELValueChanged();

protected:
	/// Helper function to handle checking errors and setting the error status
	void checkErrors(bool forceCheck = false);

	/// Helper function to check on the SGM Grating ENUM
	void checkSGMGratingErrors();
	/// Helper function to check on the AddOns Grating ENUM
	void checkAddOnsGratingErrors();
	/// Helper function to check on the SGM Harmonic ENUM
	void checkSGMHarmonicErrors();
	/// Helper function to check on the AddOns Harmonic ENUM
	void checkAddOnsHarmonicErrors();
	/// Helper function to check on the SGM Exit Slit status ENUM
	void checkSGMExitSlitGapStatusErrors();

	/// Helper function to check on the old SGM exit slit gap MDEL
	void checkSGMExitSlitGapMDELErrors();
	/// Helper function to check on the new AddOns exit slit gap MDEL
	void checkAddOnsExitSlitGapMDELErrors();

protected:
	/// Old SGM grating control setpoint
	AMControl *SGMGratingControl_;
	/// New AddOns grating control setpoint
	AMControl *AddOnsGratingControl_;

	/// Old SGM harmonic control setpoint
	AMControl *SGMHarmonicControl_;
	/// New AddOns harmonic control setpoint
	AMControl *AddOnsHarmonicControl_;

	/// Old SGM exit slit gap control setpoint
	AMControl *SGMExitSlitGapControl_;
	/// New AddOns exit slit gap control setpoitn
	AMControl *AddOnsExitSlitGapControl_;
	/// SGM exit slit gap control feedback (actual value for both sides)
	AMControl *SGMExitSlitGapFeedbackControl_;
	/// Holds whether or not we're doing the backlash double movement
	bool movingAddOnsExitSlitGap_;
	/// Provides EPICS access to the above boolean
	AMControl *AddOnsExitSlitGapAddOnsMoving_;

	/// Holds whether or not the move was initiated by the old SGM exit slit gap PV
	bool sgmInitiatedMovement_;
	/// Holds whether or not the move was initiated by the new AddOns exit slit gap PV
	bool addonsInitiatedMovement_;

	/// Old SGM exit slit gap status control
	AMControl *SGMExitSlitGapStatusControl_;
	/// New AddOns exit slit gap status control (different because it won't give a MOVE DONE when it gets fully open before starting to close again)
	AMControl *AddOnsExitSlitGapStatusControl_;

	/// Holds the value that is considered "fully open" for driving the exit slit gap
	AMControl *AddOnsExitSlitGapFullyOpenValueControl_;

	/// Allows for forcing a check on the error status of this application
	AMControl *AddOnsErrorCheckControl_;
	/// Reports the error status of this application
	AMControl *AddOnErrorStatusControl_;

	/// Read only control to check on the MDEL value for the old SGM exit slit gap setpoint
	AMControl *AddOnErrorSGMExitSlitStatusMDELControl_;
	/// Read only control to check on the MDEL value for the new AddOns exit slit gap setpoint
	AMControl *AddOnErrorAddOnsExitSlitStatusMDELControl_;

	/// Holds a list of strings for errors that have been detected
	QStringList errorsList_;

	/// All the controls (for checking connectivity)
	AMControlSet *allControls_;
	/// Holds whether or not we've connected at least once
	bool connectedOnce_;
};

#endif // SGMADDONSCOORDINATOR_H
