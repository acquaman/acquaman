/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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


#ifndef AMDetectorInfo_H
#define AMDetectorInfo_H

#include "acquaman.h"
#include "dataman/AMDbObject.h"
#include "dataman/AMMeasurementInfo.h"
#include <QStringList>
#include <QDebug>

/// This is the base class of all customizeable detectors and detector information objects. It gets associated with AMScans to indicate how their data was collected.  Subclasses should eventually contain the unique configuration and state of a particular physical detector (ex: the integration time and gain of a pico-ammeter, the HV setting for a CMA/channeltron, the bin configuration of a spectral detector like the Silicon Drift Detector.)  This base class is suitable for an un-configurable, single-point-recording generic detector.
class AMDetectorInfo : public AMDbObject
{
Q_OBJECT

	/// A description (human-readable string) for this type of detector
	Q_PROPERTY(QString description READ description WRITE setDescription)
	Q_PROPERTY(QString units READ units WRITE setUnits)
	Q_PROPERTY(int rank READ rank)
	Q_PROPERTY(AMnDIndex size READ size WRITE setSize)

	Q_CLASSINFO("AMDbObject_Attributes", "description=Generic Detector")

public:
	// Constructors and destructors
	/////////////////////////////////////
	/// Default constructor. \c name is a unique programmer's name to access this detector with. \c description is a human-readable version.
	Q_INVOKABLE AMDetectorInfo(const QString& name = "unknownDetector", const QString& description = "Generic Detector", QObject *parent = 0);

	~AMDetectorInfo() {}

	// Basic meta-data:
	////////////////////////////////////
	/// Access the unique name of this detector. (Implemented in AMDbObject)
	// QString name() const;
	/// Access a human-readable description (ex: "Sample Current" or "Total Fluorescence Yield")
	QString description() const { return description_; }
	/// The units describing this detector's readings. (ex: "counts", "milliAmps", etc.)
	QString units() const { return units_; }

	// Dimensionality and size:
	////////////////////////////////////
	/// Returns the number of dimensions in the output of this detector. A single point has rank 0. A spectrum output would have rank 1. An image output would have rank 2.
	virtual int rank() const { return 0;}
	/// Returns the size (ie: number of elements) along each dimension of the detector.  For a single-point detector, returns an empty AMnDIndex(). For a spectrum output, this would contain one number (the number of pixels or points along the axis).  For an image output, this would contain the width and height.
	virtual AMnDIndex size() const { return AMnDIndex(); }
	/// Returns a list of AMAxisInfo describing the size and nature of each detector axis, in order.
	virtual QList<AMAxisInfo>  axes() const { return QList<AMAxisInfo>(); }

	/// Returns (or casts) this AMDetectorInfo as an AMMeasurementInfo, which contains the bare-bones dimensional information.
	operator AMMeasurementInfo() {
		return AMMeasurementInfo(name(), description(), units(), axes());
	}


	/// I don't know what this is for.
	virtual bool hasDetails() const { return false; }



public slots:
	/// Set the human-readable description
	void setDescription(const QString& description) {
		description_ =  description;
		setModified(true);
	}
	/// Set the units describing the detector's readings
	void setUnits(const QString& units) {
		units_ = units;
		setModified(true);
	}

	/// Set the size of the detector. This may not be applicable or valid for some detectors (for example, a single-point detector, or one with a fixed dimensionality.  Returns false if it's invalid to set the size.
	virtual bool setSize(const AMnDIndex& size) { Q_UNUSED(size); return false; }

protected:
	QString description_;
	QString units_;
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
	virtual int rank() const { return 1;}
	/// Returns the rank (ie: number of elements) along each dimension of the detector.  Since this detector outputs a spectrum, this would contain one number (the number of pixels, points, or bins along the axis).
	virtual AMnDIndex size() const { return AMnDIndex(binCount()); }
	/// Returns a list of AMAxisInfo describing the size and nature of each detector axis, in order.  Here we have a single axis.
	virtual QList<AMAxisInfo>  axes() const {
		QList<AMAxisInfo> rv;
		AMAxisInfo binAxis(name()+"_x", binCount(), description() + " axis" );	/// \todo better name description, and provide units
		rv << binAxis;
		return rv;
	}


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

	// Since this is a single-point detector, we're using the default rank(), size(), and axes() from AMDetectorInfo.




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

	// Using the base class (AMSpectralOutputDetector) for default rank(), size(), and axes().


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
