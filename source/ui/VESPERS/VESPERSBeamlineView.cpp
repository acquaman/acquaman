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
#include "ui/VESPERS/VESPERSIntermediateSlitsView.h"
#include "ui/VESPERS/VESPERSIonChamberCalibrationView.h"
#include "ui/CLS/CLSSynchronizedDwellTimeView.h"
#include "ui/VESPERS/VESPERSBeamSelectorView.h"
#include "ui/AMTopFrame.h"

#include <QVBoxLayout>
#include <QHBoxLayout>

VESPERSBeamlineView::VESPERSBeamlineView(QWidget *parent) :
	QWidget(parent)
{
	AMTopFrame *top = new AMTopFrame("Beamline Components");
	top->setIcon(QIcon(":/system-software-update.png"));

	VESPERSIntermediateSlitsView *slitsView = new VESPERSIntermediateSlitsView(VESPERSBeamline::vespers()->intermediateSlits());
	VESPERSIonChamberCalibrationView *ionCalibrationView = new VESPERSIonChamberCalibrationView(VESPERSBeamline::vespers()->ionChamberCalibration());
	CLSSynchronizedDwellTimeView *dwellTimeView = new CLSSynchronizedDwellTimeView(VESPERSBeamline::vespers()->synchronizedDwellTime());
	VESPERSBeamSelectorView *beamSelectorView = new VESPERSBeamSelectorView;

	QVBoxLayout *current = new QVBoxLayout;
	current->addWidget(slitsView, 0, Qt::AlignCenter);
	current->addWidget(ionCalibrationView, 0, Qt::AlignCenter);
	current->addWidget(beamSelectorView, 0, Qt::AlignCenter);

	QHBoxLayout *next = new QHBoxLayout;
	next->addLayout(current);
	next->addWidget(dwellTimeView);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(top);
	mainLayout->addStretch();
	mainLayout->addLayout(next);
	mainLayout->addStretch();

	setLayout(mainLayout);
}
