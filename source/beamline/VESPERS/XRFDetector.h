#ifndef XRFDETECTOR_H
#define XRFDETECTOR_H

#include "dataman/VESPERS/XRFDetectorInfo.h"
#include "beamline/AMDetector.h"
#include "beamline/AMControlSet.h"
#include "beamline/AMROI.h"

class XRFDetector : public XRFDetectorInfo, public AMDetector
{
	Q_OBJECT
public:
	/// Constructor.  Requires all the AMProcessVariables and AMControls required to function.  These should remain static for the entirety of the life of this class and therefore setters are no provided.
	XRFDetector(QString name, int elements, AMControl *refreshRate, AMControl *peakingTime, AMControl *maximumEnergy, AMControl *integrationTime, AMControl *liveTime, AMControl *elapsedTime, AMControl *start, AMControl *stop, AMControlSet *deadTime, AMControlSet *spectra, QObject *parent = 0);
	/// Constructor.  Convenience version for single element detectors.
	XRFDetector(QString name, AMControl *refreshRate, AMControl *peakingTime, AMControl *maximumEnergy, AMControl *integrationTime, AMControl *liveTime, AMControl *elapsedTime, AMControl *start, AMControl *stop, AMControl *deadTime, AMControl *spectra, QObject *parent = 0);

	~XRFDetector();

	/// Return the meta object.
	const QMetaObject *getMetaObject() { return metaObject(); }

	/// Transforms current settings into a detector info.
	XRFDetectorInfo toXRFInfo() const { return XRFDetectorInfo(*this); }
	/// Takes in a detector info and sets all the settings for the detector.
	void fromXRFInfo(const XRFDetectorInfo &info);

	/// Holds whether the entire detector is connected.
	bool isConnected() const { return detectorConnected_; }

	// Controls and PVs.
	////////////////////////////////

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

	// End of Controsl and PVs.
	////////////////////////////////

	/// Returns the AMROI list.
	QList<AMROI *> roiList() const { return roiList_; }

public slots:

	/// Erases the current spectrum and starts collecting data.
	void start() { startControl()->move(1); }
	/// Stops collection of data.
	void stop() { stopControl()->move(1); }
	/// Set the accumulation time.
	void setTime(double time) { setIntegrationTime(time); integrationTimeControl()->move(time); liveTimeControl()->move(0.0); }
	/// Set the maximum energy of the detector.
	void setMaximumEnergyControl(double energy) { setMaximumEnergy(energy); maximumEnergyControl()->move(energy); }
	/// Sets the peaking time of the detector.
	void setPeakingTimeControl(double time) { setPeakingTime(time); peakingTimeControl()->move(time); }
	/// Sets the spectrum refresh rate.
	void setRefreshRateControl(XRFDetectorInfo::MCAUpdateRate rate);

signals:
	/// Emitted when the connected state changes.
	void connected(bool isConnected);

protected slots:
	/// Determines if the detector is connected to ALL controls and process variables.
	void detectorConnected();

protected:

	/// Bool handling whether the ROIs are connected.
	bool detectorConnected_;

	/// The list of regions of interest.
	QList<AMROI *> roiList_;

	// The controls and PVs.
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
	AMControlSet *readingControls_;
	AMControlSet *settingsControls_;
};

#endif // XRFDETECTOR_H
