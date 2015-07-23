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


#include "BioXASMainXASScanActionController.h"

#include "acquaman/BioXAS/BioXASMainXASScanConfiguration.h"
#include "beamline/BioXAS/BioXASMainBeamline.h"

BioXASMainXASScanActionController::BioXASMainXASScanActionController(BioXASMainXASScanConfiguration *configuration, QObject *parent) :
	BioXASXASScanActionController(configuration, parent)
{
	mainConfiguration_ = configuration;

	// Set exporter option.

	AMExporterOptionXDIFormat *bioXASDefaultXAS = BioXAS::buildStandardXDIFormatExporterOption("BioXAS XAS (XDI Format)", mainConfiguration_->edge().split(" ").first(), mainConfiguration_->edge().split(" ").last(), true);
	if (bioXASDefaultXAS->id() > 0)
		AMAppControllerSupport::registerClass<BioXASMainXASScanConfiguration, AMExporterXDIFormat, AMExporterOptionXDIFormat>(bioXASDefaultXAS->id());
}

BioXASMainXASScanActionController::~BioXASMainXASScanActionController()
{

}
