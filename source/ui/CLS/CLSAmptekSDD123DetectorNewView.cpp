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


#include "CLSAmptekSDD123DetectorNewView.h"

#include <QHBoxLayout>
#include "beamline/CLS/CLSAmptekSDD123DetectorNew.h"

 CLSAmptekSDD123DetectorNewView::~CLSAmptekSDD123DetectorNewView(){}
CLSAmptekSDD123DetectorNewView::CLSAmptekSDD123DetectorNewView(CLSAmptekSDD123DetectorNew *detector, QWidget *parent) :
	AMDetectorGeneralDetailedView(detector, parent)
{
	detectorTemperatureDSBox_ = new QDoubleSpinBox();
	detectorTemperatureDSBox_->setMinimum(0);
	detectorTemperatureDSBox_->setMaximum(1000);
	if(detector_->isConnected())
		detectorTemperatureDSBox_->setValue(detector->detectorTemperature());
	detectorTemperatureDSBox_->setEnabled(false);
	mainHL_->addWidget(detectorTemperatureDSBox_);

	connect(detector, SIGNAL(detectorTemperatureChanged(double)), detectorTemperatureDSBox_, SLOT(setValue(double)));
}
