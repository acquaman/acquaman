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


#include "REIXSXESMCPDetectorPre2013.h"

#include <cmath>
#include "util/AMErrorMonitor.h"

REIXSXESMCPDetectorPre2013::REIXSXESMCPDetectorPre2013(const QString& name, const QString& basePVName, QObject *parent) :
	QObject(parent)
{
	name_ = name;
	description_ = "XES Imaging Detector";
	basePVName_ = basePVName;

	totalCountsPV_ = new AMProcessVariable(basePVName_+":totalCounts", true, this);
	countsPerSecondPV_ = new AMProcessVariable(basePVName_+":countsPerSecond", true, this);
	imagePV_ = new AMProcessVariable(basePVName_+":image", true, this);
	instantaneousImagePV_ = new AMProcessVariable(basePVName_+":instantaneousImage", true, this);
	resolutionXPV_ = new AMProcessVariable(basePVName_+":resolutionX", true, this);
	resolutionYPV_ = new AMProcessVariable(basePVName_+":resolutionY", true, this);

	clearPV_ = new AMProcessVariable(basePVName_+":clear", true, this);
	orientationControl_ = new AMPVControl("mcpOrientation", basePVName_+":orientation", basePVName_+":orientation", QString(), this, 0.1);
	averagingPeriodSecsControl_ = new AMPVControl("mcpAveragingPeriod", basePVName_+":averagingPeriodSecs", basePVName_+":averagingPeriodSecs", QString(), this, 0.1);
	persistTimeSecsControl_ = new AMPVControl("mcpPersistDuration", basePVName_+":persistTimeSecs", basePVName_+":persistTimeSecs", QString(), this, 0.1);

	image_ = new REIXSXESMCPDataSourcePre2013("xesImage", imagePV_, resolutionXPV_, resolutionYPV_, this);
	image_->setDescription("Accumulated Detector Image");
	instantaneousImage_ = new REIXSXESMCPDataSourcePre2013("xesRealtimeImage", instantaneousImagePV_, resolutionXPV_, resolutionYPV_, this);
	instantaneousImage_->setDescription("Instantaneous Detector Image");

	connect(totalCountsPV_, SIGNAL(valueChanged(double)), this, SIGNAL(totalCountsChanged(double)));
	connect(countsPerSecondPV_, SIGNAL(valueChanged(double)), this, SIGNAL(countsPerSecondChanged(double)));
	connect(imagePV_, SIGNAL(valueChanged()), this, SIGNAL(imageDataChanged()));
	connect(instantaneousImagePV_, SIGNAL(valueChanged()), this, SIGNAL(instantaneousImageDataChanged()));
}

REIXSXESMCPDetectorPre2013::~REIXSXESMCPDetectorPre2013() {
	delete image_;
	delete instantaneousImage_;
}


 REIXSXESMCPDataSourcePre2013::~REIXSXESMCPDataSourcePre2013(){}
REIXSXESMCPDataSourcePre2013::REIXSXESMCPDataSourcePre2013(const QString &name, AMProcessVariable *imagePV, AMProcessVariable *resolutionXPV, AMProcessVariable *resolutionYPV, QObject *parent)
	: QObject(parent), AMDataSource(name) {

	setDescription("XES Detector Image");
	isConnected_ = false;

	axes_ << AMAxisInfo("x", 0, "x - energy axis", "pixels");
	axes_ << AMAxisInfo("y", 0, "y - vertical axis", "pixels");
	pixelsX_ = pixelsY_ = 0;

	axes_[0].isUniform = true;
	axes_[1].isUniform = true;

	imagePV_ = imagePV;
	resolutionXPV_ = resolutionXPV;
	resolutionYPV_ = resolutionYPV;


	// establish connections to see when the image changes
	connect(imagePV_, SIGNAL(valueChanged()), this, SLOT(onImageValuesChanged()));

	// establish connections to see when the PVs connect and we get all the information we need...
	connect(imagePV_, SIGNAL(connected(bool)), this, SLOT(onConnectionStateChanged()));
	connect(resolutionXPV_, SIGNAL(valueChanged()), this, SLOT(onConnectionStateChanged()));
	connect(resolutionXPV_, SIGNAL(disconnected()), this, SLOT(onConnectionStateChanged()));
	connect(resolutionYPV_, SIGNAL(valueChanged()), this, SLOT(onConnectionStateChanged()));
	connect(resolutionYPV_, SIGNAL(disconnected()), this, SLOT(onConnectionStateChanged()));

	// check just in case all of these PVs are already connected...
	onConnectionStateChanged();

}

// Called whenever the connection state of any PV changes; emits valuesChanged(), sizeChanged(), and stateChanged() as required.
void REIXSXESMCPDataSourcePre2013::onConnectionStateChanged() {
	bool wasConnected = isConnected_;

	isConnected_ = imagePV_->isConnected() &&
				   resolutionXPV_->hasValues() &&
				   resolutionYPV_->hasValues();

	// becoming disconnected...
	if(wasConnected && !isConnected_) {
		pixelsX_ = axes_[0].size = 0;
		pixelsY_ = axes_[1].size = 0;
		emitValuesChanged();
		emitSizeChanged();
		emitStateChanged(AMDataSource::InvalidFlag);

		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -2, "Lost connection to MCP Detector " + imagePV_->pvName()));
	}

	// becoming connected...
	if(!wasConnected && isConnected_) {
		pixelsX_ = axes_[0].size = int(pow(2.0, double(resolutionXPV_->getInt())));
		pixelsY_ = axes_[1].size = int(pow(2.0, double(resolutionYPV_->getInt())));
		emitValuesChanged();
		emitSizeChanged();
		emitStateChanged(AMDataSource::ProcessingFlag);

		// AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, 0, "Connection established to MCP Detector " + imagePV_->pvName() + QString(" Size: %1 x %2").arg(pixelsX_).arg(pixelsY_)));
	}
}

// Called when the image PV changes. emits valuesChanged().
void REIXSXESMCPDataSourcePre2013::onImageValuesChanged() {
	emitValuesChanged();
}
