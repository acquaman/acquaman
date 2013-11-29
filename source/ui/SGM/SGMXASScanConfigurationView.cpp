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


#include "SGMXASScanConfigurationView.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QSpacerItem>

#include "ui/SGM/SGMFluxResolutionPickerView.h"

SGMXASScanConfigurationView::SGMXASScanConfigurationView(SGMXASScanConfiguration *sxsc, QWidget *parent)  :
		AMScanConfigurationView(parent)
{
	cfg_ = NULL;

	//if(SGMBeamline::sgm()->isConnected()){
		cfg_ = sxsc;

		topFrame_ = new AMTopFrame("Configure an XAS Scan to Run Later");
		topFrame_->setIcon(QIcon(":/utilities-system-monitor.png"));

		//This should be done somewhere else
		//sxsc->xasRegions()->setEnergyControl(SGMBeamline::sgm()->energy());

		regionsLineView_ = new AMRegionsLineView(sxsc->regions(), this);
		if(SGMBeamline::sgm()->isConnected())
			regionsView_ = new AMRegionsView(sxsc->regions(), this);
		else
			regionsView_ = new AMRegionsStaticView(sxsc->regions());

		if(SGMBeamline::sgm()->isConnected()){
			fluxResolutionView_ = new SGMFluxResolutionPickerView(sxsc->xasRegions(), this);
			connect(fluxResolutionView_, SIGNAL(configValuesChanged(AMControlInfoList)), sxsc, SLOT(setFluxResolutionGroup(AMControlInfoList)));
			connect(sxsc, SIGNAL(fluxResolutionGroupChanged(AMControlInfoList)), fluxResolutionView_, SLOT(setFromInfoList(AMControlInfoList)));
		}
		else
			fluxResolutionView_ = new SGMFluxResolutionPickerStaticView(sxsc->xasRegions());

		fluxResolutionView_->setFromInfoList(sxsc->fluxResolutionGroup());
		fluxResolutionView_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);

		if(SGMBeamline::sgm()->isConnected()){
			trackingStaticView_ = 0; //NULL
			//trackingView_ = new AMControlSetView(sxsc->trackingSet(), true, this);
			trackingView_ = new AMControlSetView(SGMBeamline::sgm()->trackingSet(), true, this);
			trackingView_->setFromInfoList(sxsc->trackingGroup());
			connect(trackingView_, SIGNAL(configValuesChanged(AMControlInfoList)), sxsc, SLOT(setTrackingGroup(AMControlInfoList)));
			connect(sxsc, SIGNAL(trackingGroupChanged(AMControlInfoList)), trackingView_, SLOT(setFromInfoList(AMControlInfoList)));
		}
		else{
			trackingView_ = 0; //NULL
			QMap<int, QString> enumOverrides;
			enumOverrides.insert(0, "Don't Track");
			enumOverrides.insert(1, "Track");
			trackingStaticView_ = new AMControlInfoListView(sxsc->trackingGroup(), enumOverrides);
			trackingStaticView_->setTitle("Tracking");
		}

		if(SGMBeamline::sgm()->isConnected()){
			xasDetectorsStaticView_ = 0; //NULL
			//xasDetectorsView_ = new AMOldDetectorSetView(sxsc->detectorChoices(), sxsc->detectorChoiceConfigurations());
			xasDetectorsView_ = new AMOldDetectorSetView(SGMBeamline::sgm()->XASDetectors(), sxsc->detectorChoiceConfigurations());
			connect(xasDetectorsView_, SIGNAL(configValuesChanged()), this, SLOT(onDetectorConfigurationsChanged()));
		}
		else{
			xasDetectorsView_ = 0; //NULL
			xasDetectorsStaticView_ = new AMOldDetectorInfoSetView(sxsc->detectorChoiceConfigurations());
		}

		warningsLabel_ = new QLabel("");
		QFont warningsFont;
		warningsFont.setPointSize(48);
		warningsLabel_->setFont(warningsFont);
		warningsLabel_->setStyleSheet( "QLabel{ color: red }" );

		scanNameLabel_ = new QLabel("Scan Name");
		scanNameEdit_ = new QLineEdit(this);
		connect(scanNameEdit_, SIGNAL(textEdited(QString)), this, SLOT(onScanNameEditChanged(QString)));

		mainVL_ = new QVBoxLayout();
		mainVL_->addWidget(topFrame_);
		mainVL_->addWidget(regionsLineView_);
		bottomGL_ = new QGridLayout();
		mainVL_->addLayout(bottomGL_, 10);
		bottomGL_->addWidget(regionsView_,		0, 0);
		bottomGL_->addWidget(fluxResolutionView_,	1, 0);
		if(SGMBeamline::sgm()->isConnected())
			bottomGL_->addWidget(trackingView_,		0, 2);
		else
			bottomGL_->addWidget(trackingStaticView_,	0, 2);
		if(SGMBeamline::sgm()->isConnected())
			bottomGL_->addWidget(xasDetectorsView_,	1, 2);
		else
			bottomGL_->addWidget(xasDetectorsStaticView_,	1, 2);
		bottomGL_->setColumnStretch(0, 10);
		bottomGL_->setColumnMinimumWidth(1, 40);
		bottomGL_->setContentsMargins(10, 0, 0, 0);
		mainVL_->addStretch(8);
		QHBoxLayout *nameHL = new QHBoxLayout();
		nameHL->addWidget(scanNameLabel_);
		nameHL->addWidget(scanNameEdit_);
		nameHL->setContentsMargins(10,0,0,0);
		mainVL_->addLayout(nameHL);
		mainVL_->setContentsMargins(0,0,0,0);
		mainVL_->setSpacing(1);
		setLayout(mainVL_);

		this->setMaximumSize(800, 800);
		if(SGMBeamline::sgm()->isConnected()){
			connect(SGMBeamline::sgm(), SIGNAL(criticalControlsConnectionsChanged()), this, SLOT(onSGMBeamlineCriticalControlsConnectedChanged()));
			onSGMBeamlineCriticalControlsConnectedChanged();
		}
	//}
}

SGMXASScanConfigurationView::~SGMXASScanConfigurationView(){
}

const AMScanConfiguration* SGMXASScanConfigurationView::configuration() const{
	cfg_->setDetectorConfigurations(xasDetectorsView_->configValues());
	return cfg_;
}

void SGMXASScanConfigurationView::setDisabled(bool disabled){
	regionsView_->setDisabled(disabled);
	fluxResolutionView_->setDisabled(disabled);
	trackingView_->setDisabled(disabled);
	xasDetectorsView_->setDisabled(disabled);
}

void SGMXASScanConfigurationView::onDetectorConfigurationsChanged(){
	cfg_->setDetectorConfigurations(xasDetectorsView_->configValues());
}

void SGMXASScanConfigurationView::onSGMBeamlineCriticalControlsConnectedChanged(){
	if(SGMBeamline::sgm()->isConnected()){
		regionsView_->setEnabled(true);
		regionsLineView_->setEnabled(true);
		fluxResolutionView_->setEnabled(true);
		trackingView_->setEnabled(true);
		xasDetectorsView_->setEnabled(true);
		warningsLabel_->setText("");
	}
	else{
		regionsView_->setEnabled(false);
		regionsLineView_->setEnabled(false);
		fluxResolutionView_->setEnabled(false);
		trackingView_->setEnabled(false);
		xasDetectorsView_->setEnabled(false);
		warningsLabel_->setText("SGM Beamline Unavailable");
	}
}

void SGMXASScanConfigurationView::onScanNameEditChanged(const QString &scanName){
	cfg_->setUserScanName(scanName);
}
