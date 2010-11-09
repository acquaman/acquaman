#ifndef AMDetectorInfo_H
#define AMDetectorInfo_H

#include "acquaman.h"
#include "dataman/AMDbObject.h"
#include <QStringList>
#include <QDebug>

/// This is the base class of all customizeable detectors and detector information objects. It gets associated with AMScans to indicate how their data was collected.  Subclasses should eventually contain the unique configuration and state of a particular physical detector (ex: the integration time and gain of a pico-ammeter, the HV setting for a CMA/channeltron, the bin configuration of a spectral detector like the Silicon Drift Detector.)  This base class is suitable for an un-configurable, single-point-recording generic detector.
class AMDetectorInfo : public AMDbObject
{
Q_OBJECT

	/// A description (human-readable string) for this type of detector
	Q_PROPERTY(QString description READ description WRITE setDescription)
	Q_PROPERTY(int dimension READ dimension)
	Q_PROPERTY(QList<int> rank READ rank)	/// \bug upgrade this to AMnDIndex

	Q_CLASSINFO("AMDbObject_Attributes", "description=Generic Detector")

public:
	// Constructors and destructors
	/////////////////////////////////////
	/// Default constructor. \c name is a unique programmer's name to access this detector with. \c description is a human-readable version.
	AMDetectorInfo(const QString& name, const QString& description, QObject *parent = 0);

	~AMDetectorInfo() {}

	// Basic meta-data:
	////////////////////////////////////
	/// Access the unique name of this detector. (Implemented in AMDbObject)
	// QString name() const;
	/// Access a human-readable description (ex: "Sample Current" or "Total Fluorescence Yield")
	QString description() const { return description_; }

	// Dimensionality and size:
	////////////////////////////////////
	/// Returns the number of dimensions in the output of this detector. A single point has dimension 0. A spectrum output would have dimension 1. An image output would have dimension 2. \todo fix: backwards with rank().
	virtual int dimension() const { return 0;}
	/// Returns the rank (ie: number of elements) along each dimension of the detector.  For a single-point detector, an empty list is fine. For a spectrum output, this would contain one number (the number of pixels or points along the axis).  For an image output, this would contain the width and height. \todo fix: should be size(), and should return AMnDIndex.
	virtual QList<int> rank() const { return QList<int>(); }
	/// Convenience function to test if the detector has a dimesion > 0.
	virtual bool isSpectralOutput() const { return (dimension() > 0); }
	virtual bool hasDetails() const { return false; }



public slots:
	/// Convenience function to set the human-readable description
	void setDescription(const QString& description) {
		description_ =  description;
		setModified(true);
	}

protected:
	QString description_;
};

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

	/// Number of bins or pixels along the axis
	int binCount() const { return binCount_; }
	/// Name of the spectrum axis (ex: "eV")
	QString axisName() const { return axisName_; }
	/// Name of each column or bin
	QStringList binNames() const { return binNames_; }
	/// The sampling/integration time, in seconds
	double integrationTime() const { return integrationTime_; }
	QPair<double, double> integrationTimeRange() const { QPair<double, double> rVal; rVal.first = integrationTimeRangeMin_; rVal.second =integrationTimeRangeMax_; return rVal;}
	double integrationTimeRangeMin() const { return integrationTimeRangeMin_; }
	double integrationTimeRangeMax() const { return integrationTimeRangeMax_; }
	/// The integration mode (describes the integration time: real, ____, or \todo )
	QString integrationMode() const { return integrationMode_; }
	QStringList integrationModeList() const { return integrationModeList_; }

	// Dimensionality and size:
	////////////////////////////////////
	/// Returns the number of dimensions in the output of this detector. A single point has dimension 0. A spectrum output would have dimension 1. An image output would have dimension 2.
	virtual int dimension() const { return 1;}
	/// Returns the rank (ie: number of elements) along each dimension of the detector.  Since this detector outputs a spectrum, this would contain one number (the number of pixels, points, or bins along the axis).
	virtual QList<int> rank() const { QList<int> rv; rv << binCount(); return rv; }


	// AMDbObject database interface
	////////////////////////////////////
	/*
	/// Specify all of our unique pieces of meta-data (excluding those inherited from base classes -- ie: own only)
	static QList<AMMetaMetaData> metaDataUniqueKeys() {

		QList<AMMetaMetaData> rv;
		rv << AMMetaMetaData(QVariant::Int, "binCount", true);
		rv << AMMetaMetaData(QVariant::String, "axisName", true);
		rv << AMMetaMetaData(QVariant::StringList, "binNames", true);
		rv << AMMetaMetaData(QVariant::Double, "integrationTime", true);
		rv << AMMetaMetaData(QVariant::Double, "integrationTimeRangeMin", true);
		rv << AMMetaMetaData(QVariant::Double, "integrationTimeRangeMax", true);
		rv << AMMetaMetaData(QVariant::String, "integrationMode", true);
		rv << AMMetaMetaData(QVariant::StringList, "integrationModeList", true);
		return rv;
	}*/



public slots:
	void setBinCount(int numBins) {
		binCount_ = numBins;
		setModified(true);
	}
	void setAxisName(const QString& axisName) {
		axisName_ = axisName;
		setModified(true);
	}
	void setBinNames(const QStringList& binNames) {
		binNames_ = binNames;
		setModified(true);
	}
	void setIntegrationTime(double integrationTime) {
		integrationTime_ = integrationTime;
		setModified(true);
	}
	void setIntegrationTimeRangeMin(double min) {
		integrationTimeRangeMin_ = min;
		setModified(true);
	}
	void setIntegrationTimeRangeMax(double max) {
		integrationTimeRangeMax_ = max;
		setModified(true);
	}

	void setIntegrationTimeRange(QPair<double, double> range){
		integrationTimeRangeMin_ = range.first;
		integrationTimeRangeMax_ = range.second;
		setModified(true);
	}
	void setIntegrationMode(const QString& integrationMode) {
		/// \todo check that integrationMode is one of valid choices.
		integrationMode_ = integrationMode;
		setModified(true);
	}

protected:
	int binCount_;
	QString axisName_;
	QStringList binNames_;
	double integrationTime_, integrationTimeRangeMin_, integrationTimeRangeMax_;
	QString integrationMode_;
	QStringList integrationModeList_;
};

class MCPDetectorInfo : public AMDetectorInfo
{
Q_OBJECT
	Q_PROPERTY(double hvSetpoint READ hvSetpoint WRITE setHVSetpoint)
	Q_PROPERTY(double hvSetpointRangeMin READ hvSetpointRangeMin WRITE setHVSetpointRangeMin)
	Q_PROPERTY(double hvSetpointRangeMax READ hvSetpointRangeMax WRITE setHVSetpointRangeMax)

	Q_CLASSINFO("AMDbObject_Attributes", "description=MCP Detector")

public:
	MCPDetectorInfo(const QString& name, const QString& description, QObject *parent = 0);

	/// Operational setpoint for High Voltage (HV)
	double hvSetpoint() const { return hvSetpoint_; }
	double hvSetpointRangeMin() const { return hvSetpointRangeMin_; }
	double hvSetpointRangeMax() const { return hvSetpointRangeMax_; }
	QPair<double, double> hvSetpointRange() const {
		return QPair<double, double>(hvSetpointRangeMin_, hvSetpointRangeMax_);
	}


	// Dimensionality and size:
	////////////////////////////////////
	/// Returns the number of dimensions in the output of this detector. A single point has dimension 0. A spectrum output would have dimension 1. An image output would have dimension 2.
	virtual int dimension() const { return 0;}
	/// Returns the rank (ie: number of elements) along each dimension of the detector.  Since this detector outputs a spectrum, this would contain one number (the number of pixels, points, or bins along the axis).
	virtual QList<int> rank() const { return QList<int>(); }
	virtual bool hasDetails() const { return true; }



public slots:
	void setHVSetpoint(double hvSetpoint) {
		hvSetpoint_ = hvSetpoint;
		setModified(true);
	}
	void setHVSetpointRangeMin(double min) {
		hvSetpointRangeMin_ = min;
		setModified(true);
	}
	void setHVSetpointRangeMax(double max) {
		hvSetpointRangeMax_ = max;
		setModified(true);
	}
	void setHVSetpointRange(QPair<double, double> range){
		hvSetpointRangeMin_ = range.first;
		hvSetpointRangeMax_ = range.second;
		setModified(true);
	}

protected:
	double hvSetpointRangeMin_, hvSetpointRangeMax_, hvSetpoint_;
};

class PGTDetectorInfo : public AMSpectralOutputDetectorInfo
{
Q_OBJECT
	Q_PROPERTY(double hvSetpoint READ hvSetpoint WRITE setHVSetpoint)
	Q_PROPERTY(double hvSetpointRangeMin READ hvSetpointRangeMin WRITE setHVSetpointRangeMin)
	Q_PROPERTY(double hvSetpointRangeMax READ hvSetpointRangeMax WRITE setHVSetpointRangeMax)

	Q_CLASSINFO("AMDbObject_Attributes", "description=PGT Detector")

public:
	PGTDetectorInfo(const QString& name, const QString& description, QObject *parent = 0);

	/// Operational setpoint for High Voltage (HV)
	double hvSetpoint() const { return hvSetpoint_; }
	double hvSetpointRangeMin() const { return hvSetpointRangeMin_; }
	double hvSetpointRangeMax() const { return hvSetpointRangeMax_; }
	QPair<double, double> hvSetpointRange() const {
		return QPair<double, double>(hvSetpointRangeMin_, hvSetpointRangeMax_);
	}

	// Dimensionality and size:
	////////////////////////////////////
	/// Returns the number of dimensions in the output of this detector. A single point has dimension 0. A spectrum output would have dimension 1. An image output would have dimension 2.
	virtual int dimension() const { return 1;}
	/// Returns the rank (ie: number of elements) along each dimension of the detector.  Since this detector outputs a spectrum, this would contain one number (the number of pixels, points, or bins along the axis).
	virtual QList<int> rank() const { QList<int> rv; rv << binCount(); return rv; }
	virtual bool hasDetails() const { return true; }


public slots:
	void setHVSetpoint(double hvSetpoint) {
		hvSetpoint_ = hvSetpoint;
		setModified(true);
	}
	void setHVSetpointRangeMin(double min) {
		hvSetpointRangeMin_ = min;
		setModified(true);
	}
	void setHVSetpointRangeMax(double max) {
		hvSetpointRangeMax_ = max;
		setModified(true);
	}
	void setHVSetpointRange(QPair<double, double> range){
		hvSetpointRangeMin_ = range.first;
		hvSetpointRangeMax_ = range.second;
		setModified(true);
	}

protected:
	double hvSetpointRangeMin_, hvSetpointRangeMax_, hvSetpoint_;

};
#endif // AMDetectorInfo_H
