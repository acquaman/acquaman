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
	components_->setExclusive(false);

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

	configurations_->button(4)->setChecked(true);
	onConfigurationsChanged(4);

	QHBoxLayout *experimentConfigurationLayout = new QHBoxLayout;
	experimentConfigurationLayout->addWidget(configBox);
	experimentConfigurationLayout->addWidget(compBox);

	setLayout(experimentConfigurationLayout);
}

void VESPERSExperimentConfigurationView::onConfigurationsChanged(int id)
{
	switch(id){

	case 0: // Custom.  No changes.
		break;

	case 1:
		// XAS.  Enable: POE, SOE, and Sample Stage
		components_->button(0)->setChecked(true);
		components_->button(1)->setChecked(true);
		components_->button(2)->setChecked(false);
		components_->button(3)->setChecked(false);
		components_->button(4)->setChecked(false);
		components_->button(5)->setChecked(false);
		components_->button(6)->setChecked(true);
		break;

	case 2:
		// XAS w/ 1-el Vortex.  Enable: POE, SOE, 1-el Vortex, and Sample Stage
		components_->button(0)->setChecked(true);
		components_->button(1)->setChecked(true);
		components_->button(2)->setChecked(true);
		components_->button(3)->setChecked(false);
		components_->button(4)->setChecked(false);
		components_->button(5)->setChecked(false);
		components_->button(6)->setChecked(true);
		break;

	case 3:
		// XAS w/ 4-el Vortex.  Enable: POE, SOE, 4-el Vortex, and Sample Stage
		components_->button(0)->setChecked(true);
		components_->button(1)->setChecked(true);
		components_->button(2)->setChecked(false);
		components_->button(3)->setChecked(true);
		components_->button(4)->setChecked(false);
		components_->button(5)->setChecked(false);
		components_->button(6)->setChecked(true);
		break;

	case 4:
		// XRF w/ 1-el Vortex.  Enable: POE, SOE, 1-el Vortex, and Sample Stage
		components_->button(0)->setChecked(true);
		components_->button(1)->setChecked(true);
		components_->button(2)->setChecked(true);
		components_->button(3)->setChecked(false);
		components_->button(4)->setChecked(false);
		components_->button(5)->setChecked(false);
		components_->button(6)->setChecked(true);
		break;

	case 5:
		// XRF w/ 1-el Vortex + CCD.  Enable: POE, SOE, 1-el Vortex, CCD, Fast Shutter, and Sample Stage
		components_->button(0)->setChecked(true);
		components_->button(1)->setChecked(true);
		components_->button(2)->setChecked(true);
		components_->button(3)->setChecked(false);
		components_->button(4)->setChecked(true);
		components_->button(5)->setChecked(true);
		components_->button(6)->setChecked(true);
		break;

	case 6:
		// XRF w/ 4-el Vortex.  Enable: POE, SOE, 4-el Vortex, and Sample Stage
		components_->button(0)->setChecked(true);
		components_->button(1)->setChecked(true);
		components_->button(2)->setChecked(false);
		components_->button(3)->setChecked(true);
		components_->button(4)->setChecked(false);
		components_->button(5)->setChecked(false);
		components_->button(6)->setChecked(true);
		break;

	case 7:
		// XRF w/ 4-el Vortex.  Enable: POE, SOE, 4-el Vortex, CCD, Fast Shutter, and Sample Stage
		components_->button(0)->setChecked(true);
		components_->button(1)->setChecked(true);
		components_->button(2)->setChecked(false);
		components_->button(3)->setChecked(true);
		components_->button(4)->setChecked(true);
		components_->button(5)->setChecked(true);
		components_->button(6)->setChecked(true);
		break;
	}
}
