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

	/// Handles changes of Endstation
	void onEndstationValueChanged(double value);

	/// Handles changes of the oldCrystalSelection PV
	void onOldCrystalSelectionValueChanged();
	/// Handles changes of the oldCrystalSelectionFeedback PV
	void onOldCrystalSelectionFeedbackValueChanged();
	/// Handles changes of the addOnsCrystalSelection PV
	void onAddOnsCrystalSelectionValueChanged();

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

	/// Used to set the addOns energy status to the old energy status value after faking a move
	void restoreEnergyStatus();

	/// Handles changes coming from the old SXRMB microprobe sample stage X
	void onSXRMBMicroprobeSampleStageXValueChanged(double value);
	/// Handles changes coming from the old SXRMB microprobe sample stage Y
	void onSXRMBMicroprobeSampleStageYValueChanged(double value);
	/// Handles changes coming from the old SXRMB microprobe sample stage Z
	void onSXRMBMicroprobeSampleStageZValueChanged(double value);

	/// Handles changes coming from the old SXRMB microprobe sample stage X feedback
	void onSXRMBMicroprobeSampleStageFeedbackXValueChanged(double value);
	/// Handles changes coming from the old SXRMB microprobe sample stage Y feedback
	void onSXRMBMicroprobeSampleStageFeedbackYValueChanged(double value);
	/// Handles changes coming from the old SXRMB microprobe sample stage Z feedback
	void onSXRMBMicroprobeSampleStageFeedbackZValueChanged(double value);

	/// Handles changes coming from the old SXRMB microprobe sample stage X status
	void onSXRMBMicroprobeSampleStageStatusXValueChanged(double value);
	/// Handles changes coming from the old SXRMB microprobe sample stage Y status
	void onSXRMBMicroprobeSampleStageStatusYValueChanged(double value);
	/// Handles changes coming from the old SXRMB microprobe sample stage Z status
	void onSXRMBMicroprobeSampleStageStatusZValueChanged(double value);

	/// Handles changes coming from the new AddOns sample stage X
	void onAddOnsMicroprobeSampleStageXValueChanged(double value);
	/// Handles changes coming from the new AddOns sample stage Y
	void onAddOnsMicroprobeSampleStageYValueChanged(double value);
	/// Handles changes coming from the new AddOns sample stage Z
	void onAddOnsMicroprobeSampleStageZValueChanged(double value);

	/// Used to set the addOns sample stage X status to the old SXRMB sample stage X status value after faking a move
	void restoreAddOnsMicroprobeSampleStageStatusX();
	/// Used to set the addOns sample stage Y status to the old SXRMB sample stage X status value after faking a move
	void restoreAddOnsMicroprobeSampleStageStatusY();
	/// Used to set the addOns sample stage Z status to the old SXRMB sample stage X status value after faking a move
	void restoreAddOnsMicroprobeSampleStageStatusZ();

	/// Handles stop PV for Ambiant table height
	void onAddOnsAmbiantTableHeightStopValueChanged();

protected:


protected:
	/// New AddOns beamline endstation
	AMControl *addOnsEndstation_;

	/// Old SXRMB crystal selection
	AMControl *oldCrystalSelection_;
	/// Old SXRMB crystal selection feedback
	AMControl *oldCrystalSelectionFeedback_;
	/// New AddOns crystal selection
	AMControl *addOnsCrystalSelection_;
	/// New AddOns crystal selection feedback
	AMControl *addOnsCrystalSelectionFeedback_;

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

	/// Old SXRMB Energy tracking control
	AMControl *oldEnergyTracking_;

	/// New AddOns Energy DRVH control
	AMControl *addOnsEnergyDRVH_;
	/// New AddOns Energy DRVL control
	AMControl *addOnsEnergyDRVL_;

	/// SXRMB Mono Z2 stop control
	AMControl *monoZ2StopControl_;
	/// SXRMB Mono Y2 stop control
	AMControl *monoY2StopControl_;
	/// SXRMB Mono Theta stop control
	AMControl *monoThetaStopControl_;
	/// New AddOns energy stop control
	AMControl *addOnsEnergyStop_;

	/// Old SXRMB Microprobe Sample Stage X
	AMControl *sxrmbMicroprobeSampleStageX_;
	/// Old SXRMB Microprobe Sample Stage Y
	AMControl *sxrmbMicroprobeSampleStageY_;
	/// Old SXRMB Microprobe Sample Stage Z
	AMControl *sxrmbMicroprobeSampleStageZ_;

	/// New AddOns Microprobe Sample Stage X
	AMControl *addOnsMicroprobeSampleStageX_;
	/// New AddOns Microprobe Sample Stage Y
	AMControl *addOnsMicroprobeSampleStageY_;
	/// New AddOns Microprobe Sample Stage Z
	AMControl *addOnsMicroprobeSampleStageZ_;

	/// Old SXRMB Microprobe Sample Stage Feedback X
	AMControl *sxrmbMicroprobeSampleStageFeedbackX_;
	/// Old SXRMB Microprobe Sample Stage Feedback Y
	AMControl *sxrmbMicroprobeSampleStageFeedbackY_;
	/// Old SXRMB Microprobe Sample Stage Feedback Z
	AMControl *sxrmbMicroprobeSampleStageFeedbackZ_;

	/// New AddOns Microprobe Sample Stage Feedback X
	AMControl *addOnsMicroprobeSampleStageFeedbackX_;
	/// New AddOns Microprobe Sample Stage Feedback Y
	AMControl *addOnsMicroprobeSampleStageFeedbackY_;
	/// New AddOns Microprobe Sample Stage Feedback Z
	AMControl *addOnsMicroprobeSampleStageFeedbackZ_;

	/// Old SXRMB Microprobe Sample Stage Status X
	AMControl *sxrmbMicroprobeSampleStageStatusX_;
	/// Old SXRMB Microprobe Sample Stage Status Y
	AMControl *sxrmbMicroprobeSampleStageStatusY_;
	/// Old SXRMB Microprobe Sample Stage Status Z
	AMControl *sxrmbMicroprobeSampleStageStatusZ_;

	/// New AddOns Microprobe Sample Stage Status X
	AMControl *addOnsMicroprobeSampleStageStatusX_;
	/// New AddOns Microprobe Sample Stage Status Y
	AMControl *addOnsMicroprobeSampleStageStatusY_;
	/// New AddOns Microprobe Sample Stage Status Z
	AMControl *addOnsMicroprobeSampleStageStatusZ_;

	/// New AddOns Microprobe Sample Stage X DRVH
	AMControl *addOnsMicroprobeSampleStageDRVHX_;
	/// New AddOns Microprobe Sample Stage X DRVL
	AMControl *addOnsMicroprobeSampleStageDRVLX_;

	/// New AddOns Microprobe Sample Stage Y DRVH
	AMControl *addOnsMicroprobeSampleStageDRVHY_;
	/// New AddOns Microprobe Sample Stage Y DRVL
	AMControl *addOnsMicroprobeSampleStageDRVLY_;

	/// New AddOns Microprobe Sample Stage Z DRVH
	AMControl *addOnsMicroprobeSampleStageDRVHZ_;
	/// New AddOns Microprobe Sample Stage Z DRVL
	AMControl *addOnsMicroprobeSampleStageDRVLZ_;

	/// Original Motor PV stop for Ambiant endstation table upstream inbound
	AMControl *ambiantTableUpstreamInboundStopControl_;
	/// Original Motor PV stop for Ambiant endstation table upstream outbound
	AMControl *ambiantTableUpstreamOutboundStopControl_;
	/// Original Motor PV stop for Ambiant endstation table downstream
	AMControl *ambiantTableDownstreamStopControl_;
	/// New AddOns stop PV for Ambiant endstation table
	AMControl *addOnsAmbiantTableHeightStopControl_;

	/// All the controls (for checking connectivity)
	AMControlSet *allControls_;
	/// Holds whether or not we've connected at least once
	bool connectedOnce_;
};

#endif // SXRMBAddOnsCoordinator_H
