/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#ifndef AM1DCalibrationABEDITOR_H
#define AM1DCalibrationABEDITOR_H

#include <QWidget>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QCheckBox>
#include <QSlider>
#include <QLabel>

#include "analysis/AM1DCalibrationAB.h"

class AMChooseScanDialog;


/*!
  This is a view that contains a drop box that holds all of the available data sources for normalization, energy offest ans caling values, and an adjustable origin point to scale about.  Any changes to these values are automatically propigated to all other AM1DCalibration analysis blocks applied to the sampel scan.  Includes option to apply this set of calibration values to other scans.

  \todo Because of the dynamic nature of setting various data sources, the AMDataSource API will need to have a general capability for choosing data sources like this (or in some other way).  That way some of these views can be generalized rather than making a specific one for every analysis block.
  */
class AM1DCalibrationABEditor : public QWidget
{
	Q_OBJECT
public:
	/// Constructor.  Takes in an AM1DCalibrationAB pointer.
	explicit AM1DCalibrationABEditor(AM1DCalibrationAB *analysisBlock, QWidget *parent = 0);
	/// Destructor.
	virtual ~AM1DCalibrationABEditor(){}

signals:

public slots:

protected slots:
	/// Handles setting the data name for the analysis block based on the new choice of input data source.
	void onDataNameChoiceChanged(int index);
	/// Handles setting the normalization name for the analysis block based on the new choice of input data source.
	void onNormalizationNameChoiceChanged(int index);
	/// Helper slot.  Populates the names combo box.
	void populateComboBox();

	void onEnergyCalibrationOffsetChanged(double offset);
	void onEnergyCalibrationScalingChanged(double scaling);
	void onEnergyCalibrationReferenceChanged(double reference);
	void onIsTransmissionChanged(bool isTransmassion);
	void onToEdgeJumpChanged(bool toEdgeJump);
	void onPreEdgePointChanged(int preEdgePoint);
	void onPostEdgePointChanged(int postEdgePoint);

	void onApplyToScansButtonClicked();
	void onApplyToOtherScansChosen();

	/// Call to propigate the three calibration values to all other AM1DCalibration ABs in the scan.
	void applyToAllSources();

protected:
	/// Pointer to the normalization analysis block.
	AM1DCalibrationAB *analysisBlock_;

	/// The combo box that contains all of the names.  Used for the data input source.
	QComboBox *dataNames_;
	/// The combo box that contains all of the names.  Used for the normalization data source.
	QComboBox *normalizationNames_;
	/// Spinners for Energy calibration
	QDoubleSpinBox *energyCalibrationOffsetBox_;
	QDoubleSpinBox *energyCalibrationScalingBox_;
	QDoubleSpinBox *energyCalibrationReferenceBox_;
	/// Apply to sources/scans button
	QPushButton *applyToAllSourcesButton_;
	QPushButton *applyToScansButton_;
	/// Dialog to ask the user for a set of scans (to apply the same shift curve to many at once)
	AMChooseScanDialog* chooseScanDialog_;
	QCheckBox* isTransmissionCheckbox_;
	QCheckBox* toEdgeJumpCheckbox_;
	QSlider* preEdgePointSlider_;
	QSlider* postEdgePointSlider_;
	QLabel* preEdgePointLabel_;
	QLabel* postEdgePointLabel_;
};

#endif // AM1DCalibrationABEDITOR_H
