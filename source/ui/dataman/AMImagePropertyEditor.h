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


#ifndef AMIMAGEPROPERTYEDITOR_H
#define AMIMAGEPROPERTYEDITOR_H

#include <QFrame>
#include <QGridLayout>
#include <QRadioButton>
#include <QComboBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QSlider>
#include <QButtonGroup>
#include <QApplication>

#include "MPlot/MPlotColorMap.h"

/// This widget provides an editor for color map settings (standard color map, custom colors, brightness, contrast, gamma).  At the moment, it only supports one-way communication: changes to its editor widgets result in ___Changed() signals. However, there is no way to update its widgets programmatically -- it can only be user-driven.
/*! This widget can be used either with the MPlot library, where it can edit an MPlotColorMap directly via the colorMapChanged() signal.  It can also be used on its own, where the individual broken-out signals useStandardColorMapChanged(), standardColorMapChanged(), firstColorChanged(), secondColorChanged(), brightnessChanged(), contrastChanged(), and gammaChanged() are made available.
  */
class AMColorPickerButton;
class AMImagePropertyEditor : public QFrame
{
	Q_OBJECT

public:
	explicit AMImagePropertyEditor(QWidget *parent = 0);
	AMImagePropertyEditor(bool useStandardColorMap, int mplotStandardColorMap, const QColor& firstColor, const QColor& secondColor, double brightness, double contrast, double gamma, QWidget* parent = 0);
	AMImagePropertyEditor(const MPlotColorMap& initialMap, QWidget* parent = 0);

	bool useStandardColorMap() const;
	int standardColorMap() const;
	QColor firstColor() const;
	QColor secondColor() const;

	double brightness() const { return brightness_; }
	double contrast() const { return contrast_; }
	double gamma() const { return gamma_; }

	MPlotColorMap::BlendMode blendMode() const;

signals:
	void useStandardColorMapChanged(bool);
	void standardColorMapChanged(int mplotStandardColorMap);
	void firstColorChanged(const QColor& color);
	void secondColorChanged(const QColor& color);
	void blendModeChanged(int blendModeIsHSV);

	void brightnessChanged(double brightness);
	void contrastChanged(double contrast);
	void gammaChanged(double gamma);

	// this cumulative signal emits an updated color map anytime anything changes
	void colorMapChanged(const MPlotColorMap& newColorMap);

protected slots:
	void onBrightnessSliderChanged(int sliderValue);
	void onContrastSliderChanged(int sliderValue);
	void onGammaSliderChanged(int sliderValue);

	void onUseStandardColorMapChanged(bool useStandard);
	void onStandardColorMapChanged(int index);
	void onFirstColorChanged(const QColor& color);
	void onSecondColorChanged(const QColor& color);
	void onBlendModeChanged(bool blendModeIsHSV);

protected:
	QGridLayout *gridLayout;
	QRadioButton *useStandardColorMapButton;
	QComboBox *standardColorMapComboBox;
	QRadioButton *useCustomColorsButton;
	QHBoxLayout *horizontalLayout;
	AMColorPickerButton *firstColorButton;
	AMColorPickerButton *secondColorButton;
	QVBoxLayout *verticalLayout;
	QRadioButton *rgbButton;
	QRadioButton *hsvButton;
	QFrame *line;
	QLabel *brightnessText;
	QLabel *labelBrightness;
	QSlider *brightnessSlider;
	QLabel *labelContrast;
	QSlider *contrastSlider;
	QLabel *contrastText;
	QLabel *labelGamma;
	QSlider *gammaSlider;
	QLabel *gammaText;

	MPlotColorMap currentMap_;

	double brightness_, contrast_, gamma_;

	void setupUi();
	void makeConnections();
};

#endif // AMIMAGEPROPERTYEDITOR_H
