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


#include "REIXSRSXSMCPDataSource.h"

#include "beamline/AMPVControl.h"
#include "util/AMErrorMonitor.h"

#include <QDebug>

 REIXSRSXSMCPDataSource::~REIXSRSXSMCPDataSource(){}
REIXSRSXSMCPDataSource::REIXSRSXSMCPDataSource(const QString &name, AMReadOnlyPVControl *imagePV, QObject *parent)
	: QObject(parent), AMDataSource(name) {

	setDescription("RSXS Detector Image");
	isConnected_ = false;

	axes_ << AMAxisInfo("x", 0, "x axis", "pixels");
	axes_ << AMAxisInfo("y", 0, "y axis", "pixels");
	pixelsX_ = pixelsY_ = 128;

	axes_[0].isUniform = true;
	axes_[1].isUniform = true;

	axes_[0].size = pixelsX_;
	axes_[1].size = pixelsY_;

	imageControl_ = imagePV;

	// establish connections to see when the image changes
	connect(imageControl_, SIGNAL(valueChanged(double)), this, SLOT(onImageValuesChanged()));

	// establish connections to see when the PVs connect and we get all the information we need...
	connect(imageControl_, SIGNAL(connected(bool)), this, SLOT(onConnectionStateChanged()));

	// check just in case all of these PVs are already connected...
	onConnectionStateChanged();

}

// Called whenever the connection state of any PV changes. Emits valuesChanged(), sizeChanged(), and stateChanged() as required.
void REIXSRSXSMCPDataSource::onConnectionStateChanged() {
	bool wasConnected = isConnected_;

	isConnected_ = imageControl_->isConnected();

	// becoming disconnected...
	if(wasConnected && !isConnected_) {
		emitValuesChanged();
		emitStateChanged(AMDataSource::InvalidFlag);

		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -2, QString("Lost connection to MCP Detector %1").arg(imageControl_->readPVName()) ));
	}

	// becoming connected...
	if(!wasConnected && isConnected_) {
		emitValuesChanged();
		emitSizeChanged();
		emitStateChanged(AMDataSource::ProcessingFlag);
	}
}

int REIXSRSXSMCPDataSource::idOfAxis(const QString& axisName) const
{
	if(axisName == axes_.at(0).name)
		return 0;
	if(axisName == axes_.at(1).name)
		return 1;
	return -1;
}

// Data value access
////////////////////////////

/// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient or any are out of range, or if the data is not ready.
AMNumber REIXSRSXSMCPDataSource::value(const AMnDIndex& indexes) const {
	if(!isConnected_)
		return AMNumber();
	if(indexes.rank() != 2)
		return AMNumber(AMNumber::DimensionError);
#ifdef AM_ENABLE_BOUNDS_CHECKING
		if(indexes.i() >= pixelsX_ || indexes.j() >= pixelsY_)
			return AMNumber(AMNumber::OutOfBoundsError);
#endif

	return imageControl_->readPV()->getInt(indexes.i()*pixelsY_ + indexes.j());
}

bool REIXSRSXSMCPDataSource::values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const {
	if(!isConnected_)
		return false;
	if(indexStart.rank() != 2 || indexEnd.rank() != 2)
		return false;
	if(indexEnd.i() < indexStart.i() || indexEnd.j() < indexStart.j())
		return false;
#ifdef AM_ENABLE_BOUNDS_CHECKING
	if(indexEnd.i() >= pixelsX_ || indexEnd.j() >= pixelsY_)
		return false;
#endif
	int totalSize = indexStart.totalPointsTo(indexEnd);
	QVector<int> data = imageControl_->readPV()->lastIntegerValues();

	foreach(int v, imageControl_->readPV()->lastIntegerValues())
		*(outputValues++) = double(v);

	return true;
}

AMNumber REIXSRSXSMCPDataSource::axisValue(int axisNumber, int index) const {
	Q_UNUSED(axisNumber)
	return index;
}

bool REIXSRSXSMCPDataSource::axisValues(int axisNumber, int startIndex, int endIndex, AMNumber *outputValues) const
{
	Q_UNUSED(axisNumber);

	for (int i = 0, size = endIndex-startIndex+1; i < size; i++)
		outputValues[i] = startIndex + i;

	return true;
}

// Called when the image PV changes. emits valuesChanged().
void REIXSRSXSMCPDataSource::onImageValuesChanged() {
	//emitValuesChanged(AMnDIndex(0,0), AMnDIndex(pixelsX_-1, pixelsY_-1));
	emitValuesChanged();
}
