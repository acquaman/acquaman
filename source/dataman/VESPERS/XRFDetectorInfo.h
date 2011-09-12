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
	Q_PROPERTY(int channels READ channels WRITE setChannels)
	Q_PROPERTY(double integrationTime READ integrationTime WRITE setIntegrationTime)
	Q_PROPERTY(double peakingTime READ peakingTime WRITE setPeakingTime)
	Q_PROPERTY(AMDbObject* roiInfoList READ dbGetROIInfoList WRITE dbLoadROIInfoList)

	Q_CLASSINFO("AMDbObject_Attributes", "description=XRF Detector")

public:

	/// Default constructor.
	Q_INVOKABLE XRFDetectorInfo(const QString& name = "xrfSpectrum", const QString& description = "XRF Detector", QObject *parent = 0);
	/// Constructor that takes in a detectorInfo and retrieves all the settings.
	XRFDetectorInfo(const XRFDetectorInfo &original);

	/// Implement an = operator for this class.
	XRFDetectorInfo &operator =(const XRFDetectorInfo &other);

	// Dimensionality and size:
	///////////////////////////

	/// Returns the number of dimensions in the output of this detector.  A single point has rank 0.  A spectrum output would have rank 1.  An image output would have rank 2.
	virtual int rank() const { return 1; }
	/// Returns the size (ie: number of elements) along each dimension of the detector. For a spectrum output, this would contain one number (the number of pixels or points along the axis).  For an image output, this would contain the width and height.
	virtual AMnDIndex size() const { return AMnDIndex(channels_); }
	/// Returns a list of AMAxisInfo describing the size and nature of each detector axis, in order.
	virtual QList<AMAxisInfo> axes() const {
		QList<AMAxisInfo> axisInfo;
		AMAxisInfo ai("Energy", channels(), "Energy", "eV");
		ai.increment = AMNumber(scale());
		ai.start = AMNumber(0);
		ai.isUniform = true;
		axisInfo << ai;
		return axisInfo;
	}

	// Properties
	///////////////////////////

	/// Number of channels in the spectral output.
	int channels() const { return channels_; }
	/// The maximum energy calibration for the detector.  Primary indicator for energy divisions per channel number.
	double maximumEnergy() const { return maxEnergy_; }
	/// Returns the scale that would transform an energy and width into upper and lower bounds for a detector.
	double scale() const { return maximumEnergy()*1000/channels(); }
	/// The number of elements in the detector.
	int elements() const { return elements_; }
	/// The amount of time collecting data.
	double integrationTime() const { return integrationTime_; }
	/// The peaking time used for the detector.
	double peakingTime() const { return peakingTime_; }

	/// Returns a constant reference of the current ROI List.
	const AMROIInfoList *roiInfoList() const { return &roiInfoList_; }
	/// Returns a modifiable reference to the current ROI List.
	AMROIInfoList *roiInfoList() { return &roiInfoList_; }

	// Database loading and storing
	///////////////////////

	/// The database reading member function.
	AMDbObject *dbGetROIInfoList() { return &roiInfoList_; }
	/// Don't need to do anything because dbGetROIList always returns a valid AMDbObject.
	void dbLoadROIInfoList(AMDbObject *) {}

public slots:

	/// Set the AMnDIndex size in the spectral output.
	bool setSize(const AMnDIndex &size)
	{
		if (size.rank() != 1)
			return false;

		channels_ = size.i();
		setModified(true);
		return true;
	}
	/// Sets the number of channels in the spectral output.
	void setChannels(int num) { channels_ = num; setModified(true); }
	/// Sets the maximum energy calibration for the detector.
	void setMaximumEnergy(double energy);
	/// Sets the number of elements in the detector.
	void setElements(int num)
	{
		elements_ = num;

		activeElements_.clear();
		for (int i = 0; i < num; i++)
			activeElements_ << true;

		setModified(true);
	}
	/// Sets the integration time for the detector.
	void setIntegrationTime(double time) { integrationTime_ = time; setModified(true); }
	/// Sets the peaking time for the detector.
	void setPeakingTime(double time) { peakingTime_ = time; setModified(true); }
	/// Sets the ROI list for the detector.
	void setROIList(const AMROIInfoList &roiInfoList) { roiInfoList_.setValuesFrom(roiInfoList); setModified(true); }

protected:
	/// Number of channels in the spectral output.
	int channels_;
	/// The maximum energy calibration of the detector.
	double maxEnergy_;
	/// Number of elements in the detector.
	int elements_;
	/// A list of the elements and whether or not they are active.
	QList<bool> activeElements_;
	/// The amount of time collecting data.
	double integrationTime_;
	/// The peaking time for the detector.  The rest time between photon events.
	double peakingTime_;
	/// The list holding all the current ROIs for the detector.
	AMROIInfoList roiInfoList_;
};

#endif // XRFDETECTORINFO_H
