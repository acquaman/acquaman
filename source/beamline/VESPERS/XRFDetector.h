#ifndef XRFDETECTOR_H
#define XRFDETECTOR_H

#include "dataman/VESPERS/XRFDetectorInfo.h"
#include "beamline/AMDetector.h"
#include "beamline/AMControlSet.h"
#include "beamline/AMROI.h"
#include "util/VESPERS/XRFElement.h"
#include "util/VESPERS/GeneralUtilities.h"

#include "dataman/AMProcessVariableDataSource.h"

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

	// Data sources
	///////////////////////////////////////
	/// Returns the raw spectrum data source at \c index.  It is assumed that the data sources will be in order of element.  Must be between 0 and size()-1.
	AMProcessVariableDataSource *spectrumDataSource(int index) const { return spectrumDataSources_.at(index); }
	/// Returns the raw spectrum data sources.  Will be the size of the number of elements.
	QList<AMProcessVariableDataSource *> spectrumDataSources() const { return spectrumDataSources_; }
	/// Returns the dead time data source at \c index.  It is assumed that the data sources will be in order of the element.  Must be between 0 and size() -1.
	AMProcessVariableDataSource *deadTimeDataSource(int index) const { return deadTimeDataSources_.at(index); }
	/// Returns the dead time data sources.  Will have the size of the number of elements.
	QList<AMProcessVariableDataSource *> deadTimeDataSources() const { return deadTimeDataSources_; }
	/// Returns the analyzed data source at \c index.  It is assumed that the data sources will be in order of the element.  The last element is the corrected sum if the number of elements is greater than one.
	AMDataSource *correctedDataSource(int index) const { return correctedSpectrumDataSources_.at(index); }
	/// Returns the analyzed data sources.  Will be the size of the number of elements plus one, except for the single element detectors.
	QList<AMDataSource *> correctedDataSources() const { return correctedSpectrumDataSources_; }
	/// Returns the corrected sum spectrum data source.  Convenience function.
	AMDataSource *correctedSumDataSource() const { return correctedSpectrumDataSources_.last(); }

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
	/// Adds a region of interest.  The new ROI is appended to the end of the list.  Returns whether the addition was successful or not; it will only fail if there are no longer any ROIs to place values in.
	bool addRegionOfInterest(XRFElement *el, QString line);
	/// Removes a region of interest.  Uses the name of the ROI to find and remove it.  Returns whether the remove was successful or not.
	bool removeRegionOfInterest(XRFElement *el, QString line);
	/// Clears the list of ROIs and clears the info list.
	void clearRegionsOfInterest();
	/// Sorts the list of ROIs.
	void sort();


signals:
	/// Only emitted as true when all of the controls in the detector are connected. Is emitted false when any of the controls within the detector become unconnected.
	void detectorConnected(bool);
	/// This signal is emitted when the status changes.
	void statusChanged();
	/// Notifies that the maximum energy has changed.
	void maximumEnergyChanged(double);
	/// Notifies that the peaking time has changed.
	void peakingTimeChanged(double);
	/// Notifies that the integration time has changed.
	void integrationTimeChanged(double);
	/// Notifies that the elapsed time has changed.
	void elapsedTimeChanged(double);
	/// Notifies when the spectra refresh rate has changed.
	void refreshRateChanged(double);
	/// Notifies that the dead time has changed.  If the number of elements is greater than one, then this is emitted when any of the dead times change.
	void deadTimeChanged();
	/// Notifies that the spectra have changed.  If there is more than one spectrum then this is emitted everytime one of them changes.
	void spectraChanged();
	/// Signal used to say that the regions of interest now have their original values in them after being connected to.
	void roisHaveValues(bool);
	/// Notifier that the contents of an ROI has changed.  It passes a pointer to the particular ROI that has been changed.
	void roiUpdate(AMROI *);
	/// Notifier that a region of interest has been added.  Passes the region of interest information.
	void addedRegionOfInterest(AMROIInfo);
	/// Notifier that a region of interest has been removed.  Passes the region of interest information.
	void removedRegionOfInterest(AMROIInfo);
	/// This signal is meant to notify all classes that use this class that the ROI list has been dirtied.  This means that all classes should check the new ROI list to see if there are any changes that need to be addressed.
	void externalRegionOfInterestChanged();

protected slots:
	/// Determines if the detector is connected to ALL controls and process variables.
	void detectorConnected();
	/// Determines if the regions of interest in the detector all have values.
	void allRoisHaveValues();
	/// Determines if there is a discrepancy between the ROI list and the ROIInfo list and if there is, begins the sequence of updating the entire program.
	void onRoiNameUpdate();

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

	/// The list of all the raw spectrum data sources.
	QList<AMProcessVariableDataSource *> spectrumDataSources_;
	/// The list of all the dead times.  The order of the dead times is the same as the spectrum data sources.
	QList<AMProcessVariableDataSource *> deadTimeDataSources_;
	/// The corrected spectra.  If the number of elements is greater than one then the last spectra is always the corrected sum.
	QList<AMDataSource *> correctedSpectrumDataSources_;
};

#endif // XRFDETECTOR_H
