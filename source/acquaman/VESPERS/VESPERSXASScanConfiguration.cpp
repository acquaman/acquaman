/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "VESPERSXASScanConfiguration.h"
#include "acquaman/VESPERS/VESPERSXASDacqScanController.h"
#include "ui/VESPERS/VESPERSXASScanConfigurationView.h"
#include "beamline/VESPERS/VESPERSBeamline.h"

VESPERSXASScanConfiguration::VESPERSXASScanConfiguration(QObject *parent)
	: AMXASScanConfiguration(parent)
{
	ionChamberNames_.insert(Isplit, "Isplit");
	ionChamberNames_.insert(Iprekb, "Iprekb");
	ionChamberNames_.insert(Imini, "Imini");
	ionChamberNames_.insert(Ipost, "Ipost");

	regions_->setEnergyControl(VESPERSBeamline::vespers()->energyRelative());
	regions_->setTimeControl(VESPERSBeamline::vespers()->masterDwellTime());
	setName("XAS Scan");
	fluorescenceDetectorChoice_ = None;
	It_ = Ipost;
	I0_ = Imini;
	edge_ = "";
	energy_ = 0.0;

	goToPosition_ = false;
	position_ = qMakePair(0.0, 0.0);
}

VESPERSXASScanConfiguration::VESPERSXASScanConfiguration(const VESPERSXASScanConfiguration &original)
	: AMXASScanConfiguration(original.parent())
{
	ionChamberNames_.insert(Isplit, "Isplit");
	ionChamberNames_.insert(Iprekb, "Iprekb");
	ionChamberNames_.insert(Imini, "Imini");
	ionChamberNames_.insert(Ipost, "Ipost");

	regions_->setEnergyControl(VESPERSBeamline::vespers()->energyRelative());
	regions_->setTimeControl(VESPERSBeamline::vespers()->masterDwellTime());
	for (int i = 0; i < original.regionCount(); i++)
		regions_->addRegion(i, original.regionStart(i), original.regionDelta(i), original.regionEnd(i), original.regionTime(i));

	setName(original.name());
	fluorescenceDetectorChoice_ = original.fluorescenceDetectorChoice();
	It_ = original.transmissionChoice();
	I0_ = original.incomingChoice();
	edge_ = original.edge();
	energy_ = original.energy();

	goToPosition_ = original.goToPosition();
	position_ = original.position();

	roiInfoList_ = original.roiList();
}

AMScanConfiguration *VESPERSXASScanConfiguration::createCopy() const
{
	return new VESPERSXASScanConfiguration(*this);
}

AMScanController *VESPERSXASScanConfiguration::createController()
{
	return new VESPERSXASDacqScanController(this);
}

AMScanConfigurationView *VESPERSXASScanConfiguration::createView()
{
	return new VESPERSXASScanConfigurationView(this);
}

QString VESPERSXASScanConfiguration::detailedDescription() const
{
	return QString("VESPERS XAS Scan");
}
