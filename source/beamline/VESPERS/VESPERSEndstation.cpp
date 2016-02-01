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


#include "VESPERSEndstation.h"
#include "beamline/CLS/CLSMAXvMotor.h"
#include "application/VESPERS/VESPERS.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"
#include "actions3/actions/AMWaitAction.h"
#include "beamline/VESPERS/VESPERSToggleControl.h"

#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QTextStream>

 VESPERSEndstation::~VESPERSEndstation(){}
VESPERSEndstation::VESPERSEndstation(QObject *parent)
	: QObject(parent)
{
	current_ = 0;
	wasConnected_ = false;

	// The controls used for the control window.
	ccdControl_ = new AMPVwStatusControl("CCD motor", "SMTR1607-2-B21-18:mm:fbk", "SMTR1607-2-B21-18:mm", "SMTR1607-2-B21-18:status", "SMTR1607-2-B21-18:stop", this, 1.0, 2.0);
	microscopeControl_ = new AMPVwStatusControl("Microscope motor", "SMTR1607-2-B21-17:mm:sp", "SMTR1607-2-B21-17:mm", "SMTR1607-2-B21-17:status", "SMTR1607-2-B21-17:stop", this, 1.0, 2.0);
	fourElControl_ = new AMPVwStatusControl("4-Element Vortex motor", "SMTR1607-2-B21-27:mm:fbk", "SMTR1607-2-B21-27:mm", "SMTR1607-2-B21-27:status", "SMTR1607-2-B21-27:stop", this, 1.0, 2.0);
	singleElControl_ = new AMPVwStatusControl("1-Element Vortex motor", "SMTR1607-2-B21-15:mm:fbk", "SMTR1607-2-B21-15:mm", "SMTR1607-2-B21-15:status", "SMTR1607-2-B21-15:stop", this, 1.0, 2.0);

	laserPositionControl_ = new AMReadOnlyPVControl("Laser Position", "PSD1607-2-B20-01:OUT1:fbk", this);
	laserPositionStatusControl_ = new AMReadOnlyPVControl("Laser Position Status", "PSD1607-2-B20-01:OUT1:fbk:status", this);

	// Microscope light PV.
	micLightPV_ = new AMProcessVariable("07B2_PLC_Mic_Light_Inten", true, this);

	// Laser on/off control.
	laserPower_ = new VESPERSToggleControl("Laser Power Control", "07B2_PLC_LaserDistON", "07B2_PLC_LaserDistON_Tog", this, 0.1);

	// The beam attenuation filters.
	filter250umA_ = new VESPERSToggleControl("Filter 250um A", "07B2_PLC_PFIL_01_F1_Ctrl", "07B2_PLC_PFIL_01_F1_Toggle", this, 0.1);
	filter250umB_ = new VESPERSToggleControl("Filter 250um B", "07B2_PLC_PFIL_01_F2_Ctrl", "07B2_PLC_PFIL_01_F2_Toggle", this, 0.1);
	filter100umA_ = new VESPERSToggleControl("Filter 100um A", "07B2_PLC_PFIL_02_F3_Ctrl", "07B2_PLC_PFIL_02_F3_Toggle", this, 0.1);
	filter100umB_ = new VESPERSToggleControl("Filter 100um B", "07B2_PLC_PFIL_02_F4_Ctrl", "07B2_PLC_PFIL_02_F4_Toggle", this, 0.1);
	filter50umA_ = new VESPERSToggleControl("Filter 50um A", "07B2_PLC_PFIL_02_F1_Ctrl", "07B2_PLC_PFIL_02_F1_Toggle", this, 0.1);
	filter50umB_ = new VESPERSToggleControl("Filter 50um B", "07B2_PLC_PFIL_02_F2_Ctrl", "07B2_PLC_PFIL_02_F2_Toggle", this, 0.1);
	filterShutterUpper_ = new VESPERSToggleControl("Filter Shutter Upper", "07B2_PLC_PFIL_01_F3_Ctrl", "07B2_PLC_PFIL_01_F3_Toggle", this, 0.1);
	filterShutterLower_ = new VESPERSToggleControl("Filter Shutter Lower", "07B2_PLC_PFIL_01_F4_Ctrl", "07B2_PLC_PFIL_01_F4_Toggle", this, 0.1);
	filterThickness_ = -1;

	// Setup filters.
	filterMap_.insert("50A", filter50umA_);
	filterMap_.insert("50B", filter50umB_);
	filterMap_.insert("100A", filter100umA_);
	filterMap_.insert("100B", filter100umB_);
	filterMap_.insert("250A", filter250umA_);
	filterMap_.insert("250B", filter250umB_);

	// Get the current soft limits.
	loadConfiguration();

	// Connections.
	connect(filterShutterLower_, SIGNAL(valueChanged(double)), this, SLOT(onShutterChanged(double)));
	connect(laserPower_, SIGNAL(valueChanged(double)), this, SIGNAL(laserPoweredChanged()));
	connect(micLightPV_, SIGNAL(valueChanged(int)), this, SIGNAL(lightIntensityChanged(int)));
	connect(ccdControl_, SIGNAL(valueChanged(double)), this, SIGNAL(ccdFbkChanged(double)));
	connect(microscopeControl_, SIGNAL(valueChanged(double)), this, SIGNAL(microscopeFbkChanged(double)));
	connect(singleElControl_, SIGNAL(valueChanged(double)), this, SIGNAL(singleElFbkChanged(double)));
	connect(fourElControl_, SIGNAL(valueChanged(double)), this, SIGNAL(fourElFbkChanged(double)));
	connect(laserPositionControl_, SIGNAL(valueChanged(double)), this, SIGNAL(laserPositionChanged(double)));
	connect(laserPositionStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(onLaserPositionValidityChanged(double)));

	QList<AMControl *> list(filterMap_.values());
	for (int i = 0; i < list.size(); i++)
		connect(list.at(i), SIGNAL(connected(bool)), this, SLOT(onFiltersConnected()));
}

void VESPERSEndstation::onLaserPositionValidityChanged(double value)
{
	emit laserPositionValidityChanged(value == 1.0);
}

bool VESPERSEndstation::loadConfiguration()
{
	QString filePath = VESPERS::getHomeDirectory() % "/acquaman/build/endstation.config";
	QFile file(filePath);

	if (!file.open(QFile::ReadOnly | QFile::Text))
		return false;

	QTextStream in(&file);
	QStringList contents;

	while(!in.atEnd())
		contents << in.readLine();

	file.close();

	softLimits_.clear();

	softLimits_.insert(ccdControl_, AMRange(((QString)contents.at(2)).toDouble(), ((QString)contents.at(4)).toDouble()));
	softLimits_.insert(singleElControl_, AMRange(((QString)contents.at(6)).toDouble(), ((QString)contents.at(7)).toDouble()));
	softLimits_.insert(fourElControl_, AMRange(((QString)contents.at(10)).toDouble(), ((QString)contents.at(11)).toDouble()));
	softLimits_.insert(microscopeControl_, AMRange(((QString)contents.at(14)).toDouble(), ((QString)contents.at(15)).toDouble()));

	microscopeNames_ = qMakePair((QString)contents.at(16), (QString)contents.at(17));
	upperCcdSoftLimitwHeliumBuffer_ = contents.at(19).toDouble();
	heliumBufferAttached_ = contents.at(20) == "YES";
	ccdAt90Degrees_ = contents.at(21) == "YES";

	updateControl(current_);

	return true;
}

AMControl *VESPERSEndstation::control(QString name) const
{
	if (name.compare("CCD motor") == 0)
		return ccdControl_;
	else if (name.compare("1-Element Vortex motor") == 0)
		return singleElControl_;
	else if (name.compare("4-Element Vortex motor") == 0)
		return fourElControl_;
	else if (name.compare("Microscope motor") == 0)
		return microscopeControl_;

	return 0;
}

void VESPERSEndstation::setCurrent(QString name)
{
	if (name.compare("CCD motor") == 0)
		updateControl(ccdControl_);
	else if (name.compare("1-Element Vortex motor") == 0)
		updateControl(singleElControl_);
	else if (name.compare("4-Element Vortex motor") == 0)
		updateControl(fourElControl_);
	else if (name.compare("Microscope motor") == 0)
		updateControl(microscopeControl_);
	else
		current_ = 0;
}

void VESPERSEndstation::updateControl(AMControl *control)
{
	if (control == 0)
		return;

	current_ = control;
	emit currentControlChanged(current_);
}

void VESPERSEndstation::onFiltersConnected()
{
	bool connected = true;
	QList<AMControl *> filters(filterMap_.values());

	for (int i = 0; i < filters.size(); i++)
		connected = connected && filters.at(i)->isConnected();

	if (!wasConnected_ && connected){

		for (int i = 0; i < filters.size(); i++)
			connect(filters.at(i), SIGNAL(valueChanged(double)), this, SLOT(onFiltersChanged()));

		onFiltersChanged();
	}
	else if (wasConnected_ && !connected)
		for (int i = 0; i < filters.size(); i++)
			disconnect(filters.at(i), SIGNAL(valueChanged(double)), this, SLOT(onFiltersChanged()));
}

void VESPERSEndstation::onFiltersChanged()
{
	int sum = 0;

	if ((int)filterMap_.value("50A")->value() == 1)
		sum += 1;
	if ((int)filterMap_.value("50B")->value() == 1)
		sum += 1;
	if ((int)filterMap_.value("100A")->value() == 1)
		sum += 2;
	if ((int)filterMap_.value("100B")->value() == 1)
		sum += 2;
	if ((int)filterMap_.value("250A")->value() == 1)
		sum += 5;
	if ((int)filterMap_.value("250B")->value() == 1)
		sum += 5;

	filterThickness_ = sum*50;
	emit filterThicknessChanged(sum);
}

void VESPERSEndstation::setFilterThickness(int index)
{
	switch(index){
	case 0:
		filterMap_.value("50A")->move(0);
		filterMap_.value("50B")->move(0);
		filterMap_.value("100A")->move(0);
		filterMap_.value("100B")->move(0);
		filterMap_.value("250A")->move(0);
		filterMap_.value("250B")->move(0);

		break;
	case 1: // 50 um
		filterMap_.value("50A")->move(1);

		filterMap_.value("50B")->move(0);
		filterMap_.value("100A")->move(0);
		filterMap_.value("100B")->move(0);
		filterMap_.value("250A")->move(0);
		filterMap_.value("250B")->move(0);

		break;
	case 2: // 100 um
		filterMap_.value("100A")->move(1);

		filterMap_.value("50A")->move(0);
		filterMap_.value("50B")->move(0);
		filterMap_.value("100B")->move(0);
		filterMap_.value("250A")->move(0);
		filterMap_.value("250B")->move(0);

		break;
	case 3: // 150 um
		filterMap_.value("50A")->move(1);
		filterMap_.value("100A")->move(1);

		filterMap_.value("50B")->move(0);
		filterMap_.value("100B")->move(0);
		filterMap_.value("250A")->move(0);
		filterMap_.value("250B")->move(0);

		break;
	case 4: // 200 um
		filterMap_.value("100A")->move(1);
		filterMap_.value("100B")->move(1);

		filterMap_.value("50A")->move(0);
		filterMap_.value("50B")->move(0);
		filterMap_.value("250A")->move(0);
		filterMap_.value("250B")->move(0);

		break;
	case 5: // 250 um
		filterMap_.value("250A")->move(1);

		filterMap_.value("50A")->move(0);
		filterMap_.value("50B")->move(0);
		filterMap_.value("100A")->move(0);
		filterMap_.value("100B")->move(0);
		filterMap_.value("250B")->move(0);

		break;
	case 6: // 300 um
		filterMap_.value("250A")->move(1);
		filterMap_.value("50A")->move(1);

		filterMap_.value("50B")->move(0);
		filterMap_.value("100A")->move(0);
		filterMap_.value("100B")->move(0);
		filterMap_.value("250B")->move(0);

		break;
	case 7: // 350 um
		filterMap_.value("250A")->move(1);
		filterMap_.value("100A")->move(1);

		filterMap_.value("50A")->move(0);
		filterMap_.value("50B")->move(0);
		filterMap_.value("100B")->move(0);
		filterMap_.value("250B")->move(0);

		break;
	case 8: // 400 um
		filterMap_.value("250A")->move(1);
		filterMap_.value("100A")->move(1);
		filterMap_.value("50A")->move(1);

		filterMap_.value("50B")->move(0);
		filterMap_.value("100B")->move(0);
		filterMap_.value("250B")->move(0);

		break;
	case 9: // 450 um
		filterMap_.value("250A")->move(1);
		filterMap_.value("100A")->move(1);
		filterMap_.value("100B")->move(1);

		filterMap_.value("50A")->move(0);
		filterMap_.value("50B")->move(0);
		filterMap_.value("250B")->move(0);

		break;
	case 10: // 500 um
		filterMap_.value("250A")->move(1);
		filterMap_.value("250B")->move(1);

		filterMap_.value("50A")->move(0);
		filterMap_.value("50B")->move(0);
		filterMap_.value("100A")->move(0);
		filterMap_.value("100B")->move(0);

		break;
	case 11: // 550 um
		filterMap_.value("250A")->move(1);
		filterMap_.value("250B")->move(1);
		filterMap_.value("50A")->move(1);

		filterMap_.value("50B")->move(0);
		filterMap_.value("100A")->move(0);
		filterMap_.value("100B")->move(0);

		break;
	case 12: // 600 um
		filterMap_.value("250A")->move(1);
		filterMap_.value("250B")->move(1);
		filterMap_.value("100A")->move(1);

		filterMap_.value("50A")->move(0);
		filterMap_.value("50B")->move(0);
		filterMap_.value("100B")->move(0);

		break;
	case 13: // 650 um
		filterMap_.value("250A")->move(1);
		filterMap_.value("250B")->move(1);
		filterMap_.value("100A")->move(1);
		filterMap_.value("50A")->move(1);

		filterMap_.value("50B")->move(0);
		filterMap_.value("100B")->move(0);

		break;
	case 14: // 700 um
		filterMap_.value("250A")->move(1);
		filterMap_.value("250B")->move(1);
		filterMap_.value("100A")->move(1);
		filterMap_.value("100B")->move(1);

		filterMap_.value("50A")->move(0);
		filterMap_.value("50B")->move(0);

		break;
	case 15: // 750 um
		filterMap_.value("250A")->move(1);
		filterMap_.value("250B")->move(1);
		filterMap_.value("100A")->move(1);
		filterMap_.value("100B")->move(1);
		filterMap_.value("50A")->move(1);

		filterMap_.value("50B")->move(0);

		break;
	case 16: // 800 um
		filterMap_.value("250A")->move(1);
		filterMap_.value("250B")->move(1);
		filterMap_.value("100A")->move(1);
		filterMap_.value("100B")->move(1);
		filterMap_.value("50A")->move(1);
		filterMap_.value("50B")->move(1);
		break;
	}
}

void VESPERSEndstation::setShutterState(bool state)
{
	// Don't do anything if the shutter is already in the right state.
	if (shutterState() == state)
		return;

	// 0 = OUT.  For this to work properly, the upper shutter is to remain fixed at the out position and the lower shutter changes.  Therefore if upper is IN, put it out.
	if ((int)filterShutterUpper_->value() == 1)
		filterShutterUpper_->move(0);

	filterShutterLower_->move(state ? 1.0 : 0.0);
}

void VESPERSEndstation::setHeliumBufferFlag(bool attached)
{
	heliumBufferAttached_ = attached;
}

void VESPERSEndstation::setCCDAt90Degrees(bool at90Degrees)
{
	ccdAt90Degrees_ = at90Degrees;
}

bool VESPERSEndstation::microscopeInSafePosition(double value) const
{
	if (!ccdAt90Degrees_)
		return true;
	else if (heliumBufferAttached_)
		return controlWithinTolerance(microscopeControl_, value, softLimits_.value(microscopeControl_).maximum()) || ccdControl_->value() > upperCcdSoftLimitwHeliumBuffer_;

	else
		return controlWithinTolerance(microscopeControl_, value, softLimits_.value(microscopeControl_).maximum());
}

bool VESPERSEndstation::microscopeInSafePosition() const
{
	if (!ccdAt90Degrees_)
		return true;
	else if (heliumBufferAttached_)
		return controlWithinTolerance(microscopeControl_, microscopeControl_->value(), softLimits_.value(microscopeControl_).maximum()) || ccdControl_->value() > upperCcdSoftLimitwHeliumBuffer_;

	else
		return controlWithinTolerance(microscopeControl_, microscopeControl_->value(), softLimits_.value(microscopeControl_).maximum());
}

bool VESPERSEndstation::ccdInSafePosition(double value) const
{
	if (!ccdAt90Degrees_)
		return true;

	else if (heliumBufferAttached_)
		return value > upperCcdSoftLimitwHeliumBuffer_ || controlWithinTolerance(ccdControl_, ccdControl_->value(), upperCcdSoftLimitwHeliumBuffer_);

	else
		return value > softLimits_.value(ccdControl_).maximum() || controlWithinTolerance(ccdControl_, value, softLimits_.value(ccdControl_).maximum());
}

bool VESPERSEndstation::ccdInSafePosition() const
{
	if (!ccdAt90Degrees_)
		return true;

	else if (heliumBufferAttached_)
		return ccdControl_->value() > upperCcdSoftLimitwHeliumBuffer_ || controlWithinTolerance(ccdControl_, ccdControl_->value(), upperCcdSoftLimitwHeliumBuffer_);

	else
		return ccdControl_->value() > softLimits_.value(ccdControl_).maximum() || controlWithinTolerance(ccdControl_, ccdControl_->value(), softLimits_.value(ccdControl_).maximum());
}

void VESPERSEndstation::toggleLaserPower()
{
	laserPower_->move(laserPowered() ? 0 : 1);
	emit laserPoweredChanged();
}
