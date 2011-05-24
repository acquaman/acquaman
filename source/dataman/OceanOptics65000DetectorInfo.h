#ifndef OCEANOPTICS65000DETECTORINFO_H
#define OCEANOPTICS65000DETECTORINFO_H

#include "AMDetectorInfo.h"

class OceanOptics65000DetectorInfo : public AMDetectorInfo
{
Q_OBJECT

Q_PROPERTY(int binCount READ binCount WRITE setBinCount)
Q_PROPERTY(QString axisName READ axisName WRITE setAxisName)
Q_PROPERTY(QString axisUnit READ axisUnit WRITE setAxisUnit)
Q_PROPERTY(double integrationTime READ integrationTime WRITE setIntegrationTime)

Q_CLASSINFO("AMDbObject_Attributes", "description=Ocean Optics 65000 USB Spectrometer Detector")

public:
	OceanOptics65000DetectorInfo(const QString &name, const QString &description, QObject *parent = 0);

	OceanOptics65000DetectorInfo(const OceanOptics65000DetectorInfo &original);

	/// Creates a new info pointer from this one, caller is responsible for memory
	virtual AMDetectorInfo* toNewInfo() const;

	OceanOptics65000DetectorInfo& operator=(const OceanOptics65000DetectorInfo& other);

	/// Number of bins in spectrometer
	int binCount() const;
	/// Name of the spectrum axis
	QString axisName() const;
	/// Units for the spectrum axis
	QString axisUnit() const;
	/// The integration time in seconds
	double integrationTime() const;

	/// Returns the number of dimensions in the output of this detector. A single point has dimension 0. A spectrum output would have dimension 1. An image output would have dimension 2.
	virtual int rank() const;
	/// Returns the rank (ie: number of elements) along each dimension of the detector.  Since this detector outputs a spectrum, this would contain one number (the number of pixels, points, or bins along the axis).
	virtual AMnDIndex size() const;
	/// Returns a list of AMAxisInfo describing the size and nature of each detector axis, in order.  Here we have a single axis.
	virtual QList<AMAxisInfo> axes() const;

	/// Debug helper
	QDebug qDebugPrint(QDebug &d) const;

	/// No clue, check to see if this is still needed
	virtual bool hasDetails() const;

public slots:
	/// Sets the desired integration time
	void setIntegrationTime(double integrationTime);

protected slots:
	/// Sets the number of bins (for internal use only)
	void setBinCount(int binCount);
	/// Sets the name of the spectrum axis (for internal use only)
	void setAxisName(const QString &axisName);
	/// Sets the unit of the spectrum axis (for interanl use only)
	void setAxisUnit(const QString &axisUnit);

protected:
	int binCount_;
	QString axisName_;
	QString axisUnit_;
	double integrationTime_;
};

#endif // OCEANOPTICS65000DETECTORINFO_H
