#include "VESPERSExperimentConfigurationView.h"

#include "beamline/VESPERS/VESPERSBeamline.h"

#include <QButtonGroup>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QRadioButton>
#include <QHBoxLayout>

VESPERSExperimentConfigurationView::VESPERSExperimentConfigurationView(QWidget *parent) :
	QWidget(parent)
{
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

	connect(configurations_, SIGNAL(buttonClicked(int)), this, SLOT(onConfigurationsChanged(int)));

	QGroupBox *configBox = new QGroupBox("Configurations");
	configBox->setLayout(configLayout);

	components_ = new QButtonGroup;
	QVBoxLayout *compLayout = new QVBoxLayout;

	QCheckBox *component = new QCheckBox("POE");
	components_->addButton(component, 0);
	connect(component, SIGNAL(toggled(bool)), VESPERSBeamline::vespers(), SLOT(usePOEStatus(bool)));
	compLayout->addWidget(component);

	component = new QCheckBox("SOE");
	components_->addButton(component, 1);
	connect(component, SIGNAL(toggled(bool)), VESPERSBeamline::vespers(), SLOT(useSOEStatus(bool)));
	compLayout->addWidget(component);

	component = new QCheckBox("1-el Vortex");
	components_->addButton(component, 2);
	connect(component, SIGNAL(toggled(bool)), VESPERSBeamline::vespers(), SLOT(useSingleElementVortex(bool)));
	compLayout->addWidget(component);

	component = new QCheckBox("4-el Vortex");
	components_->addButton(component, 3);
	connect(component, SIGNAL(toggled(bool)), VESPERSBeamline::vespers(), SLOT(useFourElementVortex(bool)));
	compLayout->addWidget(component);

	component = new QCheckBox("CCD");
	components_->addButton(component, 4);
	connect(component, SIGNAL(toggled(bool)), VESPERSBeamline::vespers(), SLOT(useCCDStatus(bool)));
	compLayout->addWidget(component);

	component = new QCheckBox("Fast Shutter");
	components_->addButton(component, 5);
	connect(component, SIGNAL(toggled(bool)), VESPERSBeamline::vespers(), SLOT(useFastShutterStatus(bool)));
	compLayout->addWidget(component);

	component = new QCheckBox("Sample Stage");
	components_->addButton(component, 6);
	connect(component, SIGNAL(toggled(bool)), VESPERSBeamline::vespers(), SLOT(useSampleStageStatus(bool)));
	compLayout->addWidget(component);

	connect(components_, SIGNAL(buttonClicked(int)), this, SLOT(onComponentsChanged()));

	QGroupBox *compBox = new QGroupBox("Components");
	compBox->setLayout(compLayout);

	QHBoxLayout *experimentConfigurationLayout = new QHBoxLayout;
	experimentConfigurationLayout->addWidget(configBox);
	experimentConfigurationLayout->addWidget(compBox);

	setLayout(experimentConfigurationLayout);
}

void VESPERSExperimentConfigurationView::onConfigurationsChanged(int id)
{

}
