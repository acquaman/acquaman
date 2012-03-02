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

#include "dataman/info/AMBasicXRFDetectorInfo.h"
#include "dataman/info/AMROIInfo.h"

/// This class contains the run-time configuration parameters for Silicon Drift detectors.
class XRFDetectorInfo : public AMBasicXRFDetectorInfo
{
	Q_OBJECT

	Q_PROPERTY(double maximumEnergy READ maximumEnergy WRITE setMaximumEnergy)
	Q_PROPERTY(AMDbObject* roiInfoList READ dbGetROIInfoList WRITE dbLoadROIInfoList)

	Q_CLASSINFO("AMDbObject_Attributes", "description=XRF Detector")

public:

	/// Default constructor.
	Q_INVOKABLE XRFDetectorInfo(const QString& name = "xrfSpectrum", const QString& description = "XRF Detector", QObject *parent = 0);
	/// Constructor that takes in a detectorInfo and retrieves all the settings.
	XRFDetectorInfo(const XRFDetectorInfo &original);

	/// Implement an = operator for this class.
	XRFDetectorInfo &operator =(const XRFDetectorInfo &other);

	// Properties
	///////////////////////////

	/// The maximum energy calibration for the detector.  Primary indicator for energy divisions per channel number.
	double maximumEnergy() const { return maxEnergy_; }
	/// Returns the scale that would transform an energy and width into upper and lower bounds for a detector.
	double scale() const { return maximumEnergy()*1000/channels(); }

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

	/// Sets the maximum energy calibration for the detector.
	void setMaximumEnergy(double energy);

	/// Sets the ROI list for the detector.
	void setROIList(const AMROIInfoList &roiInfoList) { roiInfoList_.setValuesFrom(roiInfoList); setModified(true); }

protected:
	/// The maximum energy calibration of the detector.
	double maxEnergy_;
	/// The list holding all the current ROIs for the detector.
	AMROIInfoList roiInfoList_;
};

#endif // XRFDETECTORINFO_H
