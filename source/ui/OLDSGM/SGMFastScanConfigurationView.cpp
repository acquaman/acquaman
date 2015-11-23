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


#include "SGMFastScanConfigurationView.h"

#include <QGridLayout>
#include <QComboBox>
#include <QLabel>
#include <QFileDialog>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QFormLayout>

#include "ui/AMTopFrame.h"

 SGMFastScanConfigurationView::~SGMFastScanConfigurationView(){}
SGMFastScanConfigurationView::SGMFastScanConfigurationView(SGMFastScanConfiguration *sfsc, QWidget *parent) :
		AMScanConfigurationView(parent)
{
	cfg_ = NULL;

	cfg_ = sfsc;

	topFrame_ = new AMTopFrame("Configure a Fast Scan to Run Later");
	topFrame_->setIcon(QIcon(":/utilities-system-monitor.png"));

	presetsComboBox_ = new QComboBox();
	presetsComboBox_->addItems(sfsc->presets());
	connect(presetsComboBox_, SIGNAL(currentIndexChanged(int)), sfsc, SLOT(setParametersFromPreset(int)));

	connect(sfsc, SIGNAL(startPositionChanged()), this, SLOT(onParametersStartPositionChanged()));
	connect(sfsc, SIGNAL(middlePositionChanged()), this, SLOT(onParametersMiddlePositionChanged()));
	connect(sfsc, SIGNAL(endPositionChanged()), this, SLOT(onParametersEndPositionChanged()));
	connect(sfsc, SIGNAL(fastScanSettingsChanged()), this, SLOT(onFastScanSettingsChanged()));

	warningsLabel_ = new QLabel("");
	QFont warningsFont;
	warningsFont.setPointSize(48);
	warningsLabel_->setFont(warningsFont);
	warningsLabel_->setStyleSheet( "QLabel{ color: red }" );

	scanNameLabel_ = new QLabel("Scan Name");
	scanNameEdit_ = new QLineEdit();

	connect(scanNameEdit_, SIGNAL(textEdited(QString)), this, SLOT(onScanNameEditChanged(QString)));

	startPositionView_ = 0;
	middlePositionView_ = 0;
	endPositionView_ = 0;
	fastScanSettingsView_ = 0;

	onParametersStartPositionChanged();
	onParametersMiddlePositionChanged();
	onParametersEndPositionChanged();
	onFastScanSettingsChanged();

	if(SGMBeamline::sgm()->isConnected()){
		startPositionView_ = new SGMEnergyPositionWBeamlineView(&startPositionCopy_, SGMEnergyPositionView::ViewModeStartOrEnd);
		middlePositionView_ = new SGMEnergyPositionWBeamlineView(&middlePositionCopy_, SGMEnergyPositionView::ViewModeMiddle);
		endPositionView_ = new SGMEnergyPositionWBeamlineView(&endPositionCopy_, SGMEnergyPositionView::ViewModeStartOrEnd);
	}
	else{
		startPositionView_ = new SGMEnergyPositionView(&startPositionCopy_, SGMEnergyPositionView::ViewModeStartOrEnd);
		middlePositionView_ = new SGMEnergyPositionView(&middlePositionCopy_, SGMEnergyPositionView::ViewModeMiddle);
		endPositionView_ = new SGMEnergyPositionView(&endPositionCopy_, SGMEnergyPositionView::ViewModeStartOrEnd);
	}
	fastScanSettingsView_ = new SGMFastScanSettingsView(&fastScanSettingsCopy_);

	QHBoxLayout *presetsLayout = new QHBoxLayout();
	presetsLayout->addWidget(presetsComboBox_);
	presetsLayout->addStretch(10);

	QVBoxLayout *settingsLayout = new QVBoxLayout();
	settingsLayout->addWidget(fastScanSettingsView_);
	settingsLayout->addStretch(8);
	settingsLayout->setContentsMargins(5,0,5,0);

	QVBoxLayout *positionsLayout = new QVBoxLayout();
	positionsLayout->addWidget(startPositionView_);
	positionsLayout->addWidget(middlePositionView_);
	positionsLayout->addWidget(endPositionView_);
	positionsLayout->addStretch(8);
	positionsLayout->setContentsMargins(5,0,5,0);

	QHBoxLayout *hl = new QHBoxLayout();
	hl->addLayout(settingsLayout);
	hl->addLayout(positionsLayout);

	QHBoxLayout *nameHL = new QHBoxLayout();
	nameHL->addWidget(scanNameLabel_);
	nameHL->addWidget(scanNameEdit_);

	QVBoxLayout *mainLayout = new QVBoxLayout();
	mainLayout->addLayout(presetsLayout);
	mainLayout->addLayout(hl);
	mainLayout->addWidget(warningsLabel_);
	mainLayout->addStretch(10);
	mainLayout->addLayout(nameHL);

	QVBoxLayout *vl = new QVBoxLayout();
	vl->addWidget(topFrame_);
	vl->addLayout(mainLayout);
	vl->setContentsMargins(0,0,0,0);
	vl->setSpacing(1);
	mainLayout->setContentsMargins(10, 0, 10, 0);
	this->setLayout(vl);
	this->setMaximumSize(700, 800);

	if(SGMBeamline::sgm()->isConnected()){
		connect(SGMBeamline::sgm(), SIGNAL(criticalControlsConnectionsChanged()), this, SLOT(onSGMBeamlineCriticalControlsConnectedChanged()));
		onSGMBeamlineCriticalControlsConnectedChanged();
	}
}

const AMScanConfiguration* SGMFastScanConfigurationView::configuration() const{
	return cfg_;
}

void SGMFastScanConfigurationView::onSGMBeamlineCriticalControlsConnectedChanged(){
	if(SGMBeamline::sgm()->isConnected()){
		presetsComboBox_->setEnabled(true);
		startPositionView_->setEnabled(true);
		middlePositionView_->setEnabled(true);
		endPositionView_->setEnabled(true);
		fastScanSettingsView_->setEnabled(true);
		warningsLabel_->setText("");
	}
	else{
		presetsComboBox_->setEnabled(false);
		startPositionView_->setEnabled(false);
		middlePositionView_->setEnabled(false);
		endPositionView_->setEnabled(false);
		fastScanSettingsView_->setEnabled(false);
		warningsLabel_->setText("SGM Beamline Unavailable");
	}
}

void SGMFastScanConfigurationView::onScanNameEditChanged(const QString &scanName){
	cfg_->setUserScanName(scanName);
}

void SGMFastScanConfigurationView::onParametersStartPositionChanged(){
	disconnect(&startPositionCopy_, 0);
	startPositionCopy_ = cfg_->currentParameters()->scanInfo().start();
	if(startPositionView_)
		startPositionView_->setEnergyPosition(&startPositionCopy_);
	connect(&startPositionCopy_, SIGNAL(energyPositionChanged()), this, SLOT(onStartPositionCopyChanged()));
}

void SGMFastScanConfigurationView::onParametersMiddlePositionChanged(){
	disconnect(&middlePositionCopy_, 0);
	middlePositionCopy_ = cfg_->currentParameters()->scanInfo().middle();
	if(middlePositionView_)
		middlePositionView_->setEnergyPosition(&middlePositionCopy_);
	connect(&middlePositionCopy_, SIGNAL(energyPositionChanged()), this, SLOT(onMiddlePositionCopyChanged()));
}

void SGMFastScanConfigurationView::onParametersEndPositionChanged(){
	disconnect(&endPositionCopy_, 0);
	endPositionCopy_ = cfg_->currentParameters()->scanInfo().end();
	if(endPositionView_)
		endPositionView_->setEnergyPosition(&endPositionCopy_);
	connect(&endPositionCopy_, SIGNAL(energyPositionChanged()), this, SLOT(onEndPositionCopyChanged()));
}

void SGMFastScanConfigurationView::onFastScanSettingsChanged(){
	disconnect(&fastScanSettingsCopy_, 0);
	fastScanSettingsCopy_ = cfg_->currentParameters()->fastScanSettings();
	if(fastScanSettingsView_)
		fastScanSettingsView_->setFastScanSettings(&fastScanSettingsCopy_);
	connect(&fastScanSettingsCopy_, SIGNAL(fastScanSettingsChanged()), this, SLOT(onFastScanSettingsCopyChanged()));
}

void SGMFastScanConfigurationView::onStartPositionCopyChanged(){
	cfg_->currentParameters()->setStartPosition(startPositionCopy_);
}

void SGMFastScanConfigurationView::onMiddlePositionCopyChanged(){
	cfg_->currentParameters()->setMiddlePosition(middlePositionCopy_);
}

void SGMFastScanConfigurationView::onEndPositionCopyChanged(){
	cfg_->currentParameters()->setEndPosition(endPositionCopy_);
}

void SGMFastScanConfigurationView::onFastScanSettingsCopyChanged(){
	cfg_->currentParameters()->setFastScanSettings(fastScanSettingsCopy_);
}
