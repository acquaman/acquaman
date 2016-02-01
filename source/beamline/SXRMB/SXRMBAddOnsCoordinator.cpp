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


#include "SXRMBAddOnsCoordinator.h"

#include "beamline/AMPVControl.h"

SXRMBAddOnsCoordinator::SXRMBAddOnsCoordinator(QObject *parent) :
	QObject(parent)
{
	connectedOnce_ = false;

	addOnsEndstation_ = new AMSinglePVControl("AddOnsEndstation", "BL1606-B1-1:AddOns:Endstation", this, 0.5);

	oldCrystalSelection_ = new AMSinglePVControl("OldCrystalSelection", "MONO1606-4-B10-01:CrysSel", this, 0.5);
	oldCrystalSelectionFeedback_ =  new AMSinglePVControl("OldCrystalSelectionFeedback", "MONO1606-4-B10-01:CrysSel:fbk", this, 0.5);
	addOnsCrystalSelection_ =  new AMSinglePVControl("AddOnsCrystalSelection", "BL1606-B1-1:AddOns:CrystalSelection", this, 0.5);
	addOnsCrystalSelectionFeedback_ =  new AMSinglePVControl("AddOnsCrystalSelectionFeedback", "BL1606-B1-1:AddOns:CrystalSelection:fbk", this, 0.5);

	oldEnergy_ =  new AMSinglePVControl("OldEnergy", "BL1606-B1-1:Energy", this, 0.01);
	oldEnergyFeedback_ = new AMReadOnlyPVControl("OldEnergyFeedback", "BL1606-B1-1:Energy:fbk", this);
	oldEnergyStatus_ = new AMReadOnlyPVControl("OldEnergyStatus", "BL1606-B1-1:Energy:status", this);
	oldEnergyTracking_ = new AMSinglePVControl("OldEnergyTracking", "MONO1606-4-B10-01:Energy:track", this, 0.5);

	monoZ2StopControl_ = new AMSinglePVControl("MonoZ2Stop", "SMTR1606-4-B10-11:stop", this, 0.5);
	monoY2StopControl_ = new AMSinglePVControl("MonoY2Stop", "SMTR1606-4-B10-12:stop", this, 0.5);
	monoThetaStopControl_ = new AMSinglePVControl("MonoThetaStop", "SMTR1606-4-B10-16:stop", this, 0.5);

	addOnsEnergy_ = new AMSinglePVControl("AddOnsEnergy", "BL1606-B1-1:AddOns:Energy", this, 0.001);
	addOnsEnergyFeedback_ = new AMSinglePVControl("AddOnsEnergyFeedback", "BL1606-B1-1:AddOns:Energy:fbk", this, 0.001);
	addOnsEnergyStatus_ = new AMSinglePVControl("AddOnsEnergyStatus", "BL1606-B1-1:AddOns:Energy:status", this, 0.5);
	addOnsEnergyStop_ = new AMSinglePVControl("AddOnsEnergyStop", "BL1606-B1-1:AddOns:Energy:stop", this, 0.5);

	addOnsEnergyDRVH_ = new AMSinglePVControl("AddOnsEnergyDRVH", "BL1606-B1-1:AddOns:Energy.DRVH", this, 0.001);
	addOnsEnergyDRVL_ = new AMSinglePVControl("AddOnsEnergyDRVL", "BL1606-B1-1:AddOns:Energy.DRVL", this, 0.001);

	sxrmbMicroprobeSampleStageX_ = new AMSinglePVControl("SXRMBMicroprobleSampleStageX", "SVM1606-5-B10-07:mm", this, 0.0001);
	sxrmbMicroprobeSampleStageY_ = new AMSinglePVControl("SXRMBMicroprobleSampleStageY", "SVM1606-5-B10-08:mm", this, 0.0001);
	sxrmbMicroprobeSampleStageZ_ = new AMSinglePVControl("SXRMBMicroprobleSampleStageZ", "SVM1606-5-B10-09:mm", this, 0.0001);

	sxrmbMicroprobeSampleStageFeedbackX_ = new AMSinglePVControl("SXRMBMicroprobleSampleStageFeedbackX", "SVM1606-5-B10-07:mm:sp", this, 0.0001);
	sxrmbMicroprobeSampleStageFeedbackY_ = new AMSinglePVControl("SXRMBMicroprobleSampleStageFeedbackY", "SVM1606-5-B10-08:mm:sp", this, 0.0001);
	sxrmbMicroprobeSampleStageFeedbackZ_ = new AMSinglePVControl("SXRMBMicroprobleSampleStageFeedbackZ", "SVM1606-5-B10-09:mm:sp", this, 0.0001);

	sxrmbMicroprobeSampleStageStatusX_ = new AMSinglePVControl("SXRMBMicroprobleSampleStageStatusX", "SVM1606-5-B10-07:status", this, 0.5);
	sxrmbMicroprobeSampleStageStatusY_ = new AMSinglePVControl("SXRMBMicroprobleSampleStageStatusY", "SVM1606-5-B10-08:status", this, 0.5);
	sxrmbMicroprobeSampleStageStatusZ_ = new AMSinglePVControl("SXRMBMicroprobleSampleStageStatusZ", "SVM1606-5-B10-09:status", this, 0.5);

	addOnsMicroprobeSampleStageX_ = new AMSinglePVControl("AddOnsMicroprobleSampleStageX", "BL1606-B1-1:AddOns:uProbe:SampleStage:X:mm", this, 0.0001);
	addOnsMicroprobeSampleStageY_ = new AMSinglePVControl("AddOnsMicroprobleSampleStageY", "BL1606-B1-1:AddOns:uProbe:SampleStage:Y:mm", this, 0.0001);
	addOnsMicroprobeSampleStageZ_ = new AMSinglePVControl("AddOnsMicroprobleSampleStageZ", "BL1606-B1-1:AddOns:uProbe:SampleStage:Z:mm", this, 0.0001);

	addOnsMicroprobeSampleStageFeedbackX_ = new AMSinglePVControl("AddOnsMicroprobleSampleStageFeedbackX", "BL1606-B1-1:AddOns:uProbe:SampleStage:X:mm:fbk", this, 0.0001);
	addOnsMicroprobeSampleStageFeedbackY_ = new AMSinglePVControl("AddOnsMicroprobleSampleStageFeedbackY", "BL1606-B1-1:AddOns:uProbe:SampleStage:Y:mm:fbk", this, 0.0001);
	addOnsMicroprobeSampleStageFeedbackZ_ = new AMSinglePVControl("AddOnsMicroprobleSampleStageFeedbackZ", "BL1606-B1-1:AddOns:uProbe:SampleStage:Z:mm:fbk", this, 0.0001);

	addOnsMicroprobeSampleStageStatusX_ = new AMSinglePVControl("AddOnsMicroprobleSampleStageStatusX", "BL1606-B1-1:AddOns:uProbe:SampleStage:X:status", this, 0.5);
	addOnsMicroprobeSampleStageStatusY_ = new AMSinglePVControl("AddOnsMicroprobleSampleStageStatusY", "BL1606-B1-1:AddOns:uProbe:SampleStage:Y:status", this, 0.5);
	addOnsMicroprobeSampleStageStatusZ_ = new AMSinglePVControl("AddOnsMicroprobleSampleStageStatusZ", "BL1606-B1-1:AddOns:uProbe:SampleStage:Z:status", this, 0.5);

	addOnsMicroprobeSampleStageDRVHX_ = new AMSinglePVControl("AddOnsMicroprobleSampleStageDRVHX", "BL1606-B1-1:AddOns:uProbe:SampleStage:X:mm.DRVH", this, 0.0001);
	addOnsMicroprobeSampleStageDRVLX_ = new AMSinglePVControl("AddOnsMicroprobleSampleStageDRVLX", "BL1606-B1-1:AddOns:uProbe:SampleStage:X:mm.DRVL", this, 0.0001);

	addOnsMicroprobeSampleStageDRVHY_ = new AMSinglePVControl("AddOnsMicroprobleSampleStageDRVHY", "BL1606-B1-1:AddOns:uProbe:SampleStage:Y:mm.DRVH", this, 0.0001);
	addOnsMicroprobeSampleStageDRVLY_ = new AMSinglePVControl("AddOnsMicroprobleSampleStageDRVLY", "BL1606-B1-1:AddOns:uProbe:SampleStage:Y:mm.DRVL", this, 0.0001);

	addOnsMicroprobeSampleStageDRVHZ_ = new AMSinglePVControl("AddOnsMicroprobleSampleStageDRVHZ", "BL1606-B1-1:AddOns:uProbe:SampleStage:Z:mm.DRVH", this, 0.0001);
	addOnsMicroprobeSampleStageDRVLZ_ = new AMSinglePVControl("AddOnsMicroprobleSampleStageDRVLZ", "BL1606-B1-1:AddOns:uProbe:SampleStage:Z:mm.DRVL", this, 0.0001);

	ambiantTableUpstreamInboundStopControl_ = new AMSinglePVControl("AmbiantTableUpstreamInboundStop", "SMTR0000-E07-01:stop", this, 0.5);
	ambiantTableUpstreamOutboundStopControl_ = new AMSinglePVControl("AmbiantTableUpstreamOutboundStop", "SMTR0000-E07-02:stop", this, 0.5);
	ambiantTableDownstreamStopControl_ = new AMSinglePVControl("AmbiantTableDownstreamInboundStop", "SMTR0000-E07-03:stop", this, 0.5);
	addOnsAmbiantTableHeightStopControl_ = new AMSinglePVControl("AddOnsAmbiantTableHeightStop", "BL1606-B1-1:AddOns:Ambiant:TableHeight:stop", this, 0.5);

	allControls_ = new AMControlSet(this);
	allControls_->addControl(addOnsEndstation_);
	allControls_->addControl(oldCrystalSelection_);
	allControls_->addControl(oldCrystalSelectionFeedback_);
	allControls_->addControl(addOnsCrystalSelection_);
	allControls_->addControl(addOnsCrystalSelectionFeedback_);
	allControls_->addControl(oldEnergy_);
	allControls_->addControl(oldEnergyFeedback_);
	allControls_->addControl(oldEnergyStatus_);
	allControls_->addControl(oldEnergyTracking_);
	allControls_->addControl(monoZ2StopControl_);
	allControls_->addControl(monoY2StopControl_);
	allControls_->addControl(monoThetaStopControl_);
	allControls_->addControl(addOnsEnergy_);
	allControls_->addControl(addOnsEnergyFeedback_);
	allControls_->addControl(addOnsEnergyStatus_);
	allControls_->addControl(addOnsEnergyStop_);
	allControls_->addControl(addOnsEnergyDRVH_);
	allControls_->addControl(addOnsEnergyDRVL_);
	allControls_->addControl(sxrmbMicroprobeSampleStageX_);
	allControls_->addControl(sxrmbMicroprobeSampleStageY_);
	allControls_->addControl(sxrmbMicroprobeSampleStageZ_);
	allControls_->addControl(sxrmbMicroprobeSampleStageFeedbackX_);
	allControls_->addControl(sxrmbMicroprobeSampleStageFeedbackY_);
	allControls_->addControl(sxrmbMicroprobeSampleStageFeedbackZ_);
	allControls_->addControl(sxrmbMicroprobeSampleStageStatusX_);
	allControls_->addControl(sxrmbMicroprobeSampleStageStatusY_);
	allControls_->addControl(sxrmbMicroprobeSampleStageStatusZ_);
	allControls_->addControl(addOnsMicroprobeSampleStageX_);
	allControls_->addControl(addOnsMicroprobeSampleStageY_);
	allControls_->addControl(addOnsMicroprobeSampleStageZ_);
	allControls_->addControl(addOnsMicroprobeSampleStageFeedbackX_);
	allControls_->addControl(addOnsMicroprobeSampleStageFeedbackY_);
	allControls_->addControl(addOnsMicroprobeSampleStageFeedbackZ_);
	allControls_->addControl(addOnsMicroprobeSampleStageStatusX_);
	allControls_->addControl(addOnsMicroprobeSampleStageStatusY_);
	allControls_->addControl(addOnsMicroprobeSampleStageStatusZ_);
	allControls_->addControl(addOnsMicroprobeSampleStageDRVHX_);
	allControls_->addControl(addOnsMicroprobeSampleStageDRVLX_);
	allControls_->addControl(addOnsMicroprobeSampleStageDRVHY_);
	allControls_->addControl(addOnsMicroprobeSampleStageDRVLY_);
	allControls_->addControl(addOnsMicroprobeSampleStageDRVHZ_);
	allControls_->addControl(addOnsMicroprobeSampleStageDRVLZ_);
	allControls_->addControl(ambiantTableUpstreamInboundStopControl_);
	allControls_->addControl(ambiantTableUpstreamOutboundStopControl_);
	allControls_->addControl(ambiantTableDownstreamStopControl_);
	allControls_->addControl(addOnsAmbiantTableHeightStopControl_);

	connect(allControls_, SIGNAL(connected(bool)), this, SLOT(onAllControlsConnected(bool)));

	connect(addOnsEndstation_, SIGNAL(valueChanged(double)), this, SLOT(onEndstationValueChanged(double)));

	connect(oldCrystalSelection_, SIGNAL(valueChanged(double)), this, SLOT(onOldCrystalSelectionValueChanged()));
	connect(oldCrystalSelectionFeedback_, SIGNAL(valueChanged(double)), this, SLOT(onOldCrystalSelectionFeedbackValueChanged()));
	connect(addOnsCrystalSelection_, SIGNAL(valueChanged(double)), this, SLOT(onAddOnsCrystalSelectionValueChanged()));

	connect(oldEnergy_, SIGNAL(valueChanged(double)), this, SLOT(onOldEnergyValueChanged(double)));
	connect(oldEnergyFeedback_, SIGNAL(valueChanged(double)), this, SLOT(onOldEnergyFeedbackValueChanged(double)));
	connect(oldEnergyStatus_, SIGNAL(valueChanged(double)), this, SLOT(onOldEnergyStatusValueChanged(double)));

	connect(addOnsEnergy_, SIGNAL(valueChanged(double)), this, SLOT(onAddOnsEnergyValueChanged(double)));
	connect(addOnsEnergyStop_, SIGNAL(valueChanged(double)), this, SLOT(onAddOnsEnergyStopValueChanged(double)));

	connect(sxrmbMicroprobeSampleStageX_, SIGNAL(valueChanged(double)), this, SLOT(onSXRMBMicroprobeSampleStageXValueChanged(double)));
	connect(sxrmbMicroprobeSampleStageY_, SIGNAL(valueChanged(double)), this, SLOT(onSXRMBMicroprobeSampleStageYValueChanged(double)));
	connect(sxrmbMicroprobeSampleStageZ_, SIGNAL(valueChanged(double)), this, SLOT(onSXRMBMicroprobeSampleStageZValueChanged(double)));

	connect(sxrmbMicroprobeSampleStageFeedbackX_, SIGNAL(valueChanged(double)), this, SLOT(onSXRMBMicroprobeSampleStageFeedbackXValueChanged(double)));
	connect(sxrmbMicroprobeSampleStageFeedbackY_, SIGNAL(valueChanged(double)), this, SLOT(onSXRMBMicroprobeSampleStageFeedbackYValueChanged(double)));
	connect(sxrmbMicroprobeSampleStageFeedbackZ_, SIGNAL(valueChanged(double)), this, SLOT(onSXRMBMicroprobeSampleStageFeedbackZValueChanged(double)));

	connect(sxrmbMicroprobeSampleStageStatusX_, SIGNAL(valueChanged(double)), this, SLOT(onSXRMBMicroprobeSampleStageStatusXValueChanged(double)));
	connect(sxrmbMicroprobeSampleStageStatusY_, SIGNAL(valueChanged(double)), this, SLOT(onSXRMBMicroprobeSampleStageStatusYValueChanged(double)));
	connect(sxrmbMicroprobeSampleStageStatusZ_, SIGNAL(valueChanged(double)), this, SLOT(onSXRMBMicroprobeSampleStageStatusZValueChanged(double)));

	connect(addOnsMicroprobeSampleStageX_, SIGNAL(valueChanged(double)), this, SLOT(onAddOnsMicroprobeSampleStageXValueChanged(double)));
	connect(addOnsMicroprobeSampleStageY_, SIGNAL(valueChanged(double)), this, SLOT(onAddOnsMicroprobeSampleStageYValueChanged(double)));
	connect(addOnsMicroprobeSampleStageZ_, SIGNAL(valueChanged(double)), this, SLOT(onAddOnsMicroprobeSampleStageZValueChanged(double)));

	connect(addOnsAmbiantTableHeightStopControl_, SIGNAL(valueChanged(double)), this, SLOT(onAddOnsAmbiantTableHeightStopValueChanged()));
}

SXRMBAddOnsCoordinator::~SXRMBAddOnsCoordinator(){}

void SXRMBAddOnsCoordinator::onAllControlsConnected(bool connected){
	if(connected){
		connectedOnce_ = true;

		qDebug() << "Checking start up value for Endstation: " << addOnsEndstation_->value();

		qDebug() << "Checking start up value for crystal Selection: " << oldCrystalSelection_->value();
		onOldCrystalSelectionValueChanged();
		qDebug() << "Checking start up value for crystal Selection Feedback: " << oldCrystalSelectionFeedback_->value();
		onOldCrystalSelectionFeedbackValueChanged();

		qDebug() << "Checking start up value from the OLD energy as " << oldEnergy_->value();
		onOldEnergyValueChanged(oldEnergy_->value());
		qDebug() << "Checking start up value from the OLD energy feedback as " << oldEnergyFeedback_->value();
		onOldEnergyFeedbackValueChanged(oldEnergyFeedback_->value());
		qDebug() << "Checking start up value from the OLD energy status as " << oldEnergyStatus_->value();
		onOldEnergyStatusValueChanged(oldEnergyStatus_->value());

		qDebug() << "Checking start up value from the OLD sample stage X as " << sxrmbMicroprobeSampleStageX_->value();
		onSXRMBMicroprobeSampleStageXValueChanged(sxrmbMicroprobeSampleStageX_->value());
		qDebug() << "Checking start up value from the OLD sample stage X feedback as " << sxrmbMicroprobeSampleStageFeedbackX_->value();
		onSXRMBMicroprobeSampleStageFeedbackXValueChanged(sxrmbMicroprobeSampleStageFeedbackX_->value());
		qDebug() << "Checking start up value from the OLD sample stage X status as " << sxrmbMicroprobeSampleStageStatusX_->value();
		onSXRMBMicroprobeSampleStageStatusXValueChanged(sxrmbMicroprobeSampleStageStatusX_->value());

		qDebug() << "Checking start up value from the OLD sample stage Y as " << sxrmbMicroprobeSampleStageY_->value();
		onSXRMBMicroprobeSampleStageYValueChanged(sxrmbMicroprobeSampleStageY_->value());
		qDebug() << "Checking start up value from the OLD sample stage Y feedback as " << sxrmbMicroprobeSampleStageFeedbackY_->value();
		onSXRMBMicroprobeSampleStageFeedbackYValueChanged(sxrmbMicroprobeSampleStageFeedbackY_->value());
		qDebug() << "Checking start up value from the OLD sample stage Y status as " << sxrmbMicroprobeSampleStageStatusY_->value();
		onSXRMBMicroprobeSampleStageStatusYValueChanged(sxrmbMicroprobeSampleStageStatusY_->value());

		qDebug() << "Checking start up value from the OLD sample stage Z as " << sxrmbMicroprobeSampleStageZ_->value();
		onSXRMBMicroprobeSampleStageZValueChanged(sxrmbMicroprobeSampleStageZ_->value());
		qDebug() << "Checking start up value from the OLD sample stage Z feedback as " << sxrmbMicroprobeSampleStageFeedbackZ_->value();
		onSXRMBMicroprobeSampleStageFeedbackZValueChanged(sxrmbMicroprobeSampleStageFeedbackZ_->value());
		qDebug() << "Checking start up value from the OLD sample stage Z status as " << sxrmbMicroprobeSampleStageStatusZ_->value();
		onSXRMBMicroprobeSampleStageStatusZValueChanged(sxrmbMicroprobeSampleStageStatusZ_->value());

		qDebug() << "Checking start up range for OLD energy as " << oldEnergy_->minimumValue() << oldEnergy_->maximumValue();
		addOnsEnergyDRVH_->move(oldEnergy_->maximumValue());
		addOnsEnergyDRVL_->move(oldEnergy_->minimumValue());

		qDebug() << "Checking start up range for OLD sample X as " << sxrmbMicroprobeSampleStageX_->minimumValue() << sxrmbMicroprobeSampleStageX_->maximumValue();
		addOnsMicroprobeSampleStageDRVHX_->move(sxrmbMicroprobeSampleStageX_->maximumValue());
		addOnsMicroprobeSampleStageDRVLX_->move(sxrmbMicroprobeSampleStageX_->minimumValue());

		qDebug() << "Checking start up range for OLD sample Y as " << sxrmbMicroprobeSampleStageY_->minimumValue() << sxrmbMicroprobeSampleStageY_->maximumValue();
		addOnsMicroprobeSampleStageDRVHY_->move(sxrmbMicroprobeSampleStageY_->maximumValue());
		addOnsMicroprobeSampleStageDRVLY_->move(sxrmbMicroprobeSampleStageY_->minimumValue());

		qDebug() << "Checking start up range for OLD sample Z as " << sxrmbMicroprobeSampleStageZ_->minimumValue() << sxrmbMicroprobeSampleStageZ_->maximumValue();
		addOnsMicroprobeSampleStageDRVHZ_->move(sxrmbMicroprobeSampleStageZ_->maximumValue());
		addOnsMicroprobeSampleStageDRVLZ_->move(sxrmbMicroprobeSampleStageZ_->minimumValue());
	}
}

void SXRMBAddOnsCoordinator::onEndstationValueChanged(double value){
	Q_UNUSED(value)
	if(!connectedOnce_)
		return;

	qDebug() << "Detected Endstation change to " << addOnsEndstation_->value();
}

void SXRMBAddOnsCoordinator::onOldCrystalSelectionValueChanged()
{
	if (!connectedOnce_)
		return;

	qDebug() << "Detected OLD crystal selection move requested for " << oldCrystalSelection_->value() << " versus " << addOnsCrystalSelection_->value();

	if(!addOnsCrystalSelection_->withinTolerance(oldCrystalSelection_->value()))
		addOnsCrystalSelection_->move(oldCrystalSelection_->value());
}

///
/// Crystal Selection Feedback value mappings are
/// Old					addOns
/// [ 0] Unknown        [ 3] Unknown
/// [ 1] InSb (111)     [ 0] InSb (111)
/// [ 2] Si (111)       [ 1] Si (111)
/// [ 3] In Between     [ 2] In Between
///
void SXRMBAddOnsCoordinator::onOldCrystalSelectionFeedbackValueChanged()
{
	if (!connectedOnce_)
		return;

	int oldCrystalSelectionFeedbackValue = int(oldCrystalSelectionFeedback_->value());
	int mappedCrystalSelectionFeedbackValue = (oldCrystalSelectionFeedbackValue + 3) % 4;

	qDebug() << QString("Detected OLD crystal selection feedback move requested for %1 (mapped to %2) versus %3").arg(oldCrystalSelectionFeedbackValue).arg(mappedCrystalSelectionFeedbackValue).arg(addOnsCrystalSelectionFeedback_->value());

	if(!addOnsCrystalSelectionFeedback_->withinTolerance(mappedCrystalSelectionFeedbackValue))
		addOnsCrystalSelectionFeedback_->move(mappedCrystalSelectionFeedbackValue);
}

void SXRMBAddOnsCoordinator::onAddOnsCrystalSelectionValueChanged()
{
	if (!connectedOnce_)
		return;

	qDebug() << "Detected addOns crystal selection move requested for " << addOnsCrystalSelection_->value() << " versus " << oldCrystalSelection_->value();

	if(!oldCrystalSelection_->withinTolerance(addOnsCrystalSelection_->value()))
		oldCrystalSelection_->move(addOnsCrystalSelection_->value());
}

void SXRMBAddOnsCoordinator::onOldEnergyValueChanged(double value){
	Q_UNUSED(value)
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD energy move requested for " << oldEnergy_->value() << " versus " << addOnsEnergy_->value();

	if(!addOnsEnergy_->withinTolerance(oldEnergy_->value()) && oldEnergyTracking_->withinTolerance(1))
		addOnsEnergy_->move(oldEnergy_->value());
}

void SXRMBAddOnsCoordinator::onOldEnergyFeedbackValueChanged(double value){
	Q_UNUSED(value)
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD energy feedback move of " << oldEnergyFeedback_->value() << " versus " << addOnsEnergyFeedback_->value();

	if(!addOnsEnergyFeedback_->withinTolerance(oldEnergyFeedback_->value()))
		addOnsEnergyFeedback_->move(oldEnergyFeedback_->value());
}

void SXRMBAddOnsCoordinator::onOldEnergyStatusValueChanged(double value){
	Q_UNUSED(value)
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD energy status change to " << oldEnergyStatus_->value() << " versus " << addOnsEnergyStatus_->value();
	if(!addOnsEnergyStatus_->withinTolerance(oldEnergyStatus_->value()))
		addOnsEnergyStatus_->move(oldEnergyStatus_->value());
}

void SXRMBAddOnsCoordinator::onAddOnsEnergyValueChanged(double value){
	Q_UNUSED(value)
	if(!connectedOnce_)
		return;

	qDebug() << "Detected AddOns energy move requested for " << addOnsEnergy_->value() << " versus " << oldEnergy_->value() << " with OLD feedback " << oldEnergyFeedback_->value() << " and AddOns feedback " << addOnsEnergyFeedback_->value() << " with old status" << oldEnergyStatus_->value();

	// If we're requesting the same energy with AddOns as was in the old energy AND the old energy feedback matches the old energy setpoint AND the status is MOVE DONE
	// THEN issuing a move will not do anything SO we'll trick the status to say "MOVE ACTIVE" then "MOVE DONE"
	if(oldEnergy_->withinTolerance(addOnsEnergy_->value()) && oldEnergy_->withinTolerance(oldEnergyFeedback_->value()) && oldEnergyStatus_->withinTolerance(0.0)){
		qDebug() << "Faking an energy move";
		addOnsEnergyStatus_->move(1.0);
		QTimer::singleShot(100, this, SLOT(restoreEnergyStatus()));
		return;
	}

	// If we're requesting the same energy with AddOns as was in the old energy AND the old energy doesn't agree with the old energy feedback AND the status isn't moving
	// THEN we got genked somehow, and we're not going to move even though we really should SO we'll trick the old energy by moving to a value that's a fraction away from it's last value
	if(oldEnergy_->withinTolerance(addOnsEnergy_->value()) && !oldEnergy_->withinTolerance(oldEnergyFeedback_->value()) && !oldEnergyStatus_->withinTolerance(1.0)){
		qDebug() << "Having to do fractional delta move to get out of bad situation";
		oldEnergy_->move(oldEnergy_->value()+0.0000001);
		return;
	}

	if(!oldEnergy_->withinTolerance(addOnsEnergy_->value())){
		qDebug() << "Actually requesting an energy move in the regular way";
		oldEnergy_->move(addOnsEnergy_->value());
	}
}

void SXRMBAddOnsCoordinator::onAddOnsEnergyStopValueChanged(double value){
	Q_UNUSED(value)
	if(!connectedOnce_)
		return;

	qDebug() << "A request to stop energy was heard from the addOnsEnergyStop";

	if(addOnsEnergyStop_->withinTolerance(1.0)){
		monoZ2StopControl_->move(1.0);
		monoY2StopControl_->move(1.0);
		monoThetaStopControl_->move(1.0);
	}
}

void SXRMBAddOnsCoordinator::restoreEnergyStatus(){
	qDebug() << "Restoring AddOns energy status to old energy status with AddOns " << addOnsEnergyStatus_->value() << " and old energy status " << oldEnergyStatus_->value();

	if(!addOnsEnergyStatus_->withinTolerance(oldEnergyStatus_->value()))
		addOnsEnergyStatus_->move(oldEnergyStatus_->value());
}


void SXRMBAddOnsCoordinator::onSXRMBMicroprobeSampleStageXValueChanged(double value){
	Q_UNUSED(value)
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD microprobe stage X move of " << sxrmbMicroprobeSampleStageX_->value() << " versus " << addOnsMicroprobeSampleStageX_->value();

	if(!addOnsMicroprobeSampleStageX_->withinTolerance(sxrmbMicroprobeSampleStageX_->value()))
		addOnsMicroprobeSampleStageX_->move(sxrmbMicroprobeSampleStageX_->value());
}

void SXRMBAddOnsCoordinator::onSXRMBMicroprobeSampleStageYValueChanged(double value){
	Q_UNUSED(value)
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD microprobe stage Y move of " << sxrmbMicroprobeSampleStageY_->value() << " versus " << addOnsMicroprobeSampleStageY_->value();

	if(!addOnsMicroprobeSampleStageY_->withinTolerance(sxrmbMicroprobeSampleStageY_->value()))
		addOnsMicroprobeSampleStageY_->move(sxrmbMicroprobeSampleStageY_->value());
}

void SXRMBAddOnsCoordinator::onSXRMBMicroprobeSampleStageZValueChanged(double value){
	Q_UNUSED(value)
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD microprobe stage Z move of " << sxrmbMicroprobeSampleStageZ_->value() << " versus " << addOnsMicroprobeSampleStageZ_->value();

	if(!addOnsMicroprobeSampleStageZ_->withinTolerance(sxrmbMicroprobeSampleStageZ_->value()))
		addOnsMicroprobeSampleStageZ_->move(sxrmbMicroprobeSampleStageZ_->value());
}

void SXRMBAddOnsCoordinator::onSXRMBMicroprobeSampleStageFeedbackXValueChanged(double value){
	Q_UNUSED(value)
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD microprobe stage X feedback move of " << sxrmbMicroprobeSampleStageFeedbackX_->value() << " versus " << addOnsMicroprobeSampleStageFeedbackX_->value();

	if(!addOnsMicroprobeSampleStageFeedbackX_->withinTolerance(sxrmbMicroprobeSampleStageFeedbackX_->value()))
		addOnsMicroprobeSampleStageFeedbackX_->move(sxrmbMicroprobeSampleStageFeedbackX_->value());
}

void SXRMBAddOnsCoordinator::onSXRMBMicroprobeSampleStageFeedbackYValueChanged(double value){
	Q_UNUSED(value)
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD microprobe stage Y feedback move of " << sxrmbMicroprobeSampleStageFeedbackY_->value() << " versus " << addOnsMicroprobeSampleStageFeedbackY_->value();

	if(!addOnsMicroprobeSampleStageFeedbackY_->withinTolerance(sxrmbMicroprobeSampleStageFeedbackY_->value()))
		addOnsMicroprobeSampleStageFeedbackY_->move(sxrmbMicroprobeSampleStageFeedbackY_->value());
}

void SXRMBAddOnsCoordinator::onSXRMBMicroprobeSampleStageFeedbackZValueChanged(double value){
	Q_UNUSED(value)
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD microprobe stage Z feedback move of " << sxrmbMicroprobeSampleStageFeedbackZ_->value() << " versus " << addOnsMicroprobeSampleStageFeedbackZ_->value();

	if(!addOnsMicroprobeSampleStageFeedbackZ_->withinTolerance(sxrmbMicroprobeSampleStageFeedbackZ_->value()))
		addOnsMicroprobeSampleStageFeedbackZ_->move(sxrmbMicroprobeSampleStageFeedbackZ_->value());
}

void SXRMBAddOnsCoordinator::onSXRMBMicroprobeSampleStageStatusXValueChanged(double value){
	Q_UNUSED(value)
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD microprobe stage X status move of " << sxrmbMicroprobeSampleStageStatusX_->value() << " versus " << addOnsMicroprobeSampleStageStatusX_->value();

	if(!addOnsMicroprobeSampleStageStatusX_->withinTolerance(sxrmbMicroprobeSampleStageStatusX_->value()))
		addOnsMicroprobeSampleStageStatusX_->move(sxrmbMicroprobeSampleStageStatusX_->value());
}

void SXRMBAddOnsCoordinator::onSXRMBMicroprobeSampleStageStatusYValueChanged(double value){
	Q_UNUSED(value)
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD microprobe stage Y status move of " << sxrmbMicroprobeSampleStageStatusY_->value() << " versus " << addOnsMicroprobeSampleStageStatusY_->value();

	if(!addOnsMicroprobeSampleStageStatusY_->withinTolerance(sxrmbMicroprobeSampleStageStatusY_->value()))
		addOnsMicroprobeSampleStageStatusY_->move(sxrmbMicroprobeSampleStageStatusY_->value());
}

void SXRMBAddOnsCoordinator::onSXRMBMicroprobeSampleStageStatusZValueChanged(double value){
	Q_UNUSED(value)
	if(!connectedOnce_)
		return;

	qDebug() << "Detected OLD microprobe stage Z status move of " << sxrmbMicroprobeSampleStageStatusZ_->value() << " versus " << addOnsMicroprobeSampleStageStatusZ_->value();

	if(!addOnsMicroprobeSampleStageStatusZ_->withinTolerance(sxrmbMicroprobeSampleStageStatusZ_->value()))
		addOnsMicroprobeSampleStageStatusZ_->move(sxrmbMicroprobeSampleStageStatusZ_->value());
}

void SXRMBAddOnsCoordinator::onAddOnsMicroprobeSampleStageXValueChanged(double value){
	Q_UNUSED(value)
	if(!connectedOnce_)
		return;

	qDebug() << "Detected new AddOns microprobe stage X move of " << addOnsMicroprobeSampleStageX_->value() << " versus " << sxrmbMicroprobeSampleStageX_->value();

	// If we're requesting the same stage X with AddOns as was in the old stage X AND the old stage X feedback matches the old stage X setpoint AND the status is MOVE DONE
	// THEN issuing a move will not do anything SO we'll trick the status to say "MOVE ACTIVE" then "MOVE DONE"
	if(sxrmbMicroprobeSampleStageX_->withinTolerance(addOnsMicroprobeSampleStageX_->value()) && sxrmbMicroprobeSampleStageX_->withinTolerance(sxrmbMicroprobeSampleStageFeedbackX_->value()) && sxrmbMicroprobeSampleStageStatusX_->withinTolerance(0.0)){
		qDebug() << "Faking a sample stage X move";
		addOnsMicroprobeSampleStageStatusX_->move(1.0);
		QTimer::singleShot(100, this, SLOT(restoreAddOnsMicroprobeSampleStageStatusX()));
		return;
	}

	if(!sxrmbMicroprobeSampleStageX_->withinTolerance(addOnsMicroprobeSampleStageX_->value())){
		qDebug() << "Doing addOns Sample Stage X move in the regular way";
		sxrmbMicroprobeSampleStageX_->move(addOnsMicroprobeSampleStageX_->value());
	}
}

void SXRMBAddOnsCoordinator::onAddOnsMicroprobeSampleStageYValueChanged(double value){
	Q_UNUSED(value)
	if(!connectedOnce_)
		return;

	qDebug() << "Detected new AddOns microprobe stage Y move of " << addOnsMicroprobeSampleStageY_->value() << " versus " << sxrmbMicroprobeSampleStageY_->value();

	// If we're requesting the same stage Y with AddOns as was in the old stage Y AND the old stage Y feedback matches the old stage Y setpoint AND the status is MOVE DONE
	// THEN issuing a move will not do anything SO we'll trick the status to say "MOVE ACTIVE" then "MOVE DONE"
	if(sxrmbMicroprobeSampleStageY_->withinTolerance(addOnsMicroprobeSampleStageY_->value()) && sxrmbMicroprobeSampleStageY_->withinTolerance(sxrmbMicroprobeSampleStageFeedbackY_->value()) && sxrmbMicroprobeSampleStageStatusY_->withinTolerance(0.0)){
		qDebug() << "Faking a sample stage Y move";
		addOnsMicroprobeSampleStageStatusY_->move(1.0);
		QTimer::singleShot(100, this, SLOT(restoreAddOnsMicroprobeSampleStageStatusY()));
		return;
	}

	if(!sxrmbMicroprobeSampleStageY_->withinTolerance(addOnsMicroprobeSampleStageY_->value())){
		qDebug() << "Doing addOns Sample Stage Y move in the regular way";
		sxrmbMicroprobeSampleStageY_->move(addOnsMicroprobeSampleStageY_->value());
	}
}

void SXRMBAddOnsCoordinator::onAddOnsMicroprobeSampleStageZValueChanged(double value){
	Q_UNUSED(value)
	if(!connectedOnce_)
		return;

	qDebug() << "Detected new AddOns microprobe stage Z move of " << addOnsMicroprobeSampleStageZ_->value() << " versus " << sxrmbMicroprobeSampleStageZ_->value();

	// If we're requesting the same stage Z with AddOns as was in the old stage Z AND the old stage Z feedback matches the old stage Z setpoint AND the status is MOVE DONE
	// THEN issuing a move will not do anything SO we'll trick the status to say "MOVE ACTIVE" then "MOVE DONE"
	if(sxrmbMicroprobeSampleStageZ_->withinTolerance(addOnsMicroprobeSampleStageZ_->value()) && sxrmbMicroprobeSampleStageZ_->withinTolerance(sxrmbMicroprobeSampleStageFeedbackZ_->value()) && sxrmbMicroprobeSampleStageStatusZ_->withinTolerance(0.0)){
		qDebug() << "Faking a sample stage Z move";
		addOnsMicroprobeSampleStageStatusZ_->move(1.0);
		QTimer::singleShot(100, this, SLOT(restoreAddOnsMicroprobeSampleStageStatusZ()));
		return;
	}

	if(!sxrmbMicroprobeSampleStageZ_->withinTolerance(addOnsMicroprobeSampleStageZ_->value())){
		qDebug() << "Doing addOns Sample Stage Z move in the regular way";
		sxrmbMicroprobeSampleStageZ_->move(addOnsMicroprobeSampleStageZ_->value());
	}
}

void SXRMBAddOnsCoordinator::restoreAddOnsMicroprobeSampleStageStatusX(){
	qDebug() << "Restoring AddOns sample stage X status to old SXRMB sample stage X status with AddOns " << addOnsMicroprobeSampleStageStatusX_->value() << " and old X status " << sxrmbMicroprobeSampleStageStatusX_->value();

	if(!addOnsMicroprobeSampleStageStatusX_->withinTolerance(sxrmbMicroprobeSampleStageStatusX_->value()))
		addOnsMicroprobeSampleStageStatusX_->move(sxrmbMicroprobeSampleStageStatusX_->value());
}

void SXRMBAddOnsCoordinator::restoreAddOnsMicroprobeSampleStageStatusY(){
	qDebug() << "Restoring AddOns sample stage Y status to old SXRMB sample stage Y status with AddOns " << addOnsMicroprobeSampleStageStatusY_->value() << " and old Y status " << sxrmbMicroprobeSampleStageStatusY_->value();

	if(!addOnsMicroprobeSampleStageStatusY_->withinTolerance(sxrmbMicroprobeSampleStageStatusY_->value()))
		addOnsMicroprobeSampleStageStatusY_->move(sxrmbMicroprobeSampleStageStatusY_->value());
}

void SXRMBAddOnsCoordinator::restoreAddOnsMicroprobeSampleStageStatusZ(){
	qDebug() << "Restoring AddOns sample stage Z status to old SXRMB sample stage Z status with AddOns " << addOnsMicroprobeSampleStageStatusZ_->value() << " and old Z status " << sxrmbMicroprobeSampleStageStatusZ_->value();

	if(!addOnsMicroprobeSampleStageStatusZ_->withinTolerance(sxrmbMicroprobeSampleStageStatusZ_->value()))
		addOnsMicroprobeSampleStageStatusZ_->move(sxrmbMicroprobeSampleStageStatusZ_->value());
}

void SXRMBAddOnsCoordinator::onAddOnsAmbiantTableHeightStopValueChanged()
{
	if(!connectedOnce_)
		return;

	if(addOnsAmbiantTableHeightStopControl_->withinTolerance(1.0)){
		ambiantTableUpstreamInboundStopControl_->move(1.0);
		ambiantTableUpstreamOutboundStopControl_->move(1.0);
		ambiantTableDownstreamStopControl_->move(1.0);

		addOnsAmbiantTableHeightStopControl_->move(0);
	}
}

