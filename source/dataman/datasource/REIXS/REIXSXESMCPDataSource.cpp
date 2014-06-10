#include "REIXSXESMCPDataSource.h"

#include "beamline/AMPVControl.h"
#include "util/AMErrorMonitor.h"

 REIXSXESMCPDataSource::~REIXSXESMCPDataSource(){}
REIXSXESMCPDataSource::REIXSXESMCPDataSource(const QString &name, AMReadOnlyPVControl *imagePV, AMReadOnlyPVControl *resolutionXPV, AMReadOnlyPVControl *resolutionYPV, QObject *parent)
	: QObject(parent), AMDataSource(name) {

	setDescription("XES Detector Image");
	isConnected_ = false;

	axes_ << AMAxisInfo("x", 0, "x - energy axis", "pixels");
	axes_ << AMAxisInfo("y", 0, "y - vertical axis", "pixels");
	pixelsX_ = pixelsY_ = 0;

	axes_[0].isUniform = true;
	axes_[1].isUniform = true;

	imageControl_ = imagePV;
	resolutionXControl_ = resolutionXPV;
	resolutionYControl_ = resolutionYPV;


	// establish connections to see when the image changes
	connect(imageControl_, SIGNAL(valueChanged(double)), this, SLOT(onImageValuesChanged()));

	// establish connections to see when the PVs connect and we get all the information we need...
	connect(imageControl_, SIGNAL(connected(bool)), this, SLOT(onConnectionStateChanged()));
	connect(resolutionXControl_, SIGNAL(valueChanged(double)), this, SLOT(onConnectionStateChanged()));
	connect(resolutionXControl_, SIGNAL(connected(bool)), this, SLOT(onConnectionStateChanged()));
	connect(resolutionYControl_, SIGNAL(valueChanged(double)), this, SLOT(onConnectionStateChanged()));
	connect(resolutionYControl_, SIGNAL(connected(bool)), this, SLOT(onConnectionStateChanged()));

	// check just in case all of these PVs are already connected...
	onConnectionStateChanged();

}

// Called whenever the connection state of any PV changes; emits valuesChanged(), sizeChanged(), and stateChanged() as required.
void REIXSXESMCPDataSource::onConnectionStateChanged() {
	bool wasConnected = isConnected_;

	isConnected_ = imageControl_->isConnected() && resolutionXControl_->isConnected() && resolutionYControl_->isConnected();

	// becoming disconnected...
	if(wasConnected && !isConnected_) {
		pixelsX_ = axes_[0].size = 0;
		pixelsY_ = axes_[1].size = 0;
		emitValuesChanged();
		emitSizeChanged();
		emitStateChanged(AMDataSource::InvalidFlag);

		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -2, QString("Lost connection to MCP Detector %1").arg(imageControl_->readPVName()) ));
	}

	// becoming connected...
	if(!wasConnected && isConnected_) {
		pixelsX_ = axes_[0].size = int(pow(2.0, double(resolutionXControl_->value())));
		pixelsY_ = axes_[1].size = int(pow(2.0, double(resolutionYControl_->value())));
		emitValuesChanged();
		emitSizeChanged();
		emitStateChanged(AMDataSource::ProcessingFlag);

		// AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, 0, "Connection established to MCP Detector " + imagePV_->pvName() + QString(" Size: %1 x %2").arg(pixelsX_).arg(pixelsY_)));
	}
}

int REIXSXESMCPDataSource::idOfAxis(const QString& axisName) {
	if(axisName == axes_.at(0).name)
		return 0;
	if(axisName == axes_.at(1).name)
		return 1;
	return -1;
}

// Data value access
////////////////////////////

/// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient or any are out of range, or if the data is not ready.
AMNumber REIXSXESMCPDataSource::value(const AMnDIndex& indexes) const {
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

bool REIXSXESMCPDataSource::values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const {
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
	foreach(int v, imageControl_->readPV()->lastIntegerValues())
		*(outputValues++) = double(v);

	return true;
}

AMNumber REIXSXESMCPDataSource::axisValue(int axisNumber, int index) const {
	Q_UNUSED(axisNumber)
	return index;
}

// Called when the image PV changes. emits valuesChanged().
void REIXSXESMCPDataSource::onImageValuesChanged() {
	emitValuesChanged();
}
