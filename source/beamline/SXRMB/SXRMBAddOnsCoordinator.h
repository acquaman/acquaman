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


#ifndef SXRMBAddOnsCoordinator_H
#define SXRMBAddOnsCoordinator_H

#include "beamline/AMControlSet.h"

class SXRMBAddOnsCoordinator : public QObject
{
Q_OBJECT
public:
	/// Simple constructor for class
	SXRMBAddOnsCoordinator(QObject *parent = 0);

	/// Destructor
	virtual ~SXRMBAddOnsCoordinator();

protected slots:
	/// Handles watching for when the controls actually connect
	void onAllControlsConnected(bool connected);

	/// Handles changes coming from the old energy control
	void onOldEnergyValueChanged(double value);
	/// Handles changes coming from the old energy feedback control
	void onOldEnergyFeedbackValueChanged(double value);
	/// Handles changes coming from the old energy status control
	void onOldEnergyStatusValueChanged(double value);

	/// Handles changes coming from the new addOns energy control
	void onAddOnsEnergyValueChanged(double value);
	/// Handles changes coming from the new addOns energy stop
	void onAddOnsEnergyStopValueChanged(double value);

	/// Used to set the addOns energy status to return to the old energy status value after faking a move
	void restoreEnergyStatus();

protected:


protected:
	/// Old SXRMB Energy control
	AMControl *oldEnergy_;
	/// New AddOns Energy control
	AMControl *addOnsEnergy_;

	/// Old SXRMB Energy feedback control
	AMControl *oldEnergyFeedback_;
	/// New AddOns Energy feedback control
	AMControl *addOnsEnergyFeedback_;

	/// Old SXRMB Energy status control
	AMControl *oldEnergyStatus_;
	/// New AddOns Energy status control
	AMControl *addOnsEnergyStatus_;

	/// SXRMB Mono Z2 stop control
	AMControl *monoZ2StopControl_;
	/// SXRMB Mono Y2 stop control
	AMControl *monoY2StopControl_;
	/// SXRMB Mono Theta stop control
	AMControl *monoThetaStopControl_;
	/// New AddOns energy stop control
	AMControl *addOnsEnergyStop_;

	/// All the controls (for checking connectivity)
	AMControlSet *allControls_;
	/// Holds whether or not we've connected at least once
	bool connectedOnce_;
};

#endif // SXRMBAddOnsCoordinator_H
