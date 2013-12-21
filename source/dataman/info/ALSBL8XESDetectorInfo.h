/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef ALSBL8XESDETECTORINFO_H
#define ALSBL8XESDETECTORINFO_H

#include "dataman/info/AMOldDetectorInfo.h"

class ALSBL8XESDetectorInfo : public AMOldDetectorInfo
{
	Q_OBJECT

	// These properties could be different for each scan
	Q_PROPERTY(double centerEnergy READ centerEnergy WRITE setCenterEnergy);
	Q_PROPERTY(int gratingNumber READ gratingNumber WRITE setGratingNumber);
	Q_PROPERTY(double detectorAngle READ detectorAngle WRITE setDetectorAngle);
	Q_PROPERTY(double positionX READ positionX WRITE setPositionX);
	Q_PROPERTY(double positionY READ positionY WRITE setPositionY);
	Q_PROPERTY(double positionA READ positionA WRITE setPositionA);
	Q_PROPERTY(double slitWidth READ slitWidth WRITE setSlitWidth);
	Q_PROPERTY(int targetCounts READ targetCounts WRITE setTargetCounts);
	Q_PROPERTY(double targetExposureSeconds READ targetExposureSeconds WRITE setTargetExposureSeconds);

	// The following properties should be the same for all scans, unless the spectrometer is calibrated or modified
	Q_PROPERTY(int pixelsX READ pixelsX WRITE loadPixelsX);
	Q_PROPERTY(int pixelsY READ pixelsY WRITE loadPixelsY);
	Q_PROPERTY(int gratingCount READ gratingCount WRITE loadGratingCount);
	Q_PROPERTY(double anodeWidthMm READ anodeWidthMm WRITE loadAnodeWidthMm);	// also: dbc = anodeWidthX / pixelsX
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


	Q_CLASSINFO("AMDbObject_Attributes", "description=ALS BL8.0.1 XES Spectrometer Settings")

public:
 	virtual ~ALSBL8XESDetectorInfo();
	explicit ALSBL8XESDetectorInfo(QObject *parent = 0);

	// Dimensionality and size:
	////////////////////////////////////
	/// Returns the number of dimensions in the output of this detector. A single point has dimension 0. A spectrum output would have dimension 1. An image output would have dimension 2.
	virtual int rank() const { return 2;}
	/// Returns the size (ie: number of elements) along each dimension of the detector.  For a single-point detector, returns an empty AMnDIndex(). For a spectrum output, this would contain one number (the number of pixels or points along the axis).  For an image output, this would contain the width and height.
	virtual AMnDIndex size() const { return AMnDIndex(axes_.at(0).size, axes_.at(1).size); }
	/// Returns a list of AMAxisInfo describing the size and nature of each detector axis, in order.
	virtual QList<AMAxisInfo> axes() const {
		return axes_;
	}



	// Property access
	//////////////////////

	/// Energy value (in eV) of the center of the detector
	double centerEnergy() const { return centerEnergy_; }
	/// The grating to be used (for ALS spectrometer: 1, 2, 3, or 4)
	int gratingNumber() const { return gratingNumber_; }
	/// The index (0-based) of the grating to be used
	int gratingIndex() const { return gratingNumber_ - 1; }
	/// The detector angle, in degrees, up from tangential to the Rowland Circle
	double detectorAngle() const { return detectorAngle_; }
	/// The X-stage position of the detector
	double positionX() const { return positionX_; }
	/// The Y-stage position of the detector
	double positionY() const { return positionY_; }
	/// The Z-stage position of the detector
	double positionA() const { return positionA_; }

	/// The width of the slit, which determines the resolution
	double slitWidth() const { return slitWidth_; }


	/// \todo Should this be moved to the scan configuration? Yes.
	/// The target number of counts. The scan will stop after recording this many counts.
	int targetCounts() const { return targetCounts_; }
	/// The target exposure time, in seconds.  The scan will stop after reaching this exposure time.
	double targetExposureSeconds() const { return targetExposureSeconds_; }


	/// The number of pixels across the x-axis (energy-dispersive axis) of the detector
	int pixelsX() const { return axes_[0].size; }
	/// The number of pixels across the y-axis (height axis) of the detector
	int pixelsY() const { return axes_[1].size; }

	/// The number of gratings available.
	int gratingCount() const { return gratingCount_; }
	/// The width of the energy axis of the detector, in mm
	double anodeWidthMm() const { return anodeWidthMm_; }
	/// The spatial width of a single pixel, in mm.
	double dbc() const { return anodeWidthMm_ / pixelsX(); }

	/// A list of the names of each grating available
	QStringList gratingNames() const { return gratingNames_; }
	/// The slit position in the spectrometer coordinate system (mm)
	double slitPositionX() const { return slitPositionX_; }
	/// The slit position in the spectrometer coordinate system (mm)
	double slitPositionY() const { return slitPositionY_; }
	/// The groove densities (lines/mm) for each grating
	AMDoubleList gratingGrooveDensities() const { return gratingGrooveDensities_; }
	/// The radii (in mm) of each grating
	AMDoubleList gratingRadii() const { return gratingRadii_; }
	/// The position of each grating, in the spectrometer coordinate system (mm)
	AMDoubleList gratingPositionsX() const { return gratingPositionsX_; }
	/// The position of each grating, in the spectrometer coordinate system (mm)
	AMDoubleList gratingPositionsY() const { return gratingPositionsY_; }
	/// A measurement used in determining the eV axis calibration, for each grating (degrees)
	AMDoubleList zeroOrderRayAngles() const { return zeroOrderRayAngles_; }	// deg

	/// Correction terms used to calibrate the focus and energy axis, for each grating (mm)
	AMDoubleList gratingCorrectionsDX() const { return gratingCorrectionsDX_; }	// mm
	/// Correction terms used to calibrate the focus and energy axis, for each grating (mm)
	AMDoubleList gratingCorrectionsDY() const { return gratingCorrectionsDY_; }	// mm
	/// Correction terms used to calibrate the focus and energy axis, for each grating (mrad)
	AMDoubleList gratingCorrectionsDA() const { return gratingCorrectionsDA_; }	// mrad
	/// Correction terms used to calibrate the focus and energy axis, for each grating (lines/mm)
	AMDoubleList gratingCorrectionsGrooveDensity() const { return gratingCorrectionsGrooveDensity_; }	// lines/mm
	/// Correction terms used to calibrate the focus and energy axis, for each grating (mm)
	AMDoubleList gratingCorrectionsRadii() const { return gratingCorrectionsRadii_; }	// mm
	/// Correction terms used to calibrate the focus and energy axis, for each grating (mm)
	AMDoubleList slitCorrectionsDX() const { return slitCorrectionsDX_; }	// mm
	/// Correction terms used to calibrate the focus and energy axis, for each grating (mm)
	AMDoubleList slitCorrectionsDY() const { return slitCorrectionsDY_; }	// mm


	// Property setting, for database re-loading
	/////////////////////////////////

	// These properties could be different for each scan
	void setCenterEnergy(double newValue) { centerEnergy_ = newValue; setModified(true); }
	void setGratingNumber(int newValue) { gratingNumber_ = newValue; setModified(true); }
	void setDetectorAngle(double newValue) { detectorAngle_ = newValue; setModified(true); }
	void setPositionX(double newValue) { positionX_ = newValue; setModified(true); }
	void setPositionY(double newValue) { positionY_ = newValue; setModified(true); }
	void setPositionA(double newValue) { positionA_ = newValue; setModified(true); }
	void setSlitWidth(double newValue) { slitWidth_ = newValue; setModified(true); }
	void setTargetCounts(int newValue) { targetCounts_ = newValue; setModified(true); }
	void setTargetExposureSeconds(double newValue) { targetExposureSeconds_ = newValue; setModified(true); }

	// The following properties should be the same for all scans, unless the spectrometer is calibrated or modified
	void loadPixelsX(int newValue) { axes_[0].size = newValue; setModified(true); }
	void loadPixelsY(int newValue) { axes_[1].size = newValue; setModified(true); }
	void loadGratingCount(int newValue) { gratingCount_ = newValue; setModified(true); }
	void loadAnodeWidthMm(double newValue) { anodeWidthMm_ = newValue; setModified(true); }	// also: dbc = anodeWidthX / pixelsX
	void loadGratingNames(const QStringList& newValue) { gratingNames_ = newValue; setModified(true); }
	void loadSlitPositionX(double newValue) { slitPositionX_ = newValue; setModified(true); } // mm
	void loadSlitPositionY(double newValue) { slitPositionY_ = newValue; setModified(true); } // mm
	void loadGratingGrooveDensities(AMDoubleList newValue) { gratingGrooveDensities_ = newValue; setModified(true); }	// lines/mm
	void loadGratingRadii(const AMDoubleList& newValue) { gratingRadii_ = newValue; setModified(true); }	// mm
	void loadGratingPositionsX(const AMDoubleList& newValue) { gratingPositionsX_ = newValue; setModified(true); }	// mm
	void loadGratingPositionsY(const AMDoubleList& newValue) { gratingPositionsY_ = newValue; setModified(true); }	// mm
	void loadZeroOrderRayAngles(const AMDoubleList& newValue) { zeroOrderRayAngles_ = newValue; setModified(true); }	// deg

	void loadGratingCorrectionsDX(const AMDoubleList& newValue) { gratingCorrectionsDX_ = newValue; setModified(true); }	// mm
	void loadGratingCorrectionsDY(const AMDoubleList& newValue) { gratingCorrectionsDY_ = newValue; setModified(true); }	// mm
	void loadGratingCorrectionsDA(const AMDoubleList& newValue) { gratingCorrectionsDA_ = newValue; setModified(true); }	// mrad
	void loadGratingCorrectionsGrooveDensity(const AMDoubleList& newValue) { gratingCorrectionsGrooveDensity_ = newValue; setModified(true); }	// lines/mm
	void loadGratingCorrectionsRadii(const AMDoubleList& newValue) { gratingCorrectionsRadii_ = newValue; setModified(true); }	// mm
	void loadSlitCorrectionsDX(const AMDoubleList& newValue) { slitCorrectionsDX_ = newValue; setModified(true); }	// mm
	void loadSlitCorrectionsDY(const AMDoubleList& newValue) { slitCorrectionsDY_ = newValue; setModified(true); }	// mm




signals:

public slots:

protected:
	double centerEnergy_;
	int gratingNumber_;
	double detectorAngle_;
	double positionX_;
	double positionY_;
	double positionA_;
	double slitWidth_;
	int targetCounts_;
	double targetExposureSeconds_;

	int gratingCount_;
	double anodeWidthMm_;	// also: dbc = anodeWidthX / pixelsX
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

	mutable QList<AMAxisInfo> axes_;

};

#endif // ALSBL8XESDETECTORINFO_H
