/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "AMRegionScanConfigurationView.h"

#include "ui/AMTopFrame.h"
#include "ui/acquaman/AMRegionsView.h"
#include "ui/acquaman/AMRegionsLineView.h"

#include <QBoxLayout>

AMRegionScanConfigurationView::AMRegionScanConfigurationView(AMRegionScanConfiguration *configuration, QWidget *parent)
	: AMScanConfigurationView(parent)
{
	configuration_ = configuration;

	// GUI setup
	//////////////////////////////

//	topFrame_ = new AMTopFrame("Configure an XAS Scan to Run Later");
//	topFrame_->setIcon(QIcon(":/utilities-system-monitor.png"));

	regionsLineView_ = new AMRegionsLineView(configuration_->regions());
	regionsView_ = new AMRegionsView(configuration_->regions());

	QVBoxLayout* vl = new QVBoxLayout();
	vl->addWidget(regionsLineView_);
	vl->addWidget(regionsView_);
	setLayout(vl);
}

AMRegionScanConfigurationView::~AMRegionScanConfigurationView() {

}



