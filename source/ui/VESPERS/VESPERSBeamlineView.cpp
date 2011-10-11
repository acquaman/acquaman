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


#include "VESPERSBeamlineView.h"

#include "beamline/VESPERS/VESPERSBeamline.h"
#include "ui/VESPERS/VESPERSIonChamberCalibrationView.h"
#include "ui/VESPERS/VESPERSIonChamberView.h"
#include "ui/CLS/CLSSynchronizedDwellTimeView.h"
#include "ui/AMTopFrame.h"

#include "beamline/CLS/CLSIonChamber.h"
#include "ui/beamline/AMIonChamberView.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>

VESPERSBeamlineView::VESPERSBeamlineView(QWidget *parent) :
	QWidget(parent)
{
	AMTopFrame *top = new AMTopFrame("Beamline Components");
	top->setIcon(QIcon(":/system-software-update.png"));

	VESPERSIonChamberCalibrationView *ionCalibrationView = new VESPERSIonChamberCalibrationView(VESPERSBeamline::vespers()->ionChamberCalibration());
	CLSSynchronizedDwellTimeView *dwellTimeView = new CLSSynchronizedDwellTimeView(VESPERSBeamline::vespers()->synchronizedDwellTime());

	QGroupBox *ionChambers = new QGroupBox("Ion Chamber Calibration");
	QVBoxLayout *ionChamberLayout = new QVBoxLayout;
	ionChamberLayout->addWidget(new AMIonChamberView((AMIonChamber *)VESPERSBeamline::vespers()->iPreKB()));
	ionChamberLayout->addWidget(new AMIonChamberView((AMIonChamber *)VESPERSBeamline::vespers()->iMini()));
	ionChamberLayout->addWidget(new AMIonChamberView((AMIonChamber *)VESPERSBeamline::vespers()->iPost()));
	ionChambers->setLayout(ionChamberLayout);
	ionChambers->setMaximumSize(260, 200);

	QVBoxLayout *current = new QVBoxLayout;
	current->addWidget(ionCalibrationView, 0, Qt::AlignCenter);
	current->addWidget(ionChambers);

	QHBoxLayout *next = new QHBoxLayout;
	next->addStretch();
	next->addLayout(current);
	next->addWidget(dwellTimeView);
	next->addStretch();

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(top);
	mainLayout->addStretch();
	mainLayout->addLayout(next);
	mainLayout->addStretch();

	setLayout(mainLayout);
}
