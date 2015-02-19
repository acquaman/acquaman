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


#ifndef AMLINESERIESPROPERTYEDITOR_H
#define AMLINESERIESPROPERTYEDITOR_H

#include <QFrame>
#include <QFormLayout>
#include <QLabel>

#include <QSlider>
#include <QComboBox>
#include <QSpacerItem>
#include <QCheckBox>

#include "ui/AMPlotMarkerComboBox.h"
/// This widget provides an editor for pen settings (color, width, line style, fill enabled, and fill color).  At the moment, it only supports one-way communication: changes to its editor widgets result in the signals linePenChanged(), areaFilledChanged(), and areaFillBrushChanged(). However, there is no way to update its widgets programmatically -- it can only be user-driven.
class AMColorPickerButton;
class AMPenStyleComboBox;
class AMLinePropertyEditor : public QFrame
{
	Q_OBJECT
public:
	/// Default Constructor
	explicit AMLinePropertyEditor(QWidget *parent = 0);
	/// Construct an editor with its widgets initialized according to these settings
	AMLinePropertyEditor(const QPen& initialPenSettings, bool areaFilled = false, const QBrush& areaFillColor = Qt::darkGray, const QColor &markerColor = Qt::red, MPlotMarkerShape::Shape markerShape = MPlotMarkerShape::None, QWidget* parent = 0);

signals:
	void linePenChanged(const QPen& newPenSettings);
	void areaFilledChanged(bool isFilled);
	void areaFillBrushChanged(const QBrush& newFillBrush);
	void markerColorChanged(const QColor& newColor);
	void markerShapeChanged(MPlotMarkerShape::Shape shape);
public slots:


protected slots:
	/// Used internally to respond to user actions (color, width, or line style)
	void onPenSettingsChanged();
	/// Used internally to respond to user actions (area fill color changed)
	void onAreaFillColorChanged(const QColor& color);


protected:

	// Line plot details
	AMColorPickerButton *lineColorPickerButton_;
	QSlider *lineWidthSlider_;
	AMPenStyleComboBox *lineStyleComboBox_;
	QCheckBox *areaFillCheckBox_;
	AMColorPickerButton *areaFillColorPickerButton_;

	// Marker plot details
	AMColorPickerButton *markerColorPickerButton_;
	AMPlotMarkerComboBox *markerShapeComboBox_;

private:
	void setupUi();

};

#endif // AMLINESERIESPROPERTYEDITOR_H
