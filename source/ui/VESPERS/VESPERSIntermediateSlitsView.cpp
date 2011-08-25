/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "VESPERSIntermediateSlitsView.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QVBoxLayout>

VESPERSIntermediateSlitsView::VESPERSIntermediateSlitsView(VESPERSIntermediateSlits *slits, QWidget *parent)
	: QWidget(parent)
{
	slits_ = slits;

	xSlit_ = new QDoubleSpinBox;
	xSlit_->setSuffix(" mm");
	xSlit_->setDecimals(3);
	xSlit_->setSingleStep(0.001);
	connect(slits_, SIGNAL(gapXChanged(double)), xSlit_, SLOT(setValue(double)));
	connect(xSlit_, SIGNAL(editingFinished()), this, SLOT(setXGap()));

	zSlit_ = new QDoubleSpinBox;
	zSlit_->setSuffix(" mm");
	zSlit_->setDecimals(3);
	zSlit_->setSingleStep(0.001);
	connect(slits_, SIGNAL(gapZChanged(double)), zSlit_, SLOT(setValue(double)));
	connect(zSlit_, SIGNAL(editingFinished()), this, SLOT(setZGap()));

	QFormLayout *layout = new QFormLayout;
	layout->addRow("X-slit gap:", xSlit_);
	layout->addRow("Z-slit gap", zSlit_);

	QGroupBox *slitGroupBox = new QGroupBox("Intermediate Slits");
	slitGroupBox->setLayout(layout);

	QVBoxLayout *slitLayout = new QVBoxLayout;
	slitLayout->addWidget(slitGroupBox);

	setMaximumSize(200, 120);
	setLayout(slitLayout);
}
