#ifndef AMDetectorInfo_H
#define AMDetectorInfo_H

#include "acquaman.h"
#include "dataman/AMDbObject.h"
#include "dataman/AMDatabaseDefinition.h"
#include <QStringList>
#include <QDebug>

/// This is the base class of all customizeable detectors and detector information objects. It gets associated with AMScans to indicate how their data was collected.  Subclasses should eventually contain the unique configuration and state of a particular physical detector (ex: the integration time and gain of a pico-ammeter, the HV setting for a CMA/channeltron, the bin configuration of a spectral detector like the Silicon Drift Detector.)  This base class is suitable for an un-configurable, single-point-recording generic detector.
class AMDetectorInfo : public AMDbObject
{
Q_OBJECT
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
	QString description() const { return metaData("description").toString(); }

	// Dimensionality and size:
	////////////////////////////////////
	/// Returns the number of dimensions in the output of this detector. A single point has dimension 0. A spectrum output would have dimension 1. An image output would have dimension 2.
	virtual int dimension() const { return 0;}
	/// Returns the rank (ie: number of elements) along each dimension of the detector.  For a single-point detector, an empty list is fine. For a spectrum output, this would contain one number (the number of pixels or points along the axis).  For an image output, this would contain the width and height.
	virtual QList<int> rank() const { return QList<int>(); }
	/// Convenience function to test if the detector has a dimesion > 0.
	virtual bool isSpectralOutput() const { return (dimension() > 0); }
	virtual bool hasDetails() const { return false; }


	// AMDbObject database interface
	////////////////////////////////////
	/// Specify all of our unique pieces of meta-data (excluding those inherited from base classes -- ie: own only)
	static QList<AMMetaMetaData> metaDataUniqueKeys() {
		QList<AMMetaMetaData> rv;
		rv << AMMetaMetaData(QVariant::String, "description", true);
		rv << AMMetaMetaData(QVariant::Int, "dimension", false);
		rv << AMMetaMetaData(AM::IntList, "rank", false);
		return rv;
	}


	/// This function needs to be overloaded to return all the available pieces of meta data for this type of object, including those inherited from base classes. (ie: own + base classes'). We simply append our unique meta-data onto the base class:
	static QList<AMMetaMetaData> metaDataKeys() {
		return AMDbObject::metaDataKeys() << metaDataUniqueKeys();
	}

	/// This virtual function returns all the available pieces of meta data for this type of object, by introspecting it's most detailed type. (ie: own + base classes' + subclasses')
	virtual QList<AMMetaMetaData> metaDataAllKeys() const {
		return this->metaDataKeys();
	}

	/// We want to store this in a separate table (so that it's easy to create relationships between detectors and scan objects).  Therefore, we reimplement databaseTableName():
	virtual QString databaseTableName() const {
		return AMDatabaseDefinition::detectorTableName();
	}

	/// Load yourself from the database. (returns true on success)
	/*! Re-implemented from AMDbObject. */
	virtual bool loadFromDb(AMDatabase* db, int id);

	/// Store or update self in the database. (returns true on success)
	/*! Re-implemented from AMDbObject::storeToDb(), this version also writes out the dimension and rank, even though they aren't strictly part of the meta-data.
	  */
	virtual bool storeToDb(AMDatabase* db);

	/// Reimplemented from AMDbObject; provides a general human-readable description
	virtual QString typeDescription() const {
		return "Generic Detector";
	}



	// Thumbnail System
	/////////////////////////////////
	/* Removed for now: otherwise this would save a separate thumbnail with each detectorInfo instance... huge waste of space. What we want is a single icon for all in this class. Waiting on improvements to the thumbnail system.

	/// Just to be fancy, we'll give ourselves a thumbnail icon. Maybe someone will use it someday.
	int thumbnailCount() const {
		return 1;
	}

	/// Just to be fancy, we'll give ourselves a thumbnail icon. Maybe someone will use it someday. You can re-implement this to make specific icons for specific detectors.
	AMDbThumbnail thumbnail(int index) const;
	*/

public slots:
	/// Convenience function to set the human-readable description
	void setDescription(const QString& description) {
		setMetaData("description", description);
	}

protected:

};

class AMSpectralOutputDetectorInfo : public AMDetectorInfo
{
Q_OBJECT
public:
	AMSpectralOutputDetectorInfo(const QString& name, const QString& description, int binCount = 0, QString axisName = "", QStringList binNames = QStringList(), QObject *parent = 0);

	/// Number of bins or pixels along the axis
	int binCount() const { return metaData("binCount").toInt(); }
	/// Name of the spectrum axis (ex: "eV")
	QString axisName() const { return metaData("axisName").toString(); }
	/// Name of each column or bin
	QStringList binNames() const { return metaData("binNames").toStringList(); }
	/// The sampling/integration time, in seconds
	double integrationTime() const { return metaData("integrationTime").toDouble(); }
	QPair<double, double> integrationTimeRange() const { QPair<double, double> rVal; rVal.first = metaData("integrationTimeRangeMin").toDouble(); rVal.second = metaData("integrationTimeRangeMax").toDouble(); return rVal;}
	/// The integration mode (describes the integration time: real, ____, or \todo )
	QString integrationMode() const { return metaData("integrationMode").toString(); }
	QStringList integrationModeList() const { return metaData("integrationModeList").toStringList(); }
	//QStringList integrationModes() const { return integrationModes_;}

	// Dimensionality and size:
	////////////////////////////////////
	/// Returns the number of dimensions in the output of this detector. A single point has dimension 0. A spectrum output would have dimension 1. An image output would have dimension 2.
	virtual int dimension() const { return 1;}
	/// Returns the rank (ie: number of elements) along each dimension of the detector.  Since this detector outputs a spectrum, this would contain one number (the number of pixels, points, or bins along the axis).
	virtual QList<int> rank() const { QList<int> rv; rv << binCount(); return rv; }


	// AMDbObject database interface
	////////////////////////////////////
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
	}


	/// This function needs to be overloaded to return all the available pieces of meta data for this type of object, including those inherited from base classes. (ie: own + base classes'). We simply append our unique meta-data onto the base class:
	static QList<AMMetaMetaData> metaDataKeys() {
//		return AMDbObject::metaDataKeys() << metaDataUniqueKeys();
		return AMDetectorInfo::metaDataKeys() << metaDataUniqueKeys();
	}

	/// This virtual function returns all the available pieces of meta data for this type of object, by introspecting it's most detailed type. (ie: own + base classes' + subclasses')
	virtual QList<AMMetaMetaData> metaDataAllKeys() const {
		return this->metaDataKeys();
	}

	/// Since everything is stored in the meta-data hash, we can use the default implementation of storeToDb() and loadFromDb().

	/// Reimplemented from AMDbObject; provides a general human-readable description
	virtual QString typeDescription() const {
		return "Generic Binning Spectrum-Output Detector";
	}


public slots:
	void setBinCount(int numBins) {
		setMetaData("binCount", numBins);
	}
	void setAxisName(const QString& axisName) {
		setMetaData("axisName", axisName);
	}
	void setBinNames(const QStringList& binNames) {
		setMetaData("binNames", binNames);
	}
	void setIntegrationTime(double integrationTime) {
		setMetaData("integrationTime", integrationTime);
	}
	void setIntegrationTimeRange(QPair<double, double> range){
		setMetaData("integrationTimeRangeMin", range.first);
		setMetaData("integrationTimeRangeMax", range.second);
	}
	void setIntegrationMode(const QString& integrationMode) {
		/// \todo check that integrationMode is one of valid choices.
		if(integrationModeList().contains(integrationMode)){
			setMetaData("integrationMode", integrationMode);
		}
	}

protected:
	//QStringList integrationModes_;
};

class MCPDetectorInfo : public AMDetectorInfo
{
Q_OBJECT
public:
	MCPDetectorInfo(const QString& name, const QString& description, QObject *parent = 0);

	/// Operational setpoint for High Voltage (HV)
	double hvSetpoint() const { return metaData("hvSetpoint").toDouble(); }
	QPair<double, double> hvSetpointRange() const {
		QPair<double, double> rVal;
		rVal.first = metaData("hvSetpointRangeMin").toDouble();
		rVal.second = metaData("hvSetpointRangeMax").toDouble();
		return rVal;
	}

	// Dimensionality and size:
	////////////////////////////////////
	/// Returns the number of dimensions in the output of this detector. A single point has dimension 0. A spectrum output would have dimension 1. An image output would have dimension 2.
	virtual int dimension() const { return 0;}
	/// Returns the rank (ie: number of elements) along each dimension of the detector.  Since this detector outputs a spectrum, this would contain one number (the number of pixels, points, or bins along the axis).
	virtual QList<int> rank() const { return QList<int>(); }
	virtual bool hasDetails() const { return true; }


	// AMDbObject database interface
	////////////////////////////////////
	/// Specify all of our unique pieces of meta-data (excluding those inherited from base classes -- ie: own only)
	static QList<AMMetaMetaData> metaDataUniqueKeys() {
		QList<AMMetaMetaData> rv;
		rv << AMMetaMetaData(QVariant::Double, "hvSetpoint", true);
		rv << AMMetaMetaData(QVariant::Double, "hvSetpointRangeMin", true);
		rv << AMMetaMetaData(QVariant::Double, "hvSetpointRangeMax", true);
		return rv;
	}


	/// This function needs to be overloaded to return all the available pieces of meta data for this type of object, including those inherited from base classes. (ie: own + base classes'). We simply append our unique meta-data onto the base class:
	static QList<AMMetaMetaData> metaDataKeys() {
//		return AMDbObject::metaDataKeys() << metaDataUniqueKeys();
		return AMDetectorInfo::metaDataKeys() << metaDataUniqueKeys();
	}

	/// This virtual function returns all the available pieces of meta data for this type of object, by introspecting it's most detailed type. (ie: own + base classes' + subclasses')
	virtual QList<AMMetaMetaData> metaDataAllKeys() const {
		return this->metaDataKeys();
	}

	/// Since everything is stored in the meta-data hash, we can use the default implementation of storeToDb() and loadFromDb().

	/// Reimplemented from AMDbObject; provides a general human-readable description
	virtual QString typeDescription() const {
		return "MCP Detector";
	}


public slots:
	void setHVSetpoint(double hvSetpoint) {
		setMetaData("hvSetpoint", hvSetpoint);
	}
	void setHVSetpointRange(QPair<double, double> range){
		setMetaData("hvSetpointRangeMin", range.first);
		setMetaData("hvSetpointRangeMax", range.second);
	}

protected:

};

class PGTDetectorInfo : public AMSpectralOutputDetectorInfo
{
Q_OBJECT
public:
	PGTDetectorInfo(const QString& name, const QString& description, QObject *parent = 0);

	/// Operational setpoint for High Voltage (HV)
	double hvSetpoint() const { return metaData("hvSetpoint").toDouble(); }
	QPair<double, double> hvSetpointRange() const {
		QPair<double, double> rVal;
		rVal.first = metaData("hvSetpointRangeMin").toDouble();
		rVal.second = metaData("hvSetpointRangeMax").toDouble();
		return rVal;
	}

	// Dimensionality and size:
	////////////////////////////////////
	/// Returns the number of dimensions in the output of this detector. A single point has dimension 0. A spectrum output would have dimension 1. An image output would have dimension 2.
	virtual int dimension() const { return 1;}
	/// Returns the rank (ie: number of elements) along each dimension of the detector.  Since this detector outputs a spectrum, this would contain one number (the number of pixels, points, or bins along the axis).
	virtual QList<int> rank() const { QList<int> rv; rv << binCount(); return rv; }
	virtual bool hasDetails() const { return true; }


	// AMDbObject database interface
	////////////////////////////////////
	/// Specify all of our unique pieces of meta-data (excluding those inherited from base classes -- ie: own only)
	static QList<AMMetaMetaData> metaDataUniqueKeys() {
		QList<AMMetaMetaData> rv;
		rv << AMMetaMetaData(QVariant::Double, "hvSetpoint", true);
		rv << AMMetaMetaData(QVariant::Double, "hvSetpointRangeMin", true);
		rv << AMMetaMetaData(QVariant::Double, "hvSetpointRangeMax", true);
		return rv;
	}


	/// This function needs to be overloaded to return all the available pieces of meta data for this type of object, including those inherited from base classes. (ie: own + base classes'). We simply append our unique meta-data onto the base class:
	static QList<AMMetaMetaData> metaDataKeys() {
//		return AMDbObject::metaDataKeys() << metaDataUniqueKeys();
		return AMSpectralOutputDetectorInfo::metaDataKeys() << metaDataUniqueKeys();
	}

	/// This virtual function returns all the available pieces of meta data for this type of object, by introspecting it's most detailed type. (ie: own + base classes' + subclasses')
	virtual QList<AMMetaMetaData> metaDataAllKeys() const {
		return this->metaDataKeys();
	}

	/// Since everything is stored in the meta-data hash, we can use the default implementation of storeToDb() and loadFromDb().

	/// Reimplemented from AMDbObject; provides a general human-readable description
	virtual QString typeDescription() const {
		return "PGT SDD Spectrum-Output Detector";
	}


public slots:
	void setHVSetpoint(double hvSetpoint) {
		setMetaData("hvSetpoint", hvSetpoint);
	}
	void setHVSetpointRange(QPair<double, double> range){
		setMetaData("hvSetpointRangeMin", range.first);
		setMetaData("hvSetpointRangeMax", range.second);
	}

protected:

};
#endif // AMDetectorInfo_H
