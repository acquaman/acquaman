#ifndef XRFDETECTOR_H
#define XRFDETECTOR_H

#include "dataman/VESPERS/XRFDetectorInfo.h"
#include "beamline/AMDetector.h"
#include "beamline/AMControlSet.h"
#include "beamline/AMROI.h"

#include "dataman/AMDataSource.h"
#include "dataman/AMAxisInfo.h"

/*!
  This class encapsulates AMProcessVariable and puts them into an AMDataSource.  This will allow easy insertion into MPlots for viewing purposes.
  */

class XRFDetectorDataSource : public QObject, public AMDataSource
{
	Q_OBJECT

public:
	/// Constructor.  Takes in an AMReadOnlyPVControl.
	XRFDetectorDataSource(const AMProcessVariable *data, const QString& name, QObject *parent = 0);

	// Data source type
	//////////////////////////

	/// Human-readable description of the type of data source this is (ex: "One-dimensional math expression").  Subclasses should re-implement this.
	virtual QString typeDescription() const { return "XRF Detector Data Source"; }

	// State of the data
	//////////////////////////
	/// Returns an OR-combination of StateFlags describing the current state of the data. The base class interface indicates that it does not have valid data. Implementing classes should return InvalidFlag when they don't have valid data, and/or ProcessingFlag if their data might be changing. No flags indicate the data is valid and generally static.
	virtual int state() const { return data_->hasValues() ? ProcessingFlag : InvalidFlag; }

	// Axis Information
	//////////////////////
	/// Returns axis information for all axes
	virtual QList<AMAxisInfo> axes() const
	{
		QList<AMAxisInfo> axisInfo;
		axisInfo << AMAxisInfo("n", data_->count(), "n - Channel Number");
		return axisInfo;
	}

	// Following can all be determined from above. Included anyway for convenience of classes that use the interface, and for performance. Calling size(axisNumber) should be fast; using axes() to return a full list of AMAxisInfo and extracting the desired axis would be much slower.
	//////////////////////////////////////////////
	/// Returns the rank (number of dimensions) of this data set
	virtual int rank() const { return axes().count(); }
	/// Returns the size of (ie: count along) each dimension
	virtual AMnDIndex size() const { return AMnDIndex(axes().count()); }
	/// Returns the size along a single axis \c axisNumber. This should be fast. \c axisNumber is assumed to be between 0 and rank()-1.
	virtual int size(int axisNumber) const { return axes().at(axisNumber).size; }
	/// Returns a bunch of information about a particular axis. \c axisNumber is assumed to be between 0 and rank()-1.
	virtual AMAxisInfo axisInfoAt(int axisNumber) const { return axes().at(axisNumber); }
	/// Returns the id of an axis, by name. (By id, we mean the index of the axis. We called it id to avoid ambiguity with indexes <i>into</i> axes.) This could be slow, so users shouldn't call it repeatedly. Returns -1 if not found.
	virtual int idOfAxis(const QString& axisName)
	{
		for (int i = 0; i < axes().count(); i++)
			if (axes().at(i).name.compare(axisName) == 0)
				return i;

		return -1;
	}

	// Data value access
	////////////////////////////

	/// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient or any are out of range, or if the data is not ready.
	virtual AMNumber value(const AMnDIndex& indexes) const
	{
		if(!data_->isConnected())
			return AMNumber();
		if(indexes.rank() != 1)
			return AMNumber(AMNumber::DimensionError);
		if(indexes.i() >= axes().first().size)
			return AMNumber(AMNumber::OutOfBoundsError);

		return data_->getInt(indexes.i());
	}
	/// When the independent values along an axis is not simply the axis index, this returns the independent value along an axis (specified by axis number and index)
	virtual AMNumber axisValue(int axisNumber, int index) const
	{
		Q_UNUSED(axisNumber)

		return index*scale();
	}

	/// Returns the current scale used for the independent axis.
	double scale() const { return scale_; }

public slots:
	/// Changes the scale used in the plot if that scale changes.
	void setScale(double scale) { scale_ = scale; onDataChanged(); }

protected slots:
	/// Emits the data changed signal when the control gets new data.
	void onDataChanged();

protected:
	/// The control being used as a data source.
	const AMProcessVariable *data_;
	/// Holds the current scale.
	double scale_;
};

class XRFDetector : public XRFDetectorInfo, public AMDetector
{
	Q_OBJECT
public:
	/// Constructor.  Requires all the AMProcessVariables and AMControls required to function.  These should remain static for the entirety of the life of this class and therefore setters are no provided.  The ROIs are set using the setRoiList method.  All the ROIs must be created previously.
	XRFDetector(QString name, int elements, AMControl *status, AMControl *refreshRate, AMControl *peakingTime, AMControl *maximumEnergy, AMControl *integrationTime, AMControl *liveTime, AMControl *elapsedTime, AMControl *start, AMControl *stop, AMControlSet *deadTime, AMControlSet *spectra, QObject *parent = 0);
	/// Constructor.  Convenience version for single element detectors.
	XRFDetector(QString name, AMControl *status, AMControl *refreshRate, AMControl *peakingTime, AMControl *maximumEnergy, AMControl *integrationTime, AMControl *liveTime, AMControl *elapsedTime, AMControl *start, AMControl *stop, AMControl *deadTime, AMControl *spectra, QObject *parent = 0);

	~XRFDetector();

	/// Return the meta object.
	const QMetaObject *getMetaObject() { return metaObject(); }

	/// Returns the description for the detector.
	virtual QString description() const { return XRFDetectorInfo::description(); }

	/// Transforms current settings into a detector into. Returns a new instance-- caller is responsible for memory.
	virtual AMDetectorInfo *toInfo() const { return new XRFDetectorInfo(*this); }
	/// Transforms current settings into a detector info.
	XRFDetectorInfo toXRFInfo() const { return XRFDetectorInfo(*this); }
	/// Takes a detector info and sets all the settings for the detector.
	virtual bool setFromInfo(const AMDetectorInfo *info);
	/// Takes in a detector info and sets all the settings for the detector.
	void fromXRFInfo(const XRFDetectorInfo &info);

	/// Holds whether the entire detector is connected.
	bool isConnected() const { return detectorConnected_; }
	/// Holds whether the detector was connected previously.  Primarily useful at startup.
	bool wasConnected() const { return wasConnected_; }

	// Getters that aren't included in the info.  These are convenience functions that grab the current value from the control.
	//////////////////////////////////////////////////

	/// Returns the current elapsed time.
	double elapsedTime() const { return elapsedTimeControl()->value(); }
	/// Returns the current dead time. For detectors with more than one element, it returns the highest value.
	double deadTime() const;
	/// Returns the status as an int.  1 is acquiring, 0 is done.
	int status() const { return (int)statusControl_->value(); }

	// End of getters that aren't included in the info.
	/////////////////////////////////////////////////////

	// Controls and PVs.
	////////////////////////////////

	/// Returns the status control.
	AMControl *statusControl() const { return statusControl_; }
	/// Returns the refresh rate control.
	AMControl *refreshRateControl() const { return refreshRateControl_; }
	/// Returns the peaking time control.
	AMControl *peakingTimeControl() const { return peakingTimeControl_; }
	/// Returns the maximum energy control.
	AMControl *maximumEnergyControl() const { return maximumEnergyControl_; }
	/// Returns the integration time control.
	AMControl *integrationTimeControl() const { return integrationTimeControl_; }
	/// Returns the live time control.
	AMControl *liveTimeControl() const { return liveTimeControl_; }
	/// Returns the elapsed time control.
	AMControl *elapsedTimeControl() const { return elapsedTimeControl_; }
	/// Returns the dead time control set.
	AMControlSet *deadTimeControl() const { return deadTimeControl_; }
	/// Returns the spectra control set.
	AMControlSet *spectraControl() const { return spectraControl_; }
	/// Returns the start control.
	AMControl *startControl() const { return startControl_; }
	/// Returns the stop control.
	AMControl *stopControl() const { return stopControl_; }

	// End of Controls and PVs.
	////////////////////////////////

	/// Turns the spectra controls into an array of doubles and returns the spectra at \c index.
	const int *spectraAt(int index);

	/// Enables a previously disabled element.  Takes the \param id as an index of the list of elements.
	void enableElement(int id);
	/// Disables a previously enabled element.  Takes the \param id as an index of the list of elements.
	void disableElement(int id);

	/// Returns the AMROI list.
	QList<AMROI *> roiList() const { return roiList_; }
	/// Sets the AMROI list.  This should be called only ONCE.
	void setRoiList(QList<AMROI *> list);

	/// Adds a region of interest.  The new ROI is appended to the end of the list.  Returns whether the addition was successful or not; it will only fail if there are no longer any ROIs to place values in.
	bool addRegionOfInterest(AMROIInfo roi);
	/// Removes a region of interest.  Uses the name of the ROI to find and remove it.  Returns whether the remove was successful or not.
	bool removeRegionOfInterest(QString name);
	/// Sorts the list of ROIs.
	void sort();

	// Data sources
	///////////////////////////////////////
	/// Returns the data source for element \c index.  It is assumed that the data sources will be in order of element.  Must be between 0 and size()-1 where size is the number of elements in the detector.
	XRFDetectorDataSource *dataSource(int index) const { return dataSources_.at(index); }
	/// Overloaded function.  Convenience function for grabbing the first data source.
	XRFDetectorDataSource *dataSource() const { return dataSources_.first(); }
	/// Returns the list of all the raw data sources in the detector as a QList.
	QList<XRFDetectorDataSource *> dataSources() const { return dataSources_; }

public slots:

	/// Erases the current spectrum and starts collecting data.
	void start() { startControl()->move(1); }
	/// Stops collection of data.
	void stop() { stopControl()->move(1); }
	/// Set the accumulation time.
	void setTime(double time);
	/// Set the maximum energy of the detector.
	void setMaximumEnergyControl(double energy);
	/// Sets the peaking time of the detector.
	void setPeakingTimeControl(double time);
	/// Sets the spectrum refresh rate.
	void setRefreshRateControl(XRFDetectorInfo::MCAUpdateRate rate);
	/// Sets the size of the spectra channels.
	void setChannelSize();
	/// Sets the description of the detector.
	void setDescription(const QString &description) { XRFDetectorInfo::setDescription(description); }


signals:
	/// Only emitted as true when all of the controls in the detector are connected. Is emitted false when any of the controls within the detector become unconnected.
	void detectorConnected(bool);
	/// This signal is emitted when the status changes.
	void statusChanged();
	/// Signal used to say that the regions of interest now have their original values in them after being connected to.
	void roisHaveValues(bool);

protected slots:
	/// Determines if the detector is connected to ALL controls and process variables.
	void detectorConnected();

protected:

	/// Bool handling whether the detector is connected.
	bool detectorConnected_;
	/// Bool handling whether the detector was connected.
	bool wasConnected_;

	/// The list of regions of interest.
	QList<AMROI *> roiList_;

	// The controls and PVs.
	/// The status of the scan.
	AMControl *statusControl_;
	/// The spectra refresh rate.
	AMControl *refreshRateControl_;
	/// The peaking time.
	AMControl *peakingTimeControl_;
	/// The maximum energy.
	AMControl *maximumEnergyControl_;
	/// The integration time.
	AMControl *integrationTimeControl_;
	/// The live time.
	AMControl *liveTimeControl_;
	/// The elapsed time.
	AMControl *elapsedTimeControl_;
	/// The dead time.  This is a control set because the number of elements can be greater than one.
	AMControlSet *deadTimeControl_;
	/// The spectra.  This is a control set because the number of elements can be greater than one.
	AMControlSet *spectraControl_;
	/// The start control.
	AMControl *startControl_;
	/// The stop control.
	AMControl *stopControl_;

	// Helper control sets.
	/// Control set holding the controls used for reading.
	AMControlSet *readingControls_;
	/// Control set holding the controls used for setting the configuration of the detector.
	AMControlSet *settingsControls_;

	/// The list of data sources.
	QList<XRFDetectorDataSource *> dataSources_;

private:
	// Keeps track of whether the single element version was used or not.
	bool usingSingleElement_;
};

#endif // XRFDETECTOR_H
