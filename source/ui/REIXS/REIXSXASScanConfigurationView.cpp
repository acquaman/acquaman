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


#include "REIXSXASScanConfigurationView.h"
#include "ui_REIXSXASScanConfigurationView.h"

#include "ui/AMTopFrame2.h"
#include "ui/dataman/AMStepScanAxisView.h"
#include "ui/dataman/AMSamplePre2013Selector.h"
#include "util/AMDateTimeUtils.h"
#include <QStringBuilder>

REIXSXASScanConfigurationView::REIXSXASScanConfigurationView(REIXSXASScanConfiguration* config, QWidget *parent) :
	AMScanConfigurationView(parent),
    ui(new Ui::REIXSXASScanConfigurationView)
{
    ui->setupUi(this);

	if(config)
		config_ = config;
	else
		config_ = new REIXSXASScanConfiguration(this);


	// setup additional UI:
	topFrame_ = new AMTopFrame2("Setup XAS Scan", QIcon(":/utilities-system-monitor.png"));
	ui->outerVLayout->insertWidget(0, topFrame_);

	ui->innerVLayout->insertWidget(0, new AMStepScanAxisView("Region Configuration", config_));
	ui->innerVLayout->addStretch();

	sampleSelector_ = new AMSamplePre2013Selector(AMDatabase::database("user"));
	ui->scanMetaInfoLayout->setWidget(1, QFormLayout::FieldRole, sampleSelector_);


	// Initialize widgets from config_

	ui->applyGratingBox->setChecked(false); //(config_->applyMonoGrating());
	ui->applyGratingBox->setDisabled(true);
	ui->gratingBox->setCurrentIndex(config_->monoGrating());
	ui->gratingBox->setEnabled(config_->applyMonoGrating());
	ui->gratingBox->setDisabled(true);

	ui->applyMirrorBox->setChecked(config_->applyMonoMirror());
	ui->applyMirrorBox->setDisabled(true);
	ui->mirrorBox->setCurrentIndex(false); //(config_->monoMirror());
	ui->mirrorBox->setEnabled(config_->applyMonoMirror());
	ui->mirrorBox->setDisabled(true);


	ui->applySlitWidthBox->setChecked(config_->applySlitWidth());
	ui->slitWidthBox->setValue(config_->slitWidth());
	ui->slitWidthBox->setEnabled(config_->applySlitWidth());

	ui->applyPolarizationBox->setChecked(config_->applyPolarization());
	ui->polarizationBox->setCurrentIndex(config_->polarization());
	ui->polarizationAngleBox->setValue(config_->polarizationAngle());
	ui->polarizationBox->setEnabled(config_->applyPolarization());
	ui->polarizationAngleBox->setEnabled(config_->applyPolarization() && config_->polarization() == 5);

	ui->nameEdit->setText(config_->userScanName());
//	ui->numberEdit->setValue(config_->scanNumber());
	sampleSelector_->setCurrentSample(config_->sampleId());

	ui->namedAutomaticallyBox->setChecked(config_->namedAutomatically());
	ui->nameEdit->setEnabled(!config_->namedAutomatically());
//	ui->numberEdit->setEnabled(false);
	sampleSelector_->setEnabled(!config_->namedAutomatically());


	// Make connections: from widgets to scan configuration.
	connect(ui->applyGratingBox, SIGNAL(clicked(bool)), config_, SLOT(setApplyMonoGrating(bool)));
	connect(ui->applyMirrorBox, SIGNAL(clicked(bool)), config_, SLOT(setApplyMonoMirror(bool)));
	connect(ui->applySlitWidthBox, SIGNAL(clicked(bool)), config_, SLOT(setApplySlitWidth(bool)));
	connect(ui->applyPolarizationBox, SIGNAL(clicked(bool)), config_, SLOT(setApplyPolarization(bool)));
	connect(ui->namedAutomaticallyBox, SIGNAL(clicked(bool)), config_, SLOT(setNamedAutomatically(bool)));

	connect(ui->gratingBox, SIGNAL(currentIndexChanged(int)), config_, SLOT(setMonoGrating(int)));
	connect(ui->mirrorBox, SIGNAL(currentIndexChanged(int)), config_, SLOT(setMonoMirror(int)));
	connect(ui->slitWidthBox, SIGNAL(valueChanged(double)), config_, SLOT(setSlitWidth(double)));
	connect(ui->polarizationBox, SIGNAL(currentIndexChanged(int)), config_, SLOT(setPolarization(int)));
	connect(ui->polarizationAngleBox, SIGNAL(valueChanged(double)), config_, SLOT(setPolarizationAngle(double)));
	connect(ui->nameEdit, SIGNAL(textEdited(QString)), config_, SLOT(setUserScanName(QString)));
//	connect(ui->numberEdit, SIGNAL(valueChanged(int)), config_, SLOT(setScanNumber(int)));
	connect(sampleSelector_, SIGNAL(currentSampleChanged(int)), config_, SLOT(setSampleId(int)));
	connect(ui->namedAutomaticallyBox, SIGNAL(clicked(bool)), config_, SLOT(setNamedAutomatically(bool)));

	// Make connections: from widgets to enable/disable other widgets
	connect(ui->applyGratingBox, SIGNAL(clicked(bool)), ui->gratingBox, SLOT(setEnabled(bool)));
	connect(ui->applyMirrorBox, SIGNAL(clicked(bool)), ui->mirrorBox, SLOT(setEnabled(bool)));
	connect(ui->applySlitWidthBox, SIGNAL(clicked(bool)), ui->slitWidthBox, SLOT(setEnabled(bool)));
	connect(ui->applyPolarizationBox, SIGNAL(clicked(bool)), ui->polarizationBox, SLOT(setEnabled(bool)));
	connect(ui->applyPolarizationBox, SIGNAL(clicked(bool)), this, SLOT(reviewPolarizationAngleBoxEnabled()));
	connect(ui->polarizationBox, SIGNAL(activated(int)), this, SLOT(reviewPolarizationAngleBoxEnabled()));
	connect(ui->namedAutomaticallyBox, SIGNAL(clicked(bool)), ui->nameEdit, SLOT(setDisabled(bool)));
//	connect(ui->namedAutomaticallyBox, SIGNAL(clicked(bool)), ui->numberEdit, SLOT(setDisabled(bool)));
	connect(ui->namedAutomaticallyBox, SIGNAL(clicked(bool)), sampleSelector_, SLOT(setDisabled(bool)));

	connect(config_, SIGNAL(totalTimeChanged(double)), this, SLOT(onEstimatedTimeChanged()));
	connect(config_, SIGNAL(scanAxisAdded(AMScanAxis*)), this, SLOT(onEstimatedTimeChanged()));
	connect(config_, SIGNAL(scanAxisRemoved(AMScanAxis*)), this, SLOT(onEstimatedTimeChanged()));
	connect(config_, SIGNAL(modifiedChanged(bool)), this, SLOT(onEstimatedTimeChanged()));
	connect(config_, SIGNAL(configurationChanged()), this, SLOT(onEstimatedTimeChanged()));

	onRegionsChanged();
}

REIXSXASScanConfigurationView::~REIXSXASScanConfigurationView()
{
	delete ui;
}

void REIXSXASScanConfigurationView::reviewPolarizationAngleBoxEnabled()
{
	ui->polarizationAngleBox->setEnabled(config_->polarization() == 5 && config_->applyPolarization());
}

void REIXSXASScanConfigurationView::onRegionsChanged()
{
	config_->totalTime(true);
	//	QDateTime t1 = QDateTime::currentDateTime();
	//	topFrame_->setLeftSubText("Expected acquisition time: " % AMDateTimeUtils::prettyDuration(t1, t1.addSecs(config_->regions()->totalAcquisitionTime())));

}

void REIXSXASScanConfigurationView::onEstimatedTimeChanged()
{
	QString timeString = "";

	config_->blockSignals(true);
	double time = config_->totalTime(true);
	config_->blockSignals(false);

	//ui->totalPointsLabel->setText(QString("%1").arg(config_->totalPoints()));

	int days = int(time/3600.0/24.0);

	if (days > 0){

		time -= days*3600.0*24;
		timeString += QString::number(days) + "d:";
	}

	int hours = int(time/3600.0);

	if (hours > 0){

		time -= hours*3600;
		timeString += QString::number(hours) + "h:";
	}

	int minutes = int(time/60.0);

	if (minutes > 0){

		time -= minutes*60;
		timeString += QString::number(minutes) + "m:";
	}

	int seconds = ((int)time)%60;
	timeString += QString::number(seconds) + "s";

	//ui->estimatedTimeLabel->setText(timeString);
	topFrame_->setLeftSubText("Expected acquisition time: " % timeString);


}
