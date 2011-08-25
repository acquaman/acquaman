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


#include "REIXSXESScanConfigurationDetailedView.h"

#include <QGroupBox>
#include <QBoxLayout>

REIXSXESScanConfigurationDetailedView::REIXSXESScanConfigurationDetailedView(REIXSXESMCPDetector* detector, QWidget *parent) :
	AMScanConfigurationView(parent)
{
	basicConfigurationView_ = new REIXSXESScanConfigurationView();
	detectorView_ = new REIXSXESMCPDetectorView(detector);
	QGroupBox* detectorPreviewBox = new QGroupBox("Detector Preview");
	detectorPreviewBox->setLayout(new QVBoxLayout());
	detectorPreviewBox->layout()->addWidget(detectorView_);

	QHBoxLayout* hl = new QHBoxLayout();
	QVBoxLayout* vl1 = new QVBoxLayout();
	vl1->addWidget(basicConfigurationView_);
	vl1->addStretch();
	hl->addLayout(vl1);
	hl->addWidget(detectorPreviewBox);

	setLayout(hl);
}
