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

#include "PGMBeamline.h"

PGMBeamline::PGMBeamline()
	: CLSBeamline("PGM Beamline")
{
	connected_ = false;

	setupComponents();
	setupDiagnostics();
	setupSampleStage();
	setupDetectors();
	setupControlSets();
	setupMono();
	setupMotorGroup();
	setupControlsAsDetectors();
	setupExposedControls();
	setupExposedDetectors();

	// update the connection changed flag and emit the signal to indicate whether conneciton is changed
	onControlConnectionChanged();
}

PGMBeamline::~PGMBeamline()
{

}

bool PGMBeamline::isConnected() const
{
	bool connected = positionSlit_ && positionSlit_->isConnected()
				  && gapSlit_ && gapSlit_->isConnected();

	return connected;
}

void PGMBeamline::onControlConnectionChanged()
{
	bool isConnectedNow = isConnected();
	if (connected_ != isConnectedNow) {
		connected_ = isConnectedNow;

		emit connected(connected_);
	}
}

void PGMBeamline::setupDiagnostics()
{

}

void PGMBeamline::setupSampleStage()
{

}

void PGMBeamline::setupMotorGroup()
{

}

void PGMBeamline::setupDetectors()
{

}

void PGMBeamline::setupControlSets()
{

}

void PGMBeamline::setupMono()
{

}

void PGMBeamline::setupComponents()
{
	positionSlit_ = new AMSlit("positionSlits", this);
	positionSlit_->setFirstBlade(new AMPVwStatusControl("positionWidthA","PSL16114I2101:Y:mm:fbk", "SMTR16114I2105:mm","PSL16114I2101:Y:mm:status", QString(), this, 0), AMSlit::OpensPositively);
	positionSlit_->setSecondBlade(new AMPVwStatusControl("positionWidthB","PSL16114I2201:Y:mm:fbk", "SMTR16114I2205:mm","PSL16114I2201:Y:mm:status", QString(), this, 0), AMSlit::OpensNegatively);
	connect(positionSlit_, SIGNAL(connected(bool)), this, SLOT(onControlConnectionChanged()));

	gapSlit_ = new AMSlit("gapSlits", this);
	gapSlit_->setFirstBlade(new AMPVwStatusControl("gapSlitBranchA","PSL16114I2101:X:mm:fbk", "SMTR16114I2104:mm","PSL16114I2101:X:mm:status", QString(), this, 0), AMSlit::OpensPositively);
	gapSlit_->setSecondBlade(new AMPVwStatusControl("gapSlitBranchB","PSL16114I2201:X:mm:fbk", "SMTR16114I2204:mm","PSL16114I2201:X:mm:status", QString(), this, 0), AMSlit::OpensNegatively);
	connect(gapSlit_, SIGNAL(connected(bool)), this, SLOT(onControlConnectionChanged()));
}

void PGMBeamline::setupControlsAsDetectors()
{

}

void PGMBeamline::setupExposedControls()
{
	// add the slits controls
	addExposedControl(positionSlit_->center());
	addExposedControl(positionSlit_->gap());
	addExposedControl(gapSlit_->center());
	addExposedControl(gapSlit_->gap());

	//
}

void PGMBeamline::setupExposedDetectors()
{

}


