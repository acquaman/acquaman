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

#include "ui_AMLinePropertyEditor.h"

#include <QPen>
#include <QBrush>

AMLinePropertyEditor::AMLinePropertyEditor(QWidget *parent) :
	QFrame(parent), ui_(new Ui::AMLinePropertyEditor)
{
	ui_->setupUi(this);

	ui_->fillColorButton->setEnabled(false);
	connect(ui_->fillCheckBox, SIGNAL(clicked(bool)), ui_->fillColorButton, SLOT(setEnabled(bool)));

	connect(ui_->colorButton, SIGNAL(colorChanged(QColor)), this, SLOT(onPenSettingsChanged()));
	connect(ui_->widthSlider, SIGNAL(valueChanged(int)), this, SLOT(onPenSettingsChanged()));
	connect(ui_->lineStyleBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onPenSettingsChanged()));

	connect(ui_->fillCheckBox, SIGNAL(clicked(bool)), this, SIGNAL(areaFilledChanged(bool)));
	connect(ui_->fillColorButton, SIGNAL(colorChanged(QColor)), this, SLOT(onAreaFillColorChanged(QColor)));
}

AMLinePropertyEditor::AMLinePropertyEditor(const QPen& initialPen, bool areaFilled, const QBrush& areaFillColor, QWidget* parent) :
	QFrame(parent), ui_(new Ui::AMLinePropertyEditor)
{
	ui_->setupUi(this);

	ui_->colorButton->setColor(initialPen.color());
	ui_->widthSlider->setValue(initialPen.width());
	ui_->lineStyleBox->setCurrentIndex(qBound(0, initialPen.style()-1, 4));

	ui_->fillCheckBox->setChecked(areaFilled);
	ui_->fillColorButton->setColor(areaFillColor.color());

	ui_->fillColorButton->setEnabled(areaFilled);
	connect(ui_->fillCheckBox, SIGNAL(clicked(bool)), ui_->fillColorButton, SLOT(setEnabled(bool)));

	connect(ui_->colorButton, SIGNAL(colorChanged(QColor)), this, SLOT(onPenSettingsChanged()));
	connect(ui_->widthSlider, SIGNAL(valueChanged(int)), this, SLOT(onPenSettingsChanged()));
	connect(ui_->lineStyleBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onPenSettingsChanged()));

	connect(ui_->fillCheckBox, SIGNAL(clicked(bool)), this, SIGNAL(areaFilledChanged(bool)));
	connect(ui_->fillColorButton, SIGNAL(colorChanged(QColor)), this, SLOT(onAreaFillColorChanged(QColor)));
}


AMLinePropertyEditor::~AMLinePropertyEditor() {
	delete ui_;
}

void AMLinePropertyEditor::onPenSettingsChanged()
{
	emit linePenChanged(QPen(ui_->colorButton->color(),
							 ui_->widthSlider->value(),
							 (Qt::PenStyle)(ui_->lineStyleBox->currentIndex()+1)));
}

void AMLinePropertyEditor::onAreaFillColorChanged(const QColor &color)
{
	emit areaFillBrushChanged(QBrush(color));
}
