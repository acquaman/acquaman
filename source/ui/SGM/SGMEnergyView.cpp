#include "SGMEnergyView.h"

#include "ui/beamline/AMExtendedControlEditor.h"
#include "beamline/SGM/energy/SGMEnergyControlSet.h"

#include <QVBoxLayout>
#include <QGridLayout>
SGMEnergyView::SGMEnergyView(SGMEnergyControlSet* energyControlSet,
							 EnergyViewType viewType,
							 QWidget *parent) :
    QWidget(parent)
{

	energyControlSet_ = energyControlSet;

	setupUi(viewType);
}

void SGMEnergyView::onUndulatorTrackingButtonToggled(bool isToggled)
{
	double toggledPVValue;
	if(isToggled) {

		toggledPVValue = 1.0;
	} else {

		toggledPVValue = 0.0;
	}

	if(energyControlSet_->undulatorTracking()->withinTolerance(toggledPVValue)) {

		energyControlSet_->undulatorTracking()->move(toggledPVValue);
	}
}

void SGMEnergyView::onExitSlitTrackingButtonToggled(bool isToggled)
{
	double toggledPVValue;
	if(isToggled) {

		toggledPVValue = 1.0;
	} else {

		toggledPVValue = 0.0;
	}

	if(energyControlSet_->exitSlitPositionTracking()->withinTolerance(toggledPVValue)) {

		energyControlSet_->exitSlitPositionTracking()->move(toggledPVValue);
	}
}

void SGMEnergyView::onEnergyControlUndulatorTrackingChanged(double)
{
	undulatorTrackingToggleButton_->setChecked(!energyControlSet_->undulatorTracking()->withinTolerance(0.0));
}

void SGMEnergyView::onEnergyControlExitSlitTrackingChanged(double)
{
	exitSlitTrackingToggleButton_->setChecked(!energyControlSet_->exitSlitPositionTracking()->withinTolerance(0.0));
}

void SGMEnergyView::onEnergyControlGratingTrackingOptimizationModeChanged(double)
{
	// Set the grating translation selection control to read only if the optimization
	// mode is not set to manual (0x00).
	gratingTranslationEditor_->setReadOnlyPreference(!energyControlSet_->gratingTranslationOptimization()->withinTolerance(0));
}

void SGMEnergyView::setupUi(SGMEnergyView::EnergyViewType viewType)
{

	energyEditor_ = new AMExtendedControlEditor(energyControlSet_->energy());
	gratingTranslationModeEditor_ = new AMExtendedControlEditor(energyControlSet_->gratingTranslationOptimization());
	gratingTranslationEditor_ = new AMExtendedControlEditor(energyControlSet_->gratingTranslation());
	undulatorHarmonicEditor_ = new AMExtendedControlEditor(energyControlSet_->undulatorHarmonic());

	if(viewType == Advanced) {
		gratingAngleEditor_ = new AMExtendedControlEditor(energyControlSet_->gratingAngle());
		undulatorPositionEditor_ = new AMExtendedControlEditor(energyControlSet_->undulatorPosition());
		exitSlitPositionEditor_ = new AMExtendedControlEditor(energyControlSet_->exitSlitPosition());

		undulatorTrackingToggleButton_ = new QPushButton("Undulator Tracking");
		undulatorTrackingToggleButton_->setCheckable(true);
		exitSlitTrackingToggleButton_ = new QPushButton("Exit Slit Tracking");
		exitSlitTrackingToggleButton_->setCheckable(true);

		QGridLayout* mainLayout = new QGridLayout();
		mainLayout->addWidget(energyEditor_, 0, 0, 1, 2);

		mainLayout->addWidget(gratingTranslationModeEditor_, 1, 0);
		mainLayout->addWidget(gratingTranslationEditor_, 1, 1);

		mainLayout->addWidget(gratingAngleEditor_, 2, 0, 1, 2);

		mainLayout->addWidget(undulatorPositionEditor_, 3, 0);
		mainLayout->addWidget(undulatorHarmonicEditor_, 3, 1);

		mainLayout->addWidget(exitSlitPositionEditor_, 4, 0, 1, 2);

		QHBoxLayout* buttonLayout = new QHBoxLayout();
		buttonLayout->addWidget(undulatorTrackingToggleButton_);
		buttonLayout->addWidget(exitSlitTrackingToggleButton_);
		mainLayout->addLayout(buttonLayout, 5, 0, 1, 2);

		setLayout(mainLayout);

		connect(undulatorTrackingToggleButton_, SIGNAL(toggled(bool)),
				this, SLOT(onUndulatorTrackingButtonToggled(bool)));
		connect(exitSlitTrackingToggleButton_, SIGNAL(toggled(bool)),
				this, SLOT(onExitSlitTrackingButtonToggled(bool)));

		connect(energyControlSet_->undulatorTracking(), SIGNAL(valueChanged(double)),
				this, SLOT(onEnergyControlUndulatorTrackingChanged(double)));
		connect(energyControlSet_->exitSlitPositionTracking(), SIGNAL(valueChanged(double)),
				this, SLOT(onEnergyControlExitSlitTrackingChanged(double)));

	} else {
		gratingAngleEditor_ = 0;
		undulatorPositionEditor_ = 0;
		exitSlitPositionEditor_ = 0;

		undulatorTrackingToggleButton_ = 0;
		exitSlitTrackingToggleButton_ = 0;

		QVBoxLayout* mainLayout = new QVBoxLayout();
		mainLayout->addWidget(energyEditor_);
		mainLayout->addWidget(gratingTranslationModeEditor_);
		mainLayout->addWidget(gratingTranslationEditor_);
		mainLayout->addWidget(undulatorHarmonicEditor_);

		setLayout(mainLayout);
	}
}



