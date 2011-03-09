#ifndef AMSPECTRALOUTPUTDETECTORINFO_H
#define AMSPECTRALOUTPUTDETECTORINFO_H

#include "AMDetectorInfo.h"

class AMSpectralOutputDetectorInfo : public AMDetectorInfo
{
Q_OBJECT
	Q_PROPERTY(int binCount READ binCount WRITE setBinCount)
	Q_PROPERTY(QString axisName READ axisName WRITE setAxisName)
	Q_PROPERTY(QStringList binNames READ binNames WRITE setBinNames)
	Q_PROPERTY(double integrationTime READ integrationTime WRITE setIntegrationTime)
	Q_PROPERTY(double integrationTimeRangeMin READ integrationTimeRangeMin WRITE setIntegrationTimeRangeMin)
	Q_PROPERTY(double integrationTimeRangeMax READ integrationTimeRangeMax WRITE setIntegrationTimeRangeMax)
	Q_PROPERTY(QString integrationMode READ integrationMode WRITE setIntegrationMode)
	Q_PROPERTY(QStringList integrationModeList READ integrationModeList)

	Q_CLASSINFO("AMDbObject_Attributes", "description=Generic Binning Spectrum-Output Detector")

public:
	AMSpectralOutputDetectorInfo(const QString& name, const QString& description, int binCount = 0, QString axisName = "", QStringList binNames = QStringList(), QObject *parent = 0);

	AMSpectralOutputDetectorInfo(const AMSpectralOutputDetectorInfo &original);

	/// Number of bins or pixels along the axis
	int binCount() const;
	/// Name of the spectrum axis (ex: "eV")
	QString axisName() const;
	/// Name of each column or bin
	QStringList binNames() const;
	/// The sampling/integration time, in seconds
	double integrationTime() const;
	QPair<double, double> integrationTimeRange() const;
	double integrationTimeRangeMin() const;
	double integrationTimeRangeMax() const;
	/// The integration mode (describes the integration time: real, ____, or \todo )
	QString integrationMode() const;
	QStringList integrationModeList() const;

	// Dimensionality and size:
	////////////////////////////////////
	/// Returns the number of dimensions in the output of this detector. A single point has dimension 0. A spectrum output would have dimension 1. An image output would have dimension 2.
	virtual int rank() const;
	/// Returns the rank (ie: number of elements) along each dimension of the detector.  Since this detector outputs a spectrum, this would contain one number (the number of pixels, points, or bins along the axis).
	virtual AMnDIndex size() const;
	/// Returns a list of AMAxisInfo describing the size and nature of each detector axis, in order.  Here we have a single axis.
	virtual QList<AMAxisInfo>  axes() const;


public slots:
	void setBinCount(int numBins);
	void setAxisName(const QString& axisName);
	void setBinNames(const QStringList& binNames);
	void setIntegrationTime(double integrationTime);
	void setIntegrationTimeRangeMin(double min);
	void setIntegrationTimeRangeMax(double max);
	void setIntegrationTimeRange(QPair<double, double> range);
	void setIntegrationMode(const QString& integrationMode);

protected:
	int binCount_;
	QString axisName_;
	QStringList binNames_;
	double integrationTime_, integrationTimeRangeMin_, integrationTimeRangeMax_;
	QString integrationMode_;
	QStringList integrationModeList_;
};


#endif // AMSPECTRALOUTPUTDETECTORINFO_H
