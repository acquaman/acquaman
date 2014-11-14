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

	// Atto X, Z, Y

	// Wire H, V, N

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

	// Atto X, Z, Y

	// Wire H, V, N
};

#endif // VESPERSAddOnsCoordinator_H
