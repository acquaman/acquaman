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


#ifndef XRFDETECTOR_H
#define XRFDETECTOR_H

#include "dataman/VESPERS/XRFDetectorInfo.h"
#include "beamline/AMDetector.h"
#include "beamline/AMROI.h"
#include "util/VESPERS/XRFElement.h"
#include "util/VESPERS/GeneralUtilities.h"
#include "dataman/datasource/AMProcessVariableDataSource.h"
#include "beamline/AMProcessVariable.h"

#include <QTimer>

class XRFDetector : public XRFDetectorInfo, public AMDetector
{
	Q_OBJECT
public:

	/*! This is the enum for the spectrum update rate.

			- Passive waits until the accumlation time has expired before updating the spectrum.
			- Slow waits for a moderate amount of time before asking the spectrum for its current state.  Typically around 1 second.
			- Fast waits for a very short amount of time before asking the spectrum for its current state.  Typically 200 msec.
	  */
	enum MCAUpdateRate { Passive, Slow, Fast };

	/// Constructor.  Requires the name, number of elements, and the base PV name of the detector.  It builds all the PV's and connects the accordingly based on standard naming conventions.
	XRFDetector(QString name, int elements, QString baseName, QObject *parent = 0);

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

	/// Holds whether the detector was connected previously.  Primarily useful at startup.
	bool wasConnected() const { return wasConnected_; }

	// Getters that aren't included in the info.  These are convenience functions that grab the current value from the control.
	//////////////////////////////////////////////////

	/// Returns the current elapsed time.
	double elapsedTime() const { return elapsedTimePV_.first()->getDouble(); }
	/// Returns the current dead time. For detectors with more than one element, it returns the highest value.
	double deadTime();
	/// Returns the status as an bool.  true is acquiring, false is done.
	bool status() const
	{
		if (statusPV_.first()->getInt() == 1)
			return true;

		return false;
	}
	/// The refresh rate for the spectra while data is being collected.  For instance, you might want to be seeing what the spectrum looks like in a 100 second integration time, but updated every second.  This is an enum parameter.
	MCAUpdateRate refreshRate() const { return refreshRate_; }

	// End of getters that aren't included in the info.
	/////////////////////////////////////////////////////

	/// Turns the spectra controls into an array of doubles and returns the spectra at \c index.
	QVector<int> spectraValues(int index);
	/// Takes the current value of the dead time from \c index.
	double deadTimeAt(int index);
	/// Takes the current value of the input count rate from \c index.
	double inputCountRate(int index) { return icrDataSource(index)->value(AMnDIndex()); }
	/// Takes the current value of the output count rate from \c index.
	double outputCountRate(int index) { return ocrDataSource(index)->value(AMnDIndex()); }

	/// Enables a previously disabled element.  Takes the \param id as an index of the list of elements.
	void enableElement(int id);
	/// Disables a previously enabled element.  Takes the \param id as an index of the list of elements.
	void disableElement(int id);

	/// Returns the AMROI list.
	QList<AMROI *> roiList() const { return roiList_; }

	// Data sources
	///////////////////////////////////////
	/// Returns the raw spectrum data source at \c index.  It is assumed that the data sources will be in order of element.  Must be between 0 and size()-1.
	AM1DProcessVariableDataSource *spectrumDataSource(int index) const { return spectrumDataSources_.at(index); }
	/// Returns the raw spectrum data sources.  Will be the size of the number of elements.
	QList<AM1DProcessVariableDataSource *> spectrumDataSources() const { return spectrumDataSources_; }
	/// Returns the input count rate data source at \c index.  It is assumed that the data sources will be in order of the element.  Must be between 0 and size() -1.
	AM0DProcessVariableDataSource *icrDataSource(int index) const { return icrDataSources_.at(index); }
	/// Returns the input count rate data sources.  Will have the size of the number of elements.
	QList<AM0DProcessVariableDataSource *> icrDataSources() const { return icrDataSources_; }
	/// Returns the input count rate data source at \c index.  It is assumed that the data sources will be in order of the element.  Must be between 0 and size() -1.
	AM0DProcessVariableDataSource *ocrDataSource(int index) const { return ocrDataSources_.at(index); }
	/// Returns the input count rate data sources.  Will have the size of the number of elements.
	QList<AM0DProcessVariableDataSource *> ocrDataSources() const { return ocrDataSources_; }
	/// Returns the analyzed data source at \c index.  It is assumed that the data sources will be in order of the element.  The last element is the corrected sum if the number of elements is greater than one.
	AMDataSource *correctedDataSource(int index) const { return correctedSpectrumDataSources_.at(index); }
	/// Returns the analyzed data sources.  Will be the size of the number of elements plus one, except for the single element detectors.
	QList<AMDataSource *> correctedDataSources() const { return correctedSpectrumDataSources_; }
	/// Returns the corrected sum spectrum data source.  Convenience function.
	AMDataSource *correctedSumDataSource() const { return correctedSpectrumDataSources_.last(); }

public slots:

	/// Erases the current spectrum and starts collecting data.
	void start() { timer_.stop(); startPV_->setValue(1); }
	/// Stops collection of data.
	void stop() { stopPV_->setValue(1); }
	/// Set the accumulation time.
	void setTime(double time);
	/// Set the maximum energy of the detector.  \c energy is in eV.
	void setMaximumEnergyControl(double energy);
	/// Sets the peaking time of the detector.
	void setPeakingTimeControl(double time);
	/// Sets the refresh rate.  This includes the spectra and status.
	void setRefreshRate(MCAUpdateRate rate)
	{
		setSpectraRefreshRate(rate);
		setStatusRefreshRate(rate);
	}
	/// Sets the spectra update rate.
	void setSpectraRefreshRate(MCAUpdateRate rate)
	{
		switch(rate){
		case Passive:
			for (int i = 0; i < elements_; i++)
				mcaUpdateRatePV_.at(i)->setValue(0);
			break;
		case Slow:
			for (int i = 0; i < elements_; i++)
				mcaUpdateRatePV_.at(i)->setValue(6);
			break;
		case Fast:
			for (int i = 0; i < elements_; i++)
				mcaUpdateRatePV_.at(i)->setValue(8);
			break;
		}
	}
	/// Sets the status refresh rate.
	void setStatusRefreshRate(MCAUpdateRate rate)
	{
		switch(rate){
		case Passive:
			for (int i = 0; i < elements_; i++)
				statusUpdateRatePV_.at(i)->setValue(0);
			break;
		case Slow:
			for (int i = 0; i < elements_; i++)
				statusUpdateRatePV_.at(i)->setValue(6);
			break;
		case Fast:
			for (int i = 0; i < elements_; i++)
				statusUpdateRatePV_.at(i)->setValue(8);
			break;
		}
	}

	/// Sets the size of the spectra channels.
	void setChannelSize();
	/// Sets the description of the detector.
	void setDescription(const QString &description) { XRFDetectorInfo::setDescription(description); }
	/// Adds a region of interest.  The new ROI is appended to the end of the list.  Returns whether the addition was successful or not; it will only fail if there are no longer any ROIs to place values in.
	bool addRegionOfInterest(XRFElement *el, QString line);
	/// Adds a region of interest.  If propogateToDetector is true, then this is the same as addRegionOfInterest. If false, then only the info list is updated.  The reason for the distinction is when we are reading the detector to determine existing regions of interest rather than looking at the ones that have been created from within the program.
	bool addRegionOfInterest(XRFElement *el, QString line, bool propogateToDetector);
	/// Removes a region of interest.  Uses the name of the ROI to find and remove it.  Returns whether the remove was successful or not.
	bool removeRegionOfInterest(XRFElement *el, QString line);
	/// Clears the list of ROIs and clears the info list.
	void clearRegionsOfInterest();
	/// Sorts the list of ROIs.
	void sortRegionsOfInterest();


signals:
	/// Only emitted as true when all of the controls in the detector are connected. Is emitted false when any of the controls within the detector become unconnected.
	void connected(bool);
	/// This signal is emitted when the status changes.  Passes the state as a bool.  True is acquiring, false is done.
	void statusChanged(bool);
	/// Notifies that the maximum energy has changed.
	void maximumEnergyChanged(double);
	/// Notifies that the peaking time has changed.
	void peakingTimeChanged(double);
	/// Notifies that the integration time has changed.
	void integrationTimeChanged(double);
	/// Notifies that the elapsed time has changed.
	void elapsedTimeChanged(double);
	/// Notifies when the spectra refresh rate has changed.
	void refreshRateChanged(MCAUpdateRate rate);
	/// Same signal, but as an int.
	void refreshRateChanged(int rate);
	/// Notifies that the dead time has changed.  If the number of elements is greater than one, then this is emitted when any of the dead times change.
	void deadTimeChanged();
	/// Signal used to say that the regions of interest now have their original values in them after being connected to.
	void roisHaveValues();
	/// Notifier that a region of interest has been added.  Passes the region of interest information.
	void addedRegionOfInterest(AMROIInfo);
	/// Notifier that a region of interest has been removed.  Passes the region of interest information.
	void removedRegionOfInterest(AMROIInfo);
	/// This signal is meant to notify all classes that use this class that the ROI list has been dirtied.  This means that all classes should check the new ROI list to see if there are any changes that need to be addressed.
	void externalRegionsOfInterestChanged();
	/// Passes on the signal that an ROI has been updated.
	void roiUpdate(AMROI *);

protected slots:
	/// Determines if the detector is connected to ALL controls and process variables.
	void isDetectorConnected();
	/// Connects and disconnects signals based on the connection of the detector.
	void onConnectedChanged(bool isConnected);
	/// Determines if the regions of interest in the detector all have values.
	void allRoisHaveValues();
	/// Determines if there is a discrepancy between the ROI list and the ROIInfo list and if there is, begins the sequence of updating the entire program.
	void onUpdateTimer();
	/// Handles restarting the timer after the detector is finished acquiring.  It also emits the statusChanged signal.
	void onStatusChanged()
	{
		bool currStatus = status();

		if (currStatus)
			timer_.start();

		emit statusChanged(currStatus);
	}
	/// Handles changes to the spectra refresh rate.
	void onRefreshRateChanged(int rate)
	{
		switch(rate){
		case 0:
			refreshRate_ = Passive;
			emit refreshRateChanged(Passive);
			emit refreshRateChanged(0);
			break;
		case 6:
			refreshRate_ = Slow;
			emit refreshRateChanged(Slow);
			emit refreshRateChanged(1);
			break;
		case 8:
			refreshRate_ = Fast;
			emit refreshRateChanged(Fast);
			emit refreshRateChanged(2);
			break;
		}
	}
	/// Helper slot.  Sets the status update rate to Fast once it is connected.
	void onStatusUpdateRateInitialized() { setStatusRefreshRate(Fast); }
	/// Handles changes to the peaking time.
	void onPeakingTimeChanged(double pktime){ setPeakingTime(pktime); emit peakingTimeChanged(pktime); }
	/// Handles changes to the maximum energy.
	void onMaximumEnergyChanged(double maxE){ setMaximumEnergy(maxE); emit maximumEnergyChanged(maxE); }
	/// Handles changes to the integration time.
	void onIntegrationTimeChanged(double time){ setIntegrationTime(time); emit integrationTimeChanged(time); }
	/// Handles making sure the ROI info list is up to date with the latest values based on changes to AMROIs.
	void onAMROIUpdate(AMROI *roi);

protected:
	/// Helper function.  Takes in a base name and creates a list of ROIs based on the number of elements.  Creates PVs for the name, low limit, high limit, and current value.
	void createROIList(QString baseName);

	/// Bool handling whether the detector was connected.
	bool wasConnected_;
	/// The refresh rate.
	MCAUpdateRate refreshRate_;

	/// Timer used to periodically update the ROI lists.
	QTimer timer_;

	// The PVs.  They are all lists because the detector could have more than one element.  Start and stop are special because naming conventions didn't allow for the correct behaviour.
	/// The status of the scan.
	QList<AMProcessVariable *> statusPV_;
	/// The spectra refresh rate.
	QList<AMProcessVariable *> mcaUpdateRatePV_;
	/// The status refresh rate.
	QList<AMProcessVariable *> statusUpdateRatePV_;
	/// The peaking time.
	QList<AMProcessVariable *> peakingTimePV_;
	/// The maximum energy.
	QList<AMProcessVariable *> maximumEnergyPV_;
	/// The integration time.
	QList<AMProcessVariable *> integrationTimePV_;
	/// The live time.
	QList<AMProcessVariable *> liveTimePV_;
	/// The elapsed time.
	QList<AMProcessVariable *> elapsedTimePV_;
	/// The input count rate.
	QList<AMProcessVariable *> icrPV_;
	/// The output count rate.
	QList<AMProcessVariable *> ocrPV_;
	/// The start PV.
	AMProcessVariable * startPV_;
	/// The stop PV.
	AMProcessVariable * stopPV_;
	/// The spectra.
	QList<AMProcessVariable *> spectraPV_;

	/// The list of regions of interest.
	QList<AMROI *> roiList_;

	/// The list of all the raw spectrum data sources.
	QList<AM1DProcessVariableDataSource *> spectrumDataSources_;
	/// The list of all the input count rates.  The order of the dead times is the same as the spectrum data sources.
	QList<AM0DProcessVariableDataSource *> icrDataSources_;
	/// The list of all the output count rates.  The order of the dead times is the same as the spectrum data sources.
	QList<AM0DProcessVariableDataSource *> ocrDataSources_;
	/// The corrected spectra.  If the number of elements is greater than one then the last spectra is always the corrected sum.
	QList<AMDataSource *> correctedSpectrumDataSources_;
};

#endif // XRFDETECTOR_H
