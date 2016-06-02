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


#ifndef VESPERSAddOnsCoordinator_H
#define VESPERSAddOnsCoordinator_H

#include "beamline/AMControlSet.h"
#include "beamline/AMPVControl.h"

#include <QTimer>

class VESPERSAddOnsCoordinator : public QObject
{
Q_OBJECT
public:
	/// Simple constructor for class
	VESPERSAddOnsCoordinator(QObject *parent = 0);

	/// Destructor
	virtual ~VESPERSAddOnsCoordinator();

protected slots:
	/// Handles watching for when the controls actually connect
	void onAllControlsConnected(bool connected);

	// Sample H, V, N

	void onOldSampleHSetpointControlChanged();
	void onOldSampleHFeedbackControlChanged();

	void onOldSampleVSetpointControlChanged();
	void onOldSampleVFeedbackControlChanged();

	void onOldSampleNSetpointControlChanged();
	void onOldSampleNFeedbackControlChanged();

	void onAddOnsSampleHSetpointControlChanged();
	void onAddOnsSampleVSetpointControlChanged();
	void onAddOnsSampleNSetpointControlChanged();

	void restoreAddOnsSampleHStatus();
	void restoreAddOnsSampleVAndNStatus();

	// Sample X, Z, Y

	void onOldSampleXSetpointControlChanged();
	void onOldSampleXFeedbackControlChanged();

	void onOldSampleZSetpointControlChanged();
	void onOldSampleZFeedbackControlChanged();

	void onOldSampleYSetpointControlChanged();
	void onOldSampleYFeedbackControlChanged();

	void onAddOnsSampleXSetpointControlChanged();
	void onAddOnsSampleZSetpointControlChanged();
	void onAddOnsSampleYSetpointControlChanged();

	void restoreAddOnsSampleXStatus();
	void restoreAddOnsSampleZStatus();
	void restoreAddOnsSampleYStatus();

	// Sample low level status

	void onOldRealSampleXStatusControlChanged();
	void onOldRealSampleZStatusControlChanged();
	void onOldRealSampleYStatusControlChanged();

	// Atto H, V, N

	void onOldAttoHSetpointControlChanged();
	void onOldAttoHFeedbackControlChanged();

	void onOldAttoVSetpointControlChanged();
	void onOldAttoVFeedbackControlChanged();

	void onOldAttoNSetpointControlChanged();
	void onOldAttoNFeedbackControlChanged();

	void onAddOnsAttoHSetpointControlChanged();
	void onAddOnsAttoVSetpointControlChanged();
	void onAddOnsAttoNSetpointControlChanged();

	void restoreAddOnsAttoHStatus();
	void restoreAddOnsAttoVAndNStatus();

	// Atto X, Z, Y

	void onOldAttoXSetpointControlChanged();
	void onOldAttoXFeedbackControlChanged();

	void onOldAttoZSetpointControlChanged();
	void onOldAttoZFeedbackControlChanged();

	void onOldAttoYSetpointControlChanged();
	void onOldAttoYFeedbackControlChanged();

	void onAddOnsAttoXSetpointControlChanged();
	void onAddOnsAttoZSetpointControlChanged();
	void onAddOnsAttoYSetpointControlChanged();

	void restoreAddOnsAttoXStatus();
	void restoreAddOnsAttoZStatus();
	void restoreAddOnsAttoYStatus();

	// Atto low level status

	void onOldRealAttoXStatusControlChanged();
	void onOldRealAttoZStatusControlChanged();
	void onOldRealAttoYStatusControlChanged();

	// Atto rotation Rx, Rz, Ry

	void onOldAttoRxSetpointControlChanged();
	void onOldAttoRxFeedbackControlChanged();

	void onOldAttoRzSetpointControlChanged();
	void onOldAttoRzFeedbackControlChanged();

	void onOldAttoRySetpointControlChanged();
	void onOldAttoRyFeedbackControlChanged();

	void onAddOnsAttoRxSetpointControlChanged();
	void onAddOnsAttoRzSetpointControlChanged();
	void onAddOnsAttoRySetpointControlChanged();

	void restoreAddOnsAttoRxStatus();
	void restoreAddOnsAttoRzStatus();
	void restoreAddOnsAttoRyStatus();

	// Atto rotation low level status

	void onOldRealAttoRxStatusControlChanged();
	void onOldRealAttoRzStatusControlChanged();
	void onOldRealAttoRyStatusControlChanged();

	// Wire H, V, N

	void onOldWireHSetpointControlChanged();
	void onOldWireHFeedbackControlChanged();

	void onOldWireVSetpointControlChanged();
	void onOldWireVFeedbackControlChanged();

	void onOldWireNSetpointControlChanged();
	void onOldWireNFeedbackControlChanged();

	void onOldRealWireXStatusControlChanged();
	void onOldRealWireYAndZStatusControlChanged();

	void onAddOnsWireHSetpointControlChanged();
	void onAddOnsWireVSetpointControlChanged();
	void onAddOnsWireNSetpointControlChanged();

	void restoreAddOnsWireHStatus();
	void restoreAddOnsWireVAndNStatus();

	// Mono

	void onMonoStatusChanged(bool isMoving);
	void onMonoTimerTimeout();

protected:
	/// Takes two independent motor statuses and returns a new status for the combined state.
	int computeFinalStatus(int status1, int status2) const;

	/// All the controls (for checking connectivity)
	AMControlSet *allControls_;
	/// Holds whether or not we've connected at least once
	bool connectedOnce_;

	// Sample H, V, N

	AMSinglePVControl *oldSampleHSetpointControl_;
	AMReadOnlyPVControl *oldSampleHFeedbackControl_;

	AMSinglePVControl *oldSampleVSetpointControl_;
	AMReadOnlyPVControl *oldSampleVFeedbackControl_;

	AMSinglePVControl *oldSampleNSetpointControl_;
	AMReadOnlyPVControl *oldSampleNFeedbackControl_;

	AMSinglePVControl *addOnsSampleHSetpointControl_;
	AMSinglePVControl *addOnsSampleHFeedbackControl_;
	AMSinglePVControl *addOnsSampleHStatusControl_;

	AMSinglePVControl *addOnsSampleVSetpointControl_;
	AMSinglePVControl *addOnsSampleVFeedbackControl_;
	AMSinglePVControl *addOnsSampleVStatusControl_;

	AMSinglePVControl *addOnsSampleNSetpointControl_;
	AMSinglePVControl *addOnsSampleNFeedbackControl_;
	AMSinglePVControl *addOnsSampleNStatusControl_;

	// Sample X, Z, Y

	AMSinglePVControl *oldSampleXSetpointControl_;
	AMReadOnlyPVControl *oldSampleXFeedbackControl_;

	AMSinglePVControl *oldSampleZSetpointControl_;
	AMReadOnlyPVControl *oldSampleZFeedbackControl_;

	AMSinglePVControl *oldSampleYSetpointControl_;
	AMReadOnlyPVControl *oldSampleYFeedbackControl_;

	AMSinglePVControl *addOnsSampleXSetpointControl_;
	AMSinglePVControl *addOnsSampleXFeedbackControl_;
	AMSinglePVControl *addOnsSampleXStatusControl_;

	AMSinglePVControl *addOnsSampleZSetpointControl_;
	AMSinglePVControl *addOnsSampleZFeedbackControl_;
	AMSinglePVControl *addOnsSampleZStatusControl_;

	AMSinglePVControl *addOnsSampleYSetpointControl_;
	AMSinglePVControl *addOnsSampleYFeedbackControl_;
	AMSinglePVControl *addOnsSampleYStatusControl_;

	// Sample low level status

	AMReadOnlyPVControl *oldRealSampleXStatusControl_;
	AMReadOnlyPVControl *oldRealSampleYStatusControl_;
	AMReadOnlyPVControl *oldRealSampleZStatusControl_;

	// Atto H, V, N

	AMSinglePVControl *oldAttoHSetpointControl_;
	AMReadOnlyPVControl *oldAttoHFeedbackControl_;

	AMSinglePVControl *oldAttoVSetpointControl_;
	AMReadOnlyPVControl *oldAttoVFeedbackControl_;

	AMSinglePVControl *oldAttoNSetpointControl_;
	AMReadOnlyPVControl *oldAttoNFeedbackControl_;

	AMSinglePVControl *addOnsAttoHSetpointControl_;
	AMSinglePVControl *addOnsAttoHFeedbackControl_;
	AMSinglePVControl *addOnsAttoHStatusControl_;

	AMSinglePVControl *addOnsAttoVSetpointControl_;
	AMSinglePVControl *addOnsAttoVFeedbackControl_;
	AMSinglePVControl *addOnsAttoVStatusControl_;

	AMSinglePVControl *addOnsAttoNSetpointControl_;
	AMSinglePVControl *addOnsAttoNFeedbackControl_;
	AMSinglePVControl *addOnsAttoNStatusControl_;

	// Atto X, Z, Y

	AMSinglePVControl *oldAttoXSetpointControl_;
	AMReadOnlyPVControl *oldAttoXFeedbackControl_;

	AMSinglePVControl *oldAttoZSetpointControl_;
	AMReadOnlyPVControl *oldAttoZFeedbackControl_;

	AMSinglePVControl *oldAttoYSetpointControl_;
	AMReadOnlyPVControl *oldAttoYFeedbackControl_;

	AMSinglePVControl *addOnsAttoXSetpointControl_;
	AMSinglePVControl *addOnsAttoXFeedbackControl_;
	AMSinglePVControl *addOnsAttoXStatusControl_;

	AMSinglePVControl *addOnsAttoZSetpointControl_;
	AMSinglePVControl *addOnsAttoZFeedbackControl_;
	AMSinglePVControl *addOnsAttoZStatusControl_;

	AMSinglePVControl *addOnsAttoYSetpointControl_;
	AMSinglePVControl *addOnsAttoYFeedbackControl_;
	AMSinglePVControl *addOnsAttoYStatusControl_;

	// Atto low level status

	AMReadOnlyPVControl *oldRealAttoXStatusControl_;
	AMReadOnlyPVControl *oldRealAttoYStatusControl_;
	AMReadOnlyPVControl *oldRealAttoZStatusControl_;

	// Atto rotation controls

	AMSinglePVControl *oldAttoRxSetpointControl_;
	AMReadOnlyPVControl *oldAttoRxFeedbackControl_;

	AMSinglePVControl *oldAttoRzSetpointControl_;
	AMReadOnlyPVControl *oldAttoRzFeedbackControl_;

	AMSinglePVControl *oldAttoRySetpointControl_;
	AMReadOnlyPVControl *oldAttoRyFeedbackControl_;

	AMSinglePVControl *addOnsAttoRxSetpointControl_;
	AMSinglePVControl *addOnsAttoRxFeedbackControl_;
	AMSinglePVControl *addOnsAttoRxStatusControl_;

	AMSinglePVControl *addOnsAttoRzSetpointControl_;
	AMSinglePVControl *addOnsAttoRzFeedbackControl_;
	AMSinglePVControl *addOnsAttoRzStatusControl_;

	AMSinglePVControl *addOnsAttoRySetpointControl_;
	AMSinglePVControl *addOnsAttoRyFeedbackControl_;
	AMSinglePVControl *addOnsAttoRyStatusControl_;

	// Atto rotation low level status

	AMReadOnlyPVControl *oldRealAttoRxStatusControl_;
	AMReadOnlyPVControl *oldRealAttoRyStatusControl_;
	AMReadOnlyPVControl *oldRealAttoRzStatusControl_;

	// Wire H, V, N

	AMSinglePVControl *oldWireHSetpointControl_;
	AMReadOnlyPVControl *oldWireHFeedbackControl_;

	AMSinglePVControl *oldWireVSetpointControl_;
	AMReadOnlyPVControl *oldWireVFeedbackControl_;

	AMSinglePVControl *oldWireNSetpointControl_;
	AMReadOnlyPVControl *oldWireNFeedbackControl_;

	AMReadOnlyPVControl *oldRealWireXStatusControl_;
	AMReadOnlyPVControl *oldRealWireYStatusControl_;
	AMReadOnlyPVControl *oldRealWireZStatusControl_;

	AMSinglePVControl *addOnsWireHSetpointControl_;
	AMSinglePVControl *addOnsWireHFeedbackControl_;
	AMSinglePVControl *addOnsWireHStatusControl_;

	AMSinglePVControl *addOnsWireVSetpointControl_;
	AMSinglePVControl *addOnsWireVFeedbackControl_;
	AMSinglePVControl *addOnsWireVStatusControl_;

	AMSinglePVControl *addOnsWireNSetpointControl_;
	AMSinglePVControl *addOnsWireNFeedbackControl_;
	AMSinglePVControl *addOnsWireNStatusControl_;

	// Mono
	AMReadOnlyPVwStatusControl *monoFeedbackControl_;
	AMSinglePVControl *monoMoveRelativeStepControl_;
	QTimer *monoWaitTimer_;
};

#endif // VESPERSAddOnsCoordinator_H
