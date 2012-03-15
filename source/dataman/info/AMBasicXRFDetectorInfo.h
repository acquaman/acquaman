#ifndef AMBASICXRFDETECTORINFO_H
#define AMBASICXRFDETECTORINFO_H

#include "dataman/info/AMDetectorInfo.h"

/// This class contains the run-time configuration parameters for basic XRF detectors - those without regions of interest
class AMBasicXRFDetectorInfo : public AMDetectorInfo
{
Q_OBJECT

Q_PROPERTY(int elements READ elements WRITE setElements)
Q_PROPERTY(int channels READ channels WRITE setChannels)
Q_PROPERTY(double integrationTime READ integrationTime WRITE setIntegrationTime)
Q_PROPERTY(double peakingTime READ peakingTime WRITE setPeakingTime)

Q_CLASSINFO("AMDbObject_Attributes", "description=Basic XRF Detector")

public:
	/// Default Constructor
	Q_INVOKABLE AMBasicXRFDetectorInfo(const QString& name = "BasicXRF", const QString& description = "Basic XRF Detector", QObject *parent = 0);
	/// Copy Constructor
	AMBasicXRFDetectorInfo(const AMBasicXRFDetectorInfo &original);

	/// Implement assignment operator
	AMBasicXRFDetectorInfo& operator=(const AMBasicXRFDetectorInfo &other);

	/// Returns the number of dimensions in the output of this detector.  A single point has rank 0.  A spectrum output would have rank 1.  An image output would have rank 2.
	virtual int rank() const;
	/// Returns the size (ie: number of elements) along each dimension of the detector. For a spectrum output, this would contain one number (the number of pixels or points along the axis).  For an image output, this would contain the width and height.
	virtual AMnDIndex size() const;
	/// Returns a list of AMAxisInfo describing the size and nature of each detector axis, in order.
	virtual QList<AMAxisInfo> axes() const;

	// Properties
	///////////////////////////

	/// Number of channels in the spectral output.
	int channels() const;
	/// The number of elements in the detector.
	int elements() const;
	/// The amount of time collecting data.
	double integrationTime() const;
	/// The peaking time used for the detector.
	double peakingTime() const;

public slots:

	/// Set the AMnDIndex size in the spectral output.
	bool setSize(const AMnDIndex &size);
	/// Sets the number of channels in the spectral output.
	void setChannels(int num);
	/// Convenience call for above
	void setChannelsAsDouble(double num);

	/// Sets the number of elements in the detector.
	void setElements(int num);
	/// Sets the integration time for the detector.
	void setIntegrationTime(double time);
	/// Sets the peaking time for the detector.
	void setPeakingTime(double time);

protected:
	/// Number of channels in the spectral output.
	int channels_;
	/// Number of elements in the detector.
	int elements_;
	/// A list of the elements and whether or not they are active.
	QList<bool> activeElements_;
	/// The amount of time collecting data.
	double integrationTime_;
	/// The peaking time for the detector.  The rest time between photon events.
	double peakingTime_;
};

#endif // AMBASICXRFDETECTORINFO_H
