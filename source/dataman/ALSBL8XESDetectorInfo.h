#ifndef ALSBL8XESDETECTORINFO_H
#define ALSBL8XESDETECTORINFO_H

#include "dataman/AMDetectorInfo.h"

class ALSBL8XESDetectorInfo : public AMDetectorInfo
{
	Q_OBJECT
public:
	explicit ALSBL8XESDetectorInfo(QObject *parent = 0);

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
		rv << AMMetaMetaData(QVariant::Double, "centerEnergy", true);
		rv << AMMetaMetaData(QVariant::Int, "grating", true);
		rv << AMMetaMetaData(QVariant::Double, "angle", true);
		rv << AMMetaMetaData(QVariant::Double, "posX", true);
		rv << AMMetaMetaData(QVariant::Double, "posY", true);
		rv << AMMetaMetaData(QVariant::Double, "posA", true);
		rv << AMMetaMetaData(QVariant::Double, "slitWidth", true);
		rv << AMMetaMetaData(QVariant::Int, "targetCounts", true);
		rv << AMMetaMetaData(QVariant::Double, "targetExposure", true);

		rv << AMMetaMetaData(QVariant::Int, "pixelsX", false);
		rv << AMMetaMetaData(QVariant::Int, "pixelsY", false);
		rv << AMMetaMetaData(QVariant::Int, "numGratings", false);
		rv << AMMetaMetaData(QVariant::Double, "anodeWidthX", false);	// also: dbc = anodeWidthX / pixelsX
		rv << AMMetaMetaData(QVariant::StringList, "gratingNames", false);
		rv << AMMetaMetaData(QVariant::Double, "slitPositionX", false); // mm
		rv << AMMetaMetaData(QVariant::Double, "slitPositionY", false); // mm
		rv << AMMetaMetaData(AM::DoubleList, "gratingGrooveDensities", false);	// lines/mm
		rv << AMMetaMetaData(AM::DoubleList, "gratingRadii", false);	// mm
		rv << AMMetaMetaData(AM::DoubleList, "gratingPositionsX", false);	// mm
		rv << AMMetaMetaData(AM::DoubleList, "gratingPositionsY", false);	// mm
		rv << AMMetaMetaData(AM::DoubleList, "zeroOrderRayAngles", false);	// deg
		rv << AMMetaMetaData(AM::DoubleList, "gratingCorrectionsDX", false);	// mm
		rv << AMMetaMetaData(AM::DoubleList, "gratingCorrectionsDY", false);	// mm
		rv << AMMetaMetaData(AM::DoubleList, "gratingCorrectionsDA", false);	// mrad
		rv << AMMetaMetaData(AM::DoubleList, "gratingCorrectionsGrooveDensity", false);	// lines/mm
		rv << AMMetaMetaData(AM::DoubleList, "gratingCorrectionsRadii", false);	// mm
		rv << AMMetaMetaData(AM::DoubleList, "slitCorrectionsDX", false);	// mm
		rv << AMMetaMetaData(AM::DoubleList, "slitCorrectionsDY", false);	// mm

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

	/// Reimplemented from AMDbObject; provides a general human-readable description
	virtual QString typeDescription() const {
		return "ALS BL8.0.1 ";
	}

signals:

public slots:

};

#endif // ALSBL8XESDETECTORINFO_H
