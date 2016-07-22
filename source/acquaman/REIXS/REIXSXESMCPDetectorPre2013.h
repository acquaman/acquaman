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


#ifndef REIXSXESMCPDETECTORPRE2013_H
#define REIXSXESMCPDETECTORPRE2013_H

#include <QObject>
#include "beamline/AMPVControl.h"
#include "dataman/datasource/AMDataSource.h"
#include "dataman/REIXS/REIXSXESMCPDetectorInfo.h"

/// This class exposes an array process variable as an AMDataSource, for use with REIXSXESMCPDetector. The first (0) axis is the energy axis, and the second (1) axis is the vertical detector axis.
/*! \todo Should there be an abstract class above this one, that doesn't tie this to an EPICS PV implementation? */
class REIXSXESMCPDataSourcePre2013 : public QObject, public AMDataSource {
	Q_OBJECT
public:
	/// Constructor. \c name is the AMDataSource::name() for this source, and imagePV is the process variable containing the image data. We also need the PVs which describe the
	virtual ~REIXSXESMCPDataSourcePre2013();
	REIXSXESMCPDataSourcePre2013(const QString& name, AMProcessVariable* imagePV, AMProcessVariable* resolutionXPV, AMProcessVariable* resolutionYPV, QObject* parent = 0);

	/// Human-readable description of the type of data source this is (ex: "One-dimensional math expression").  Subclasses should re-implement this.
	virtual QString typeDescription() const { return "XES Imaging Detector"; }

	/// Returns an OR-combination of StateFlags describing the current state of the data. We return InvalidFlag before we're connected and ProcessingFlag afterwards (because the data might be changing.)
	virtual int state() const { return isConnected_ ? ProcessingFlag : InvalidFlag; }

	// Axis Information
	//////////////////////
	/// Returns axis information for all axes
	virtual QList<AMAxisInfo> axes() const { return axes_; }
	/// Returns the rank (number of dimensions) of this data set: 2 (image)
	virtual int rank() const { return 2; }
	/// Returns the size of (ie: count along) each dimension
	virtual AMnDIndex size() const {
		return AMnDIndex(pixelsX_, pixelsY_);
	}
	/// Returns the size along a single axis \c axisNumber. This should be fast. \c axisNumber is assumed to be between 0 and rank()-1.
	virtual int size(int axisNumber) const {
		return axes_.at(axisNumber).size;
	}
	/// Returns a bunch of information about a particular axis. \c axisNumber is assumed to be between 0 and rank()-1.
	virtual AMAxisInfo axisInfoAt(int axisNumber) const {
		return axes_.at(axisNumber);
	}
	/// Returns the id of an axis, by name. (By id, we mean the index of the axis. We called it id to avoid ambiguity with indexes <i>into</i> axes.) This could be slow, so users shouldn't call it repeatedly. Returns -1 if not found.
	virtual int idOfAxis(const QString& axisName) const
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
	virtual AMNumber value(const AMnDIndex& indexes) const {
		if(!isConnected_)
			return AMNumber();
		if(indexes.rank() != 2)
			return AMNumber(AMNumber::DimensionError);
		if(indexes.i() >= pixelsX_ || indexes.j() >= pixelsY_)
			return AMNumber(AMNumber::OutOfBoundsError);

		return imagePV_->getInt(indexes.i()*pixelsY_ + indexes.j());
	}

	virtual bool values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const {
		if(!isConnected_)
			return false;
		if(indexStart.rank() != 2 || indexEnd.rank() != 2)
			return false;
		if(indexEnd.i() < indexStart.i() || indexEnd.j() < indexStart.j())
			return false;
		if(indexEnd.i() >= pixelsX_ || indexEnd.j() >= pixelsY_)
			return false;
		foreach(int v, imagePV_->lastIntegerValues())
			*(outputValues++) = double(v);

		return true;
	}

	/// When the independent values along an axis is not simply the axis index, this returns the independent value along an axis (specified by axis number and index)
	virtual AMNumber axisValue(int axisNumber, int index) const {
		Q_UNUSED(axisNumber)
		return index;
	}

	/// Performance optimization of axisValue():  instead of a single value, copies a block of values from \c startIndex to \c endIndex in \c outputValues.  The provided pointer must contain enough space for all the requested values.
    virtual bool axisValues(int axisNumber, int startIndex, int endIndex, double *outputValues) const
	{
		Q_UNUSED(axisNumber);

		for (int i = 0, size = endIndex-startIndex+1; i < size; i++)
            outputValues[i] = double(startIndex + i);

		return true;
	}

protected slots:
	/// Called whenever the connection state of any PV changes: emits valuesChanged(), sizeChanged(), and stateChanged() as required.
	void onConnectionStateChanged();
	/// Called when the image PV changes. emits valuesChanged().
	void onImageValuesChanged();

protected:
	/// Retains axis information
	QList<AMAxisInfo> axes_;
	/// PVs to access detector data
	AMProcessVariable* imagePV_, *resolutionXPV_, *resolutionYPV_;
	/// Connection state: true if all 3 PVs are connected
	bool isConnected_;
	/// Caches the size of the image, for speed. Same as axes_.at(0).size and axes_.at(1).size
	int pixelsX_, pixelsY_;


};

/// This object is a "live" link that can be used to control the MCP detector in the REIXS spectrometer, and access its real-time data.  It exposes the accumulated image, and the instantaneous image on the detector, as AMDataSources. It also provides methods to clear the accumulated image.
/*! This implementation is tied to the usb2401 Epics Driver and digital interface module from the MCP position analyzer. \todo Re-implement an abstract version of an imaging detector interface that can be used by general scan controllers? */
class REIXSXESMCPDetectorPre2013 : public QObject
{
	Q_OBJECT
public:
	/// Construct a new detector object.   \c name is an abitrary name, that will be used for the data sources. \c basePVName is the base of the Process Variable names we use to read and control the detector (ex: [basePVName]:image, [basePVName]:clear, etc.)
	REIXSXESMCPDetectorPre2013(const QString& name, const QString& basePVName, QObject *parent = 0);

	virtual ~REIXSXESMCPDetectorPre2013();

	QString name() const { return name_; }
	QString description() const { return description_; }
	void setName(const QString& name) { name_ = name; }
	void setDescription(const QString& description) { description_ = description; }

	// Is the detector "live" and connected?
	//////////////////

	/// Is the connection to the detector up, and able to read values/images?
	bool canRead() const {
		return totalCountsPV_->canRead() && countsPerSecondPV_->canRead() && orientationControl_->canMeasure() && imagePV_->canRead() && instantaneousImagePV_->canRead();
	}

	/// Are we able to configure the detector? (Connection established and permission to write to the configure PVs)
	bool canConfigure() const {
		return orientationControl_->isConnected() && averagingPeriodSecsControl_->isConnected() && persistTimeSecsControl_->isConnected() && clearPV_->canWrite();
	}

	/// \todo Signals when canRead() / canConfigure() change? (or an isConnected() and signals for that?)

	// Reading the detector
	////////////////////////

	/// Access a data source corresponding to the detector's accumulated image
	AMDataSource* image() const { return image_; }
	/// Access a data source corresponding to the detector's real-time (persist-delay) image
	AMDataSource* instantaneousImage() const { return instantaneousImage_; }

	/// What is the size of the detector?
	AMnDIndex size() const { return image_->size(); }

	/// Read the total counts in the image:
	double totalCounts() const { return totalCountsPV_->lastValue(); }
	/// Access the counts per second at this instant in time
	double countsPerSecond() const { return countsPerSecondPV_->lastValue(); }
	/// Return the orientation configuration of the detector (0 for horizontal, 1 for vertical)
	bool orientation() const { return (int)orientationControl_->value(); }

	/// AMDataSource* image() exposes the data nicely for plotting or analysis. For performance, sometimes it's nice to have a raw pointer to the image data arrays. This returns a vector of the raw image data, with the y-axis varying the fastest.  Returning this vector is fast because of QVector's implicit sharing.
	QVector<int> imageData() const { return imagePV_->lastIntegerValues(); }

	/// Returns the raw instantaneous image data, organized with the y-axis varying the fastest. Returning this vector is fast, because of QVector's implicit sharing.
	QVector<int> instantaneousImageData() const { return instantaneousImagePV_->lastIntegerValues(); }

	/// Returns (or casts) this detector as an REIXSXESMCPDetectorInfo, which contains this detector's current configuration information but not the live control abilities.
	operator REIXSXESMCPDetectorInfo() {
		REIXSXESMCPDetectorInfo rv(name(), description());
		rv.setSize(image_->size());
		rv.setOrientation(orientation());
		return rv;
	}

	// Controlling the detector
	/////////////////////////////

	/// Change the detector's configuration RIGHT NOW, from a saved set (REIXSXESDetectorInfo) of values. Note: the REIXSXESMCPDetectorInfo::size() of \c info is ignored.  Currently the orientation is ignored as well.
	bool setFromInfo(const REIXSXESMCPDetectorInfo& info) {
		bool success = true;

		setName(info.name());
		setDescription(info.description());

		/*
		/// \todo: set HV. (No current way to set it: no PV control)
		removed for now: success |= setOrientation(info.orientation());
		*/

		return success;
	}

	AMControl* orientationControl() { return orientationControl_; }
	AMControl* averagingPeriodControl() { return averagingPeriodSecsControl_; }
	AMControl* persistDurationControl() { return persistTimeSecsControl_; }



public slots:

	/// Clear the accumulated detector image
	bool clearImage() {
		if(clearPV_->canWrite()) {
			clearPV_->setValue(1);
			return true;
		}
		return false;
	}

	/// set the orientation (swap the way the detector interprets X and Y axes)
	bool setOrientation(bool isVertical) {
		if(orientationControl_->canMove()) {
			if(isVertical)
				orientationControl_->move(1.0);
			else
				orientationControl_->move(0.0);
			return true;
		}
		return false;
	}

	/// set the number of seconds over which the count rate is averaged
	bool setAveragingPeriod(double seconds) {
		if(averagingPeriodSecsControl_->canMove()) {
			averagingPeriodSecsControl_->move(seconds);
			return true;
		}
		return false;
	}

	/// set the number of seconds that counts persist for on the instantaneous image
	bool setPersistDuration(double seconds) {
		if(persistTimeSecsControl_->canMove()) {
			persistTimeSecsControl_->move(seconds);
			return true;
		}
		return false;
	}



signals:

	/// Emitted whenever the countsPerSecond changes
	void countsPerSecondChanged(double);
	/// Emitted whenever the total number of counts changes
	void totalCountsChanged(double);
	/// Emitted when the image and imageData() changes (ie: has new values)
	void imageDataChanged();
	/// Emitted when the instantaneousImage() and instantaneousImageData() changes (ie: has new values)
	void instantaneousImageDataChanged();

public slots:

protected:
	AMProcessVariable* totalCountsPV_;
	AMProcessVariable* countsPerSecondPV_;
	AMProcessVariable* imagePV_;
	AMProcessVariable* instantaneousImagePV_;
	AMProcessVariable* resolutionXPV_;
	AMProcessVariable* resolutionYPV_;

	AMProcessVariable* clearPV_;
	AMPVControl* orientationControl_;
	AMPVControl* averagingPeriodSecsControl_;
	AMPVControl* persistTimeSecsControl_;

	REIXSXESMCPDataSourcePre2013* image_, *instantaneousImage_;

	QString basePVName_;
	QString name_, description_;



};

#endif // REIXSXESMCPDETECTORPRE2013_H
