#ifndef XRFDETECTORINFO_H
#define XRFDETECTORINFO_H

#include "dataman/AMDetectorInfo.h"
#include "dataman/AMROIInfo.h"

/// This class contains the run-time configuration parameters for Silicon Drift detectors.
class XRFDetectorInfo : public AMDetectorInfo
{
	Q_OBJECT

	Q_PROPERTY(double maximumEnergy READ maximumEnergy WRITE setMaximumEnergy)
	Q_PROPERTY(int elements READ elements WRITE setElements)
	Q_PROPERTY(int activeElements READ activeElements WRITE setActiveElements)
	Q_PROPERTY(MCAUpdateRate refreshRate READ refreshRate WRITE setRefreshRate)
	Q_PROPERTY(double integrationTime READ integrationTime WRITE setIntegrationTime)
	Q_PROPERTY(double peakingTime READ peakingTime WRITE setPeakingTime)
	Q_PROPERTY(AMDbObject* roiList READ dbGetROIList WRITE dbLoadROIList)

	Q_CLASSINFO("AMDbObject_Attributes", "description=XRF Detector")

public:

	/*! This is the enum for the spectrum update rate.

			- Passive waits until the accumlation time has expired before updating the spectrum.
			- Slow waits for a moderate amount of time before asking the spectrum for its current state.  Typically around 1 second.
			- Fast waits for a very short amount of time before asking the spectrum for its current state.  Typically 200 msec.
	  */
	enum MCAUpdateRate { Passive, Slow, Fast };

	/// Default constructor.
	Q_INVOKABLE XRFDetectorInfo(const QString& name = "xrfSpectrum", const QString& description = "XRF Detector", QObject *parent = 0);

	// Dimensionality and size:
	///////////////////////////

	/// Returns the number of dimensions in the output of this detector.  A single point has rank 0.  A spectrum output would have rank 1.  An image output would have rank 2.
	virtual int rank() const { return 1; }
	/// Returns the size (ie: number of elements) along each dimension of the detector. For a spectrum output, this would contain one number (the number of pixels or points along the axis).  For an image output, this would contain the width and height.
	virtual AMnDIndex size() const { return AMnDIndex(channels_); }
	/// Returns a list of AMAxisInfo describing the size and nature of each detector axis, in order.
	virtual QList<AMAxisInfo> axes() const {
		QList<AMAxisInfo> axisInfo;
		axisInfo << AMAxisInfo("n", channels_, "n - Channel Number");
		return axisInfo;
	}

	// Properties
	///////////////////////////

	/// Number of channels in the spectral output.
	int channels() const { return channels_; }
	/// The maximum energy calibration for the detector.  Primary indicator for energy divisions per channel number.
	double maximumEnergy() const { return maxEnergy_; }
	/// The number of elements in the detector.
	int elements() const { return elements_; }
	/// The number of active elements.  The number currently being used (note: #active <= #elements).
	int activeElements() const { return activeElements_; }
	/// The refresh rate for the spectra while data is being collected.  For instance, you might want to be seeing what the spectrum looks like in a 100 second integration time, but updated every second.  This is an enum parameter.
	MCAUpdateRate refreshRate() const { return refreshRate_; }
	/// The amount of time collecting data.
	double integrationTime() const { return integrationTime_; }
	/// The peaking time used for the detector.
	double peakingTime() const { return peakingTime_; }

	/// Returns a constant reference of the current ROI List.
	const AMROIInfoList *roiList() const { return &roiList_; }
	/// Returns a modifiable reference to the current ROI List.
	AMROIInfoList *roiList() { return &roiList_; }

	// Database loading and storing
	///////////////////////

	/// The database reading member function.
	AMDbObject *dbGetROIList() { return &roiList_; }
	/// Don't need to do anything because dbGetROIList always returns a valid AMDbObject.
	void dbLoadROIList(AMDbObject *) {}

public slots:

	/// Set the number of channels in the spectral output.
	bool setSize(const AMnDIndex &size)
	{
		if (size.rank() != 1)
			return false;

		channels_ = size.i();
		setModified(true);
		return true;
	}
	/// Sets the maximum energy calibration for the detector.
	void setMaximumEnergy(double energy) { maxEnergy_ = energy; setModified(true); }
	/// Sets the number of elements in the detector.
	void setElements(int num) { elements_ = num; setModified(true); }
	/// Sets the number of active elements in the detector.
	bool setActiveElements(int num)
	{
		if (num > elements_)
			return false;

		activeElements_ = num;
		setModified(true);
		return true;
	}
	/// Sets the refresh rate for the spectra while data is being collected.
	void setRefreshRate(MCAUpdateRate rate) { refreshRate_ = rate; setModified(true); }
	/// Sets the integration time for the detector.
	void setIntegrationTime(double time) { integrationTime_ = time; setModified(true); }
	/// Sets the peaking time for the detector.
	void setPeakingTime(double time) { peakingTime_ = time; setModified(true); }
	/// Sets the ROI list for the detector.
	void setROIList(const AMROIInfoList &roiList) { roiList_.setValuesFrom(roiList); setModified(true); }

protected:
	/// Number of channels in the spectral output.
	int channels_;
	/// The maximum energy calibration of the detector.
	double maxEnergy_;
	/// Number of elements in the detector.
	int elements_;
	/// Number of active elements in the detector.  Useful because not all elements are working or giving good data.
	int activeElements_;
	/// The refresh rate for the spectra while data is being collected.  For instance, you might want to be seeing what the spectrum looks like in a 100 second integration time, but updated every second.  This is an enum parameter.
	MCAUpdateRate refreshRate_;
	/// The amount of time collecting data.
	double integrationTime_;
	/// The peaking time for the detector.  The rest time between photon events.
	double peakingTime_;
	/// The list holding all the current ROIs for the detector.
	AMROIInfoList roiList_;
};

#endif // XRFDETECTORINFO_H
