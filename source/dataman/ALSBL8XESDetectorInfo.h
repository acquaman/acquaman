#ifndef ALSBL8XESDETECTORINFO_H
#define ALSBL8XESDETECTORINFO_H

#include "dataman/AMDetectorInfo.h"

class ALSBL8XESDetectorInfo : public AMDetectorInfo
{
	Q_OBJECT
	// this is sweet reg exp for changing rv << AMMetaMetaData(...) into Q_PROPERTY string: \,\ "[^"]+", [^\)]+\)
	/// \todo write property functions...
	/*
	Q_PROPERTY(double centerEnergy READ centerEnergy WRITE loadCenterEnergy);
	Q_PROPERTY(int grating READ grating WRITE loadGrating);
	Q_PROPERTY(double angle READ angle WRITE loadAngle);
	Q_PROPERTY(double posX READ posX WRITE loadPosX);
	Q_PROPERTY(double posY READ posY WRITE loadPosY);
	Q_PROPERTY(double posA READ posA WRITE loadPosA);
	Q_PROPERTY(double slitWidth READ slitWidth WRITE loadSlitWidth);
	Q_PROPERTY(int targetCounts READ targetCounts WRITE loadTargetCounts);
	Q_PROPERTY(double targetExposure READ targetExposure WRITE loadTargetExposure);

	Q_PROPERTY(int pixelsX READ pixelsX WRITE loadPixelsX);
	Q_PROPERTY(int pixelsY READ pixelsY WRITE loadPixelsY);
	Q_PROPERTY(int numGratings READ numGratings WRITE loadNumGratings);
	Q_PROPERTY(double anodeWidthX READ anodeWidthX WRITE loadAnodeWidthX);	// also: dbc = anodeWidthX / pixelsX
	Q_PROPERTY(QStringList gratingNames READ gratingNames WRITE loadGratingNames);
	Q_PROPERTY(double slitPositionX READ slitPositionX WRITE loadSlitPositionX); // mm
	Q_PROPERTY(double slitPositionY READ slitPositionY WRITE loadSlitPositionY); // mm
	Q_PROPERTY(AMDoubleList gratingGrooveDensities READ gratingGrooveDensities WRITE loadGratingGrooveDensities);	// lines/mm
	Q_PROPERTY(AMDoubleList gratingRadii READ gratingRadii WRITE loadGratingRadii);	// mm
	Q_PROPERTY(AMDoubleList gratingPositionsX READ gratingPositionsX WRITE loadGratingPositionsX);	// mm
	Q_PROPERTY(AMDoubleList gratingPositionsY READ gratingPositionsY WRITE loadGratingPositionsY);	// mm
	Q_PROPERTY(AMDoubleList zeroOrderRayAngles READ zeroOrderRayAngles WRITE loadZeroOrderRayAngles);	// deg
	Q_PROPERTY(AMDoubleList gratingCorrectionsDX READ gratingCorrectionsDX WRITE loadGratingCorrectionsDX);	// mm
	Q_PROPERTY(AMDoubleList gratingCorrectionsDY READ gratingCorrectionsDY WRITE loadGratingCorrectionsDY);	// mm
	Q_PROPERTY(AMDoubleList gratingCorrectionsDA READ gratingCorrectionsDA WRITE loadGratingCorrectionsDA);	// mrad
	Q_PROPERTY(AMDoubleList gratingCorrectionsGrooveDensity READ gratingCorrectionsGrooveDensity WRITE loadGratingCorrectionsGrooveDensity);	// lines/mm
	Q_PROPERTY(AMDoubleList gratingCorrectionsRadii READ gratingCorrectionsRadii WRITE loadGratingCorrectionsRadii);	// mm
	Q_PROPERTY(AMDoubleList slitCorrectionsDX READ slitCorrectionsDX WRITE loadSlitCorrectionsDX);	// mm
	Q_PROPERTY(AMDoubleList slitCorrectionsDY READ slitCorrectionsDY WRITE loadSlitCorrectionsDY);	// mm
	*/

	AM_DBOBJECTINFO("description=ALS BL8.0.1")

public:
	explicit ALSBL8XESDetectorInfo(QObject *parent = 0);

	// Basic meta-data:
	////////////////////////////////////
	/// Access the unique name of this detector. (Implemented in AMDbObject)
	// QString name() const;
	/// Access a human-readable description (ex: "Sample Current" or "Total Fluorescence Yield")
	QString description() const { return description_; }

	// Dimensionality and size:
	////////////////////////////////////
	/// Returns the number of dimensions in the output of this detector. A single point has dimension 0. A spectrum output would have dimension 1. An image output would have dimension 2.
	virtual int dimension() const { return 0;}
	/// Returns the rank (ie: number of elements) along each dimension of the detector.  For a single-point detector, an empty list is fine. For a spectrum output, this would contain one number (the number of pixels or points along the axis).  For an image output, this would contain the width and height.
	virtual QList<int> rank() const { return QList<int>(); }
	virtual bool hasDetails() const { return false; }



signals:

public slots:

protected:
	double centerEnergy_;
	int grating_;
	double angle_;
	double posX_;
	double posY_;
	double posA_;
	double slitWidth_;
	int targetCounts_;
	double targetExposure_;

	int pixelsX_;
	int pixelsY_;
	int numGratings_;
	double anodeWidthX_;	// also: dbc = anodeWidthX / pixelsX
	QStringList gratingNames_;
	double slitPositionX_; // mm
	double slitPositionY_; // mm
	AMDoubleList gratingGrooveDensities_;	// lines/mm
	AMDoubleList gratingRadii_;	// mm
	AMDoubleList gratingPositionsX_;	// mm
	AMDoubleList gratingPositionsY_;	// mm
	AMDoubleList zeroOrderRayAngles_;	// deg
	AMDoubleList gratingCorrectionsDX_;	// mm
	AMDoubleList gratingCorrectionsDY_;	// mm
	AMDoubleList gratingCorrectionsDA_;	// mrad
	AMDoubleList gratingCorrectionsGrooveDensity_;	// lines/mm
	AMDoubleList gratingCorrectionsRadii_;	// mm
	AMDoubleList slitCorrectionsDX_;	// mm
	AMDoubleList slitCorrectionsDY_;	// mm

};

#endif // ALSBL8XESDETECTORINFO_H
