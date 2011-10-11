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

#include "VESPERSExperimentConfigurationView.h"

#include "ui/AMTopFrame.h"

#include <QButtonGroup>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QRadioButton>
#include <QHBoxLayout>
#include <QLabel>

VESPERSExperimentConfigurationView::VESPERSExperimentConfigurationView(VESPERSExperimentConfiguration *experimentConfiguration, QWidget *parent)
	: QWidget(parent)
{
	AMTopFrame *frame = new AMTopFrame("VESPERS Experimental Setup");

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

	connect(configurations_, SIGNAL(buttonClicked(int)), experimentConfiguration_, SLOT(setType(int)));

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

	component = new QCheckBox("SOE");
	components_->addButton(component, 1);
	connect(experimentConfiguration_, SIGNAL(SOEStatusChanged(bool)), component, SLOT(setChecked(bool)));
	connect(component, SIGNAL(toggled(bool)), experimentConfiguration_, SLOT(useSOEStatus(bool)));
	compLayout->addWidget(component);

	component = new QCheckBox("1-el Vortex");
	components_->addButton(component, 2);
	connect(experimentConfiguration_, SIGNAL(singleElementVortexStatusChanged(bool)), component, SLOT(setChecked(bool)));
	connect(component, SIGNAL(toggled(bool)), experimentConfiguration_, SLOT(useSingleElementVortex(bool)));
	compLayout->addWidget(component);

	component = new QCheckBox("4-el Vortex");
	components_->addButton(component, 3);
	connect(experimentConfiguration_, SIGNAL(fourElementVortexStatusChanged(bool)), component, SLOT(setChecked(bool)));
	connect(component, SIGNAL(toggled(bool)), experimentConfiguration_, SLOT(useFourElementVortex(bool)));
	compLayout->addWidget(component);

	component = new QCheckBox("CCD");
	components_->addButton(component, 4);
	connect(experimentConfiguration_, SIGNAL(CCDStatusChanged(bool)), component, SLOT(setChecked(bool)));
	connect(component, SIGNAL(toggled(bool)), experimentConfiguration_, SLOT(useCCDStatus(bool)));
	compLayout->addWidget(component);

	component = new QCheckBox("Fast Shutter");
	components_->addButton(component, 5);
	connect(experimentConfiguration_, SIGNAL(fastShutterStatusChanged(bool)), component, SLOT(setChecked(bool)));
	connect(component, SIGNAL(toggled(bool)), experimentConfiguration_, SLOT(useFastShutterStatus(bool)));
	compLayout->addWidget(component);

	component = new QCheckBox("Sample Stage");
	components_->addButton(component, 6);
	connect(experimentConfiguration_, SIGNAL(sampleStageStatusChanged(bool)), component, SLOT(setChecked(bool)));
	connect(component, SIGNAL(toggled(bool)), experimentConfiguration_, SLOT(useSampleStageStatus(bool)));
	compLayout->addWidget(component);

	connect(components_, SIGNAL(buttonClicked(int)), this, SLOT(onComponentsChanged()));

	QGroupBox *compBox = new QGroupBox("Components");
	compBox->setLayout(compLayout);

	configurations_->button(0)->setChecked(true);
	experimentConfiguration_->setType(VESPERSExperimentConfiguration::Custom);

	QString message("Use the lists below to select what kind of experiment you are going to perform.  \nThis will automatically set some of the important beamline parameters for you.  \nYou should return to this screen every time you decide to change experiments.");

	QHBoxLayout *experimentConfigurationLayout = new QHBoxLayout;
	experimentConfigurationLayout->addStretch();
	experimentConfigurationLayout->addWidget(configBox);
	experimentConfigurationLayout->addWidget(compBox);
	experimentConfigurationLayout->addStretch();

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(frame);
	mainLayout->addStretch();
	mainLayout->addWidget(new QLabel(message), 0, Qt::AlignCenter);
	mainLayout->addLayout(experimentConfigurationLayout);
	mainLayout->addStretch();

	setLayout(mainLayout);
}
