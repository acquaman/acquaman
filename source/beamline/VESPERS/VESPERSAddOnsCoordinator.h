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

	void onOldRealSampleXStatusControlChanged();
	void onOldRealSampleYAndZStatusControlChanged();

	void onAddOnsSampleHSetpointControlChanged();
	void onAddOnsSampleVSetpointControlChanged();
	void onAddOnsSampleNSetpointControlChanged();

	void restoreAddOnsSampleHStatus();
	void restoreAddOnsSampleVAndNStatus();

	// Sample X, Z, Y

	// Atto H, V, N

	void onOldAttoHSetpointControlChanged();
	void onOldAttoHFeedbackControlChanged();

	void onOldAttoVSetpointControlChanged();
	void onOldAttoVFeedbackControlChanged();

	void onOldAttoNSetpointControlChanged();
	void onOldAttoNFeedbackControlChanged();

	void onOldRealAttoXStatusControlChanged();
	void onOldRealAttoYAndZStatusControlChanged();

	void onAddOnsAttoHSetpointControlChanged();
	void onAddOnsAttoVSetpointControlChanged();
	void onAddOnsAttoNSetpointControlChanged();

	void restoreAddOnsAttoHStatus();
	void restoreAddOnsAttoVAndNStatus();

	// Atto X, Z, Y

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

	AMReadOnlyPVControl *oldRealSampleXStatusControl_;
	AMReadOnlyPVControl *oldRealSampleYStatusControl_;
	AMReadOnlyPVControl *oldRealSampleZStatusControl_;

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

	// Atto H, V, N

	AMSinglePVControl *oldAttoHSetpointControl_;
	AMReadOnlyPVControl *oldAttoHFeedbackControl_;

	AMSinglePVControl *oldAttoVSetpointControl_;
	AMReadOnlyPVControl *oldAttoVFeedbackControl_;

	AMSinglePVControl *oldAttoNSetpointControl_;
	AMReadOnlyPVControl *oldAttoNFeedbackControl_;

	AMReadOnlyPVControl *oldRealAttoXStatusControl_;
	AMReadOnlyPVControl *oldRealAttoYStatusControl_;
	AMReadOnlyPVControl *oldRealAttoZStatusControl_;

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
};

#endif // VESPERSAddOnsCoordinator_H
