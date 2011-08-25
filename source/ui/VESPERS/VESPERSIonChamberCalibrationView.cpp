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


#include "VESPERSIonChamberCalibrationView.h"

#include "ui/VESPERS/VESPERSIonChamberView.h"

#include <QGridLayout>
#include <QLabel>

VESPERSIonChamberCalibrationView::VESPERSIonChamberCalibrationView(VESPERSIonChamberCalibration *calibration, QWidget *parent)
	: QWidget(parent)
{
	calibration_ = calibration;

	QGridLayout *calibrationLayout = new QGridLayout;
	calibrationLayout->addWidget(new QLabel("Name"), 0, 0, Qt::AlignCenter);
	calibrationLayout->addWidget(new QLabel("Sensitivity Value and Units"), 0, 1, 1, 2, Qt::AlignCenter);
	calibrationLayout->addWidget(new QLabel("Voltage (V)"), 0, 3, Qt::AlignCenter);

	calibrationLayout->addWidget(new VESPERSSplitIonChamberView(calibration_->splitIonChamber()), 1, 0, 1, 4);

	VESPERSIonChamberView *temp;

	for (int i = 0; i < calibration_->ionChamberCount(); i++){

		temp = new VESPERSIonChamberView(calibration_->ionChamberAt(i));
		calibrationLayout->addWidget(temp, i+2, 0, 1, 4);
	}

	setLayout(calibrationLayout);
	setMaximumSize(400, 300);
}
