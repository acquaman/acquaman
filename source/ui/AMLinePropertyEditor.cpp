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


#include "AMLinePropertyEditor.h"
#include "ui/dataman/AMColorPickerButton.h"
#include "ui/AMPenStyleComboBox.h"

#include <QPen>
#include <QBrush>
#include <QApplication>

AMLinePropertyEditor::AMLinePropertyEditor(QWidget *parent) :
	QFrame(parent)
{
	setupUi();

	areaFillColorPickerButton_->setEnabled(false);
	connect(areaFillCheckBox_, SIGNAL(clicked(bool)), areaFillColorPickerButton_, SLOT(setEnabled(bool)));

	connect(lineColorPickerButton_, SIGNAL(colorChanged(QColor)), this, SLOT(onPenSettingsChanged()));
	connect(lineWidthSlider_, SIGNAL(valueChanged(int)), this, SLOT(onPenSettingsChanged()));	
	connect(lineStyleComboBox_, SIGNAL(currentPenStyleChanged(Qt::PenStyle)), this, SLOT(onPenSettingsChanged()));

	connect(areaFillCheckBox_, SIGNAL(clicked(bool)), this, SIGNAL(areaFilledChanged(bool)));
	connect(areaFillColorPickerButton_, SIGNAL(colorChanged(QColor)), this, SLOT(onAreaFillColorChanged(QColor)));

	connect(markerColorPickerButton_, SIGNAL(colorChanged(QColor)), this, SIGNAL(markerColorChanged(QColor)));
	connect(markerShapeComboBox_, SIGNAL(currentMarkerShapeChanged(MPlotMarkerShape::Shape)), this, SIGNAL(markerShapeChanged(MPlotMarkerShape::Shape)));
}

AMLinePropertyEditor::AMLinePropertyEditor(const QPen& initialPen, bool areaFilled, const QBrush& areaFillColor,
										   const QColor& markerColor, MPlotMarkerShape::Shape markerShape,
										   QWidget* parent) :
	QFrame(parent)
{
	setupUi();

	lineColorPickerButton_->setColor(initialPen.color());
	lineWidthSlider_->setValue(initialPen.width());
	lineStyleComboBox_->setSelectedPenStyle(initialPen.style());

	areaFillCheckBox_->setChecked(areaFilled);
	areaFillColorPickerButton_->setColor(areaFillColor.color());

	areaFillColorPickerButton_->setEnabled(areaFilled);
	markerColorPickerButton_->setColor(markerColor);
	markerShapeComboBox_->setSelectedMarkerShape(markerShape);

	connect(areaFillCheckBox_, SIGNAL(clicked(bool)), areaFillColorPickerButton_, SLOT(setEnabled(bool)));

	connect(lineColorPickerButton_, SIGNAL(colorChanged(QColor)), this, SLOT(onPenSettingsChanged()));
	connect(lineWidthSlider_, SIGNAL(valueChanged(int)), this, SLOT(onPenSettingsChanged()));
	connect(lineStyleComboBox_, SIGNAL(currentPenStyleChanged(Qt::PenStyle)), this, SLOT(onPenSettingsChanged()));

	connect(areaFillCheckBox_, SIGNAL(clicked(bool)), this, SIGNAL(areaFilledChanged(bool)));
	connect(areaFillColorPickerButton_, SIGNAL(colorChanged(QColor)), this, SLOT(onAreaFillColorChanged(QColor)));

	connect(markerColorPickerButton_, SIGNAL(colorChanged(QColor)), this, SIGNAL(markerColorChanged(QColor)));
	connect(markerShapeComboBox_, SIGNAL(currentMarkerShapeChanged(MPlotMarkerShape::Shape)), this, SIGNAL(markerShapeChanged(MPlotMarkerShape::Shape)));
}

void AMLinePropertyEditor::onPenSettingsChanged()
{
	emit linePenChanged(QPen(lineColorPickerButton_->color(),
							 lineWidthSlider_->value(),
							 lineStyleComboBox_->selectedPenStyle()));
}

void AMLinePropertyEditor::onAreaFillColorChanged(const QColor &color)
{
	emit areaFillBrushChanged(QBrush(color));
}

void AMLinePropertyEditor::setupUi()
{
	if (objectName().isEmpty())
		setObjectName(QString::fromUtf8("AMLinePropertyEditor"));
	resize(159, 193);
	setFrameShape(QFrame::NoFrame);
	setFrameShadow(QFrame::Plain);
	setStyleSheet(QString("AMLinePropertyEditor { background-color: black }"));

	QFormLayout* mainLayout = new QFormLayout();

	QLabel* lineSectionLabel = new QLabel();
	lineSectionLabel->setText("Line:");
	lineSectionLabel->setStyleSheet(
				QString("QLabel { color: white }"));

	// Line Color Picker
	lineColorPickerButton_ = new AMColorPickerButton();
	mainLayout->addRow(QString("Line Color:"), lineColorPickerButton_);
	mainLayout->labelForField(
				lineColorPickerButton_)->setStyleSheet(
					QString("QLabel { color: white }"));

	// Line Width Slider
	lineWidthSlider_ = new QSlider();
	lineWidthSlider_->setMinimum(1);
	lineWidthSlider_->setMaximum(10);
	lineWidthSlider_->setPageStep(2);
	lineWidthSlider_->setOrientation(Qt::Horizontal);
	lineWidthSlider_->setTickPosition(QSlider::TicksBelow);
	lineWidthSlider_->setTickInterval(2);
	mainLayout->addRow(QString("Line Width:"), lineWidthSlider_);
	mainLayout->labelForField(
				lineWidthSlider_)->setStyleSheet(
					QString("QLabel { color: white }"));

	// Line Style
	lineStyleComboBox_ = new AMPenStyleComboBox();
	mainLayout->addRow(QString("Line Style:"), lineStyleComboBox_);
	mainLayout->labelForField(
				lineStyleComboBox_)->setStyleSheet(
					QString("QLabel { color: white }"));

	// Marker Outline Color
	markerColorPickerButton_ = new AMColorPickerButton();
	mainLayout->addRow(QString("Marker Color:"), markerColorPickerButton_);
	mainLayout->labelForField(
				markerColorPickerButton_)->setStyleSheet(
					QString("QLabel { color: white }"));

	// Marker Style
	markerShapeComboBox_ = new AMPlotMarkerComboBox();
	mainLayout->addRow(QString("Marker Style:"), markerShapeComboBox_);
	mainLayout->labelForField(
				markerShapeComboBox_)->setStyleSheet(
					QString("QLabel { color: white }"));

	// Whether to fill the area of the plot (not yet supported in MPlot, therefore
	// the widget is disabled
	areaFillCheckBox_ = new QCheckBox();
	areaFillCheckBox_->setEnabled(false);
	mainLayout->addRow(QString("Area Fill:"), areaFillCheckBox_);
	mainLayout->labelForField(
				areaFillCheckBox_)->setStyleSheet(
					QString("QLabel { color: gray }"));

	// The color to fill the area of the plot with (not yet supported in MPlot,
	// therefore the widget is disabled
	areaFillColorPickerButton_ = new AMColorPickerButton();
	areaFillColorPickerButton_->setEnabled(false);
	mainLayout->addRow(QString("Area Fill Color:"), areaFillColorPickerButton_);
	mainLayout->labelForField(
				areaFillColorPickerButton_)->setStyleSheet(
					QString("QLabel { color: gray }"));

	setLayout(mainLayout);

	setWindowTitle(QApplication::translate("AMLinePropertyEditor", "Frame", 0, QApplication::UnicodeUTF8));

}
