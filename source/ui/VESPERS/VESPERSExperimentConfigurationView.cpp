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

#include "VESPERSExperimentConfigurationView.h"

#include "ui/AMTopFrame.h"
#include "ui/VESPERS/VESPERSMapSetupView.h"

#include <QButtonGroup>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QRadioButton>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>

VESPERSExperimentConfigurationView::VESPERSExperimentConfigurationView(VESPERSExperimentConfiguration *experimentConfiguration, QWidget *parent)
	: QWidget(parent)
{
	AMTopFrame *frame = new AMTopFrame("VESPERS Experiment Setup");

	mapSetup_ = new VESPERSMapSetup;
	VESPERSMapSetupView *mapSetupView = new VESPERSMapSetupView(mapSetup_, this);

	experimentConfiguration_ = experimentConfiguration;

	configurations_ = new QButtonGroup;
	QVBoxLayout *configLayout = new QVBoxLayout;

	QRadioButton *types = new QRadioButton("Custom");
	configurations_->addButton(types, 0);
	configLayout->addWidget(types);

	types = new QRadioButton("XAS");
	configurations_->addButton(types, 1);
	configLayout->addWidget(types);

	types = new QRadioButton("XAS w/ 1-el Vortex");
	configurations_->addButton(types, 2);
	configLayout->addWidget(types);

	types = new QRadioButton("XAS w/ 4-el Vortex");
	configurations_->addButton(types, 3);
	configLayout->addWidget(types);

	types = new QRadioButton("XRF w/ 1-el Vortex");
	configurations_->addButton(types, 4);
	configLayout->addWidget(types);

	types = new QRadioButton("XRF w/ 1-el Vortex and XRD");
	configurations_->addButton(types, 5);
	configLayout->addWidget(types);

	types = new QRadioButton("XRF w/ 4-el Vortex");
	configurations_->addButton(types, 6);
	configLayout->addWidget(types);

	types = new QRadioButton("XRF w/ 4-el Vortex and XRD");
	configurations_->addButton(types, 7);
	configLayout->addWidget(types);

	connect(configurations_, SIGNAL(buttonClicked(int)), this, SLOT(onConfiguraitonChanged(int)));

	QGroupBox *configBox = new QGroupBox("Configurations");
	configBox->setLayout(configLayout);

	components_ = new QButtonGroup;
	components_->setExclusive(false);

	QVBoxLayout *compLayout = new QVBoxLayout;

	QCheckBox *component = new QCheckBox("POE");
	components_->addButton(component, 0);
	connect(experimentConfiguration_, SIGNAL(POEStatusChanged(bool)), component, SLOT(setChecked(bool)));
	connect(component, SIGNAL(toggled(bool)), experimentConfiguration_, SLOT(usePOEStatus(bool)));
	compLayout->addWidget(component);

	component = new QCheckBox("1-el Vortex");
	components_->addButton(component, 1);
	connect(experimentConfiguration_, SIGNAL(singleElementVortexStatusChanged(bool)), component, SLOT(setChecked(bool)));
	connect(component, SIGNAL(toggled(bool)), experimentConfiguration_, SLOT(useSingleElementVortex(bool)));
	compLayout->addWidget(component);

	component = new QCheckBox("4-el Vortex");
	components_->addButton(component, 2);
	connect(experimentConfiguration_, SIGNAL(fourElementVortexStatusChanged(bool)), component, SLOT(setChecked(bool)));
	connect(component, SIGNAL(toggled(bool)), experimentConfiguration_, SLOT(useFourElementVortex(bool)));
	compLayout->addWidget(component);

	component = new QCheckBox("CCD");
	components_->addButton(component, 3);
	connect(experimentConfiguration_, SIGNAL(CCDStatusChanged(bool)), component, SLOT(setChecked(bool)));
	connect(component, SIGNAL(toggled(bool)), experimentConfiguration_, SLOT(useCCDStatus(bool)));
	compLayout->addWidget(component);

	component = new QCheckBox("Fast Shutter");
	components_->addButton(component, 4);
	connect(experimentConfiguration_, SIGNAL(fastShutterStatusChanged(bool)), component, SLOT(setChecked(bool)));
	connect(component, SIGNAL(toggled(bool)), experimentConfiguration_, SLOT(useFastShutterStatus(bool)));
	compLayout->addWidget(component);

	component = new QCheckBox("Sample Stage");
	components_->addButton(component, 5);
	connect(experimentConfiguration_, SIGNAL(sampleStageStatusChanged(bool)), component, SLOT(setChecked(bool)));
	connect(component, SIGNAL(toggled(bool)), experimentConfiguration_, SLOT(useSampleStageStatus(bool)));
	compLayout->addWidget(component);

	connect(components_, SIGNAL(buttonClicked(int)), this, SLOT(onComponentsChanged()));

	QGroupBox *compBox = new QGroupBox("Components");
	compBox->setLayout(compLayout);

	configurations_->button(0)->setChecked(true);
	experimentConfiguration_->setType(VESPERSExperimentConfiguration::Custom);

	QString message("Use the lists below to select what kind of experiment you are going to perform.  \nThis will automatically set some of the important beamline parameters for you.  \nYou should return to this screen every time you decide to change experiments.");

	sampleStage_ = new QButtonGroup;
	QHBoxLayout *sampleStageLayout = new QHBoxLayout;

	QRadioButton *stage = new QRadioButton("X && Z");
	sampleStage_->addButton(stage, 0);
	sampleStageLayout->addWidget(stage);

	stage = new QRadioButton("H && V");
	sampleStage_->addButton(stage, 1);
	sampleStageLayout->addWidget(stage);
	connect(sampleStage_, SIGNAL(buttonClicked(int)), this, SLOT(onSampleStageChanged(int)));
	connect(experimentConfiguration_, SIGNAL(sampleStageChoiceChanged(bool)), this, SLOT(onSampleStageUpdated(bool)));

	// The button for the pseudo-motor reset.
	resetPseudoMotorsButton_ = new QPushButton(QIcon(":/reset.png"), "Reset Pseudo-Motors");
	sampleStageLayout->addWidget(resetPseudoMotorsButton_);
	connect(resetPseudoMotorsButton_, SIGNAL(clicked()), experimentConfiguration_, SLOT(resetHVNPseudoMotors()));

	QGroupBox *sampleStageBox = new QGroupBox("Sample Stage");
	sampleStageBox->setLayout(sampleStageLayout);

	QHBoxLayout *experimentConfigurationLayout = new QHBoxLayout;
	experimentConfigurationLayout->addStretch();
	experimentConfigurationLayout->addWidget(configBox);
	experimentConfigurationLayout->addWidget(compBox);
	experimentConfigurationLayout->addStretch();

	QVBoxLayout *experimentLayout = new QVBoxLayout;
	experimentLayout->addStretch();
	experimentLayout->addWidget(sampleStageBox, 0, Qt::AlignCenter);
	experimentLayout->addWidget(new QLabel(message), 0, Qt::AlignCenter);
	experimentLayout->addLayout(experimentConfigurationLayout);
	experimentLayout->addStretch();

	QHBoxLayout *mainLayout = new QHBoxLayout;
	mainLayout->addLayout(experimentLayout);
	mainLayout->addWidget(mapSetupView);

	QVBoxLayout *frameLayout = new QVBoxLayout;
	frameLayout->addWidget(frame);
	frameLayout->addLayout(mainLayout);

	setLayout(frameLayout);
}

void VESPERSExperimentConfigurationView::onConfiguraitonChanged(int id)
{
	experimentConfiguration_->setType(id);

	switch(id){

	case VESPERSExperimentConfiguration::XRFw1el:
		mapSetup_->set2D();
		mapSetup_->useSingleElement();
		mapSetup_->useCCD(false);
		mapSetup_->useMultiImages(false);
		break;

	case VESPERSExperimentConfiguration::XRFw1elAndXRD:
		mapSetup_->set2D();
		mapSetup_->useSingleElement();
		mapSetup_->useCCD(true);
		mapSetup_->useMultiImages(false);
		break;

	case VESPERSExperimentConfiguration::XRFw4el:
		mapSetup_->set2D();
		mapSetup_->useFourElement();
		mapSetup_->useCCD(false);
		mapSetup_->useMultiImages(false);
		break;

	case VESPERSExperimentConfiguration::XRFw4elAndXRD:
		mapSetup_->set2D();
		mapSetup_->useFourElement();
		mapSetup_->useCCD(true);
		mapSetup_->useMultiImages(false);
		break;

	default:
		break;
	}
}

void VESPERSExperimentConfigurationView::onSampleStageChanged(int id)
{
	if (id == 0){

		experimentConfiguration_->usePseudoMotors(false);
		QMessageBox message(QMessageBox::Question, "Switching Sample Stages", "If you are actually switching to using the X & Z sample stage after using the H & V sample stage you must reset the pseudo motors.  Would you like to reset them now?");
		QPushButton *resetLater = message.addButton("Later", QMessageBox::RejectRole);
		QPushButton *resetNow = message.addButton("Reset now", QMessageBox::AcceptRole);
		message.setEscapeButton(resetLater);
		message.setDefaultButton(resetNow);
		message.exec();

		if (message.result() == QDialog::Accepted)
			experimentConfiguration_->resetXYZPseudoMotors();
	}
	else{

		experimentConfiguration_->usePseudoMotors(true);
		QMessageBox message(QMessageBox::Question, "Switching Sample Stages", "If you are actually switching to using the H & V sample stage after using the X & Z sample stage you must reset the pseudo motors.  Would you like to reset them now?");
		QPushButton *resetLater = message.addButton("Later", QMessageBox::RejectRole);
		QPushButton *resetNow = message.addButton("Reset now", QMessageBox::AcceptRole);
		message.setEscapeButton(resetLater);
		message.setDefaultButton(resetNow);
		message.exec();

		if (message.result() == QDialog::Accepted)
			experimentConfiguration_->resetHVNPseudoMotors();
	}
}

void VESPERSExperimentConfigurationView::onSampleStageUpdated(bool usingPseudoMotors)
{
	sampleStage_->button(usingPseudoMotors ? 1 : 0)->setChecked(true);
	resetPseudoMotorsButton_->setText(QString("Reset %1 Pseudo-Motors").arg(usingPseudoMotors ? "H & V" : "X & Z"));
}
