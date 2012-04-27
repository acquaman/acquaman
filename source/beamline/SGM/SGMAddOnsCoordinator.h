#ifndef SGMADDONSCOORDINATOR_H
#define SGMADDONSCOORDINATOR_H

#include "beamline/AMControlSet.h"

class SGMAddOnsCoordinator : public QObject
{
Q_OBJECT
public:
	/// Simple constructor for class
	SGMAddOnsCoordinator(QObject *parent = 0);

protected slots:
	/// Handles changes coming from the old SGM grating and propogating those (when necessary) to the AddOns version
	void onSGMGratingControlChanged(double sgmGrating);
	/// Handles changes coming from the AddOns grating and propogating those (when necessary) to the old SGM version
	void onAddOnsGratingControlChanged(double addOnsGrating);

	/// Handles changes coming from the old SGM exit slit gap and propogating those (when necessary) to the AddOns version
	void onSGMExitSlitGapControlChanged(double sgmExitSlitGap);
	/// Handles changes coming from the AddOns exit slit gap and propogating those (when necessary) to the old SGM version
	void onAddOnsExitSlitGapControlChanged(double addOnsExitSlitGap);
	/// Handles changes coming from the SGM exit slit gap status and propograting those (almost always) to the AddOns version
	void onSGMExitSlitGapStatusControlChanged(double sgmExitSlitGapStatus);

	/// Handles the backlash correction and monitors when the exit slit gap hits fully open
	void onSGMGratingControlFullyOpened(double sgmExitSlitGap);

	/// Handles watching for when the controls actually connect
	void onAllControlsConnected(bool connected);

protected:
	/// Old SGM grating control setpoint
	AMControl *SGMGratingControl_;
	/// New AddOns grating control setpoint
	AMControl *AddOnsGratingControl_;

	/// Old SGM exit slit gap control setpoint
	AMControl *SGMExitSlitGapControl_;
	/// New AddOns exit slit gap control setpoitn
	AMControl *AddOnsExitSlitGapControl_;
	/// SGM exit slit gap control feedback (actual value for both sides)
	AMControl *SGMExitSlitGapFeedbackControl_;
	/// Holds whether or not we're doing the backlash double movement
	bool movingAddOnsExitSlitGap_;

	/// Old SGM exit slit gap status control
	AMControl *SGMExitSlitGapStatusControl_;
	/// New AddOns exit slit gap status control (different because it won't give a MOVE DONE when it gets fully open before starting to close again)
	AMControl *AddOnsExitSlitGapStatusControl_;

	/// All the controls (for checking connectivity)
	AMControlSet *allControls_;
	/// Holds whether or not we've connected at least once
	bool connectedOnce_;
};

#endif // SGMADDONSCOORDINATOR_H
