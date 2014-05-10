/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "REIXSXASScanConfiguration.h"
#include "beamline/REIXS/REIXSBeamline.h"

REIXSXASScanConfiguration::REIXSXASScanConfiguration(QObject *parent) :
	AMXASScanConfiguration(parent)
{
	regions()->appendRegion(380, 0.1, 420, 1);
	regions()->setSensibleRange(100, 1000);

	scanNumber_ = 0;
	sampleId_ = -1;
	namedAutomatically_ = true;

	slitWidth_ = 50;
	applySlitWidth_ = false;
	monoGrating_ = 0;
	applyMonoGrating_ = false;
	monoMirror_ = 0;
	applyMonoMirror_ = false;
	polarization_ = 3;
	polarizationAngle_ = -90;
	applyPolarization_ = false;
}

REIXSXASScanConfiguration::~REIXSXASScanConfiguration() {

}

REIXSXASScanConfiguration::REIXSXASScanConfiguration(const REIXSXASScanConfiguration &other)
	: AMXASScanConfiguration(other)
{
	/// \todo This belongs in AMRegionScanConfiguration: copy the regions.
	regions_->setSensibleStart(other.regions()->sensibleStart());
	regions_->setSensibleEnd(other.regions()->sensibleEnd());
	regions_->setDefaultUnits(other.regions()->defaultUnits());
	regions_->setDefaultTimeUnits(other.regions()->defaultTimeUnits());
	for(int x = 0; x < other.regionCount(); x++)
		regions_->addRegion(x, other.regionStart(x), other.regionDelta(x), other.regionEnd(x), other.regionTime(x));

	//regions_->setDefaultControl(other.regions()->defaultControl());
	regions_->setDefaultControl(REIXSBeamline::bl()->photonSource()->energy());
	scanNumber_ = other.scanNumber_;
	sampleId_ = other.sampleId_;
	namedAutomatically_ = other.namedAutomatically_;

	slitWidth_ = other.slitWidth_;
	applySlitWidth_ = other.applySlitWidth_;
	monoGrating_ = other.monoGrating_;
	applyMonoGrating_ = other.applyMonoGrating_;
	monoMirror_ = other.monoMirror_;
	applyMonoMirror_ = other.applyMonoMirror_;
	polarization_ = other.polarization_;
	polarizationAngle_ = other.polarizationAngle_;
	applyPolarization_ = other.applyPolarization_;
	////////////////////////
	setAutoExportEnabled(false);
}

AMScanConfiguration * REIXSXASScanConfiguration::createCopy() const
{
	// using default copy constructor:
	return new REIXSXASScanConfiguration(*this);
}

#include "acquaman/REIXS/REIXSXASScanController.h"
#include "acquaman/REIXS/REIXSXASScanActionController.h"
AMScanController * REIXSXASScanConfiguration::createController()
{
	//return new REIXSXASScanController(this);
//	return new REIXSXASScanActionController(this);
	AMScanActionController *controller = new REIXSXASScanActionController(this);
	controller->buildScanController();

	return controller;
}

#include "ui/dataman/AMRegionScanConfigurationView.h"
AMScanConfigurationView * REIXSXASScanConfiguration::createView()
{
	return new AMRegionScanConfigurationView(this);
}
