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


#include "SGMXASScanConfigurationView.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QSpacerItem>

#include "ui/SGM/SGMFluxResolutionPickerView.h"
#include "ui/dataman/AMControlInfoListView.h"
#include "ui/dataman/AMOldDetectorInfoSetView.h"
#include "ui/dataman/AMStepScanAxisView.h"

SGMXASScanConfigurationView::SGMXASScanConfigurationView(SGMXASScanConfiguration *sxsc, QWidget *parent)  :
	AMScanConfigurationView(parent)
{
	cfg_ = sxsc;

	topFrame_ = new AMTopFrame("Configure an XAS Scan to Run Later");
	topFrame_->setIcon(QIcon(":/utilities-system-monitor.png"));

	// Regions setup
	regionsView_ = new AMStepScanAxisView("", cfg_);

	QVBoxLayout *regionsViewLayout = new QVBoxLayout;
	regionsViewLayout->addWidget(regionsView_);

	QGroupBox *regionsViewGroupBox = new QGroupBox("Regions Setup");
	regionsViewGroupBox->setLayout(regionsViewLayout);

	fluxResolutionView_ = new SGMFluxResolutionPickerStaticView(cfg_->scanAxisAt(0));

	fluxResolutionView_->setFromInfoList(sxsc->fluxResolutionGroup());
	fluxResolutionView_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);

	QMap<int, QString> enumOverrides;
	enumOverrides.insert(0, "Don't Track");
	enumOverrides.insert(1, "Track");
	trackingStaticView_ = new AMControlInfoListView(sxsc->trackingGroup(), enumOverrides);
	trackingStaticView_->setTitle("Tracking");
	xasDetectorsStaticView_ = new AMOldDetectorInfoSetView(sxsc->detectorChoiceConfigurations());

	mainVL_ = new QVBoxLayout();
	mainVL_->addWidget(topFrame_);
	bottomGL_ = new QGridLayout();
	mainVL_->addLayout(bottomGL_, 10);
	bottomGL_->addWidget(regionsViewGroupBox, 0, 0);
	bottomGL_->addWidget(fluxResolutionView_, 1, 0);
	bottomGL_->addWidget(trackingStaticView_, 0, 2);
	bottomGL_->addWidget(xasDetectorsStaticView_, 1, 2);
	bottomGL_->setColumnStretch(0, 10);
	bottomGL_->setColumnMinimumWidth(1, 40);
	bottomGL_->setContentsMargins(10, 0, 0, 0);
	mainVL_->addStretch(8);
	mainVL_->setContentsMargins(0,0,0,0);
	mainVL_->setSpacing(1);
	setLayout(mainVL_);

	this->setMaximumSize(800, 800);
}

SGMXASScanConfigurationView::~SGMXASScanConfigurationView(){
}

const AMScanConfiguration* SGMXASScanConfigurationView::configuration() const{
	return cfg_;
}

void SGMXASScanConfigurationView::setDisabled(bool disabled){
	regionsView_->setDisabled(disabled);
	fluxResolutionView_->setDisabled(disabled);
}
