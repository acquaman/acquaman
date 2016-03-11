#include "SGMEnergyView.h"

#include "ui/beamline/AMExtendedControlEditor.h"
#include "ui/AMValidatorIcon.h"
#include "beamline/SGM/energy/SGMEnergyControlSet.h"
#include "ui/SGM/SGMEnergyTrajectoryView.h"

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


	if(!energyControlSet_->undulatorTracking()->withinTolerance(toggledPVValue)) {

		energyControlSet_->undulatorTracking()->move(toggledPVValue);
	}

	if(isToggled) {
		undulatorTrackingButton_->setText("Undulator Tracking (on)");
	} else {
		undulatorTrackingButton_->setText("Undulator Tracking (off)");
	}
}

void SGMEnergyView::onExitSlitPositionTrackingButtonToggled(bool isToggled)
{
	double toggledPVValue;

	if(isToggled) {

		toggledPVValue = 1.0;
	} else {

		toggledPVValue = 0.0;
	}

	if(!energyControlSet_->exitSlitPositionTracking()->withinTolerance(toggledPVValue)) {

		energyControlSet_->exitSlitPositionTracking()->move(toggledPVValue);
	}

	if(isToggled) {
		exitSlitTrackingButton_->setText("Exit Slit Position Tracking (on)");
	} else {
		exitSlitTrackingButton_->setText("Exit Slit Position Tracking (off)");
	}
}

void SGMEnergyView::onEnergyControlUndulatorTrackingChanged(double)
{
	undulatorTrackingButton_->setChecked(!energyControlSet_->undulatorTracking()->withinTolerance(0.0));
}

void SGMEnergyView::onEnergyControlExitSlitTrackingChanged(double)
{
	exitSlitTrackingButton_->setChecked(!energyControlSet_->exitSlitPositionTracking()->withinTolerance(0.0));
}

void SGMEnergyView::onEnergyControlGratingTrackingOptimizationModeChanged(double)
{
	// Set the grating translation selection control to read only if the optimization
	// mode is not set to manual (0x00).
	gratingTranslationEditor_->setReadOnlyPreference(!energyControlSet_->gratingTranslationOptimization()->withinTolerance(0));
}

void SGMEnergyView::onStopEnergyButtonClicked()
{
	energyControlSet_->energy()->stop();
}

void SGMEnergyView::setupUi(SGMEnergyView::EnergyViewType viewType)
{

	energyEditor_ = new AMExtendedControlEditor(energyControlSet_->energy());
	energyEditor_->setPrecision(6);
	stopEnergyButton_ = new QPushButton("Stop Energy");
	gratingTranslationModeEditor_ = new AMExtendedControlEditor(energyControlSet_->gratingTranslationOptimization());
	gratingTranslationEditor_ = new AMExtendedControlEditor(energyControlSet_->gratingTranslation(), 0, !energyControlSet_->gratingTranslationOptimization()->withinTolerance(0));
	undulatorHarmonicEditor_ = new AMExtendedControlEditor(energyControlSet_->undulatorHarmonic());

	errorValidatorIcon_ = new AMValidatorIcon(energyControlSet_->errorValidator(),
	                                          ":/dialog-error.png");
	warningValidatorIcon_ = new AMValidatorIcon(energyControlSet_->warningValidator(),
	                                            ":/dialog-information.png");


	connect(energyControlSet_->gratingTranslationOptimization(), SIGNAL(valueChanged(double)),
	        this, SLOT(onEnergyControlGratingTrackingOptimizationModeChanged(double)));
	connect(stopEnergyButton_, SIGNAL(clicked()), this, SLOT(onStopEnergyButtonClicked()));

	if(viewType == Advanced) {
		gratingAngleEditor_ = new AMExtendedControlEditor(energyControlSet_->gratingAngle());
		gratingAngleEditor_->setPrecision(12);
		undulatorDetuneOffsetEditor_ = new AMExtendedControlEditor(energyControlSet_->undulatorDetuneOffset());
		undulatorPositionEditor_ = new AMExtendedControlEditor(energyControlSet_->undulatorPosition());
		undulatorPositionEditor_->setPrecision(6);
		exitSlitPositionEditor_ = new AMExtendedControlEditor(energyControlSet_->exitSlitPosition());
		exitSlitPositionEditor_->setPrecision(6);

		undulatorTrackingButton_ = new QPushButton();
		undulatorTrackingButton_->setCheckable(true);
		if(!energyControlSet_->undulatorTracking()->withinTolerance(1)) {
			undulatorTrackingButton_->setText("Undulator Tracking (on)");
			undulatorTrackingButton_->setChecked(true);
		} else {
			undulatorTrackingButton_->setText("Undulator Tracking (off)");
		}

		exitSlitTrackingButton_ = new QPushButton();
		exitSlitTrackingButton_->setCheckable(true);
		if(!energyControlSet_->exitSlitPositionTracking()->withinTolerance(1)) {
			exitSlitTrackingButton_->setText("Exit Slit Position Tracking (on)");
			exitSlitTrackingButton_->setChecked(true);
		} else {
			exitSlitTrackingButton_->setText("Exit Slit Position Tracking (off)");
		}

		energyTrajectoryView_ = new SGMEnergyTrajectoryView(energyControlSet_);

		QGridLayout* mainLayout = new QGridLayout();
		mainLayout->addWidget(energyEditor_, 0, 0, 1, 2);
		mainLayout->addWidget(stopEnergyButton_, 1, 0, 1, 2);

		mainLayout->addWidget(gratingTranslationModeEditor_, 2, 0);
		mainLayout->addWidget(gratingTranslationEditor_, 2, 1);

		mainLayout->addWidget(gratingAngleEditor_, 3, 0, 1, 2);

		mainLayout->addWidget(undulatorPositionEditor_, 4, 0, 1, 2);
		mainLayout->addWidget(undulatorDetuneOffsetEditor_, 5, 0);
		mainLayout->addWidget(undulatorHarmonicEditor_, 5, 1);

		mainLayout->addWidget(exitSlitPositionEditor_, 6, 0, 1, 2);

		QHBoxLayout* buttonLayout = new QHBoxLayout();
		buttonLayout->addWidget(undulatorTrackingButton_);
		buttonLayout->addWidget(exitSlitTrackingButton_);
		mainLayout->addLayout(buttonLayout, 7, 0, 1, 2);

		mainLayout->addWidget(energyTrajectoryView_, 8, 0, 1, 2);

		QHBoxLayout* validatorsLayout = new QHBoxLayout();
		validatorsLayout->addWidget(errorValidatorIcon_);
		validatorsLayout->addWidget(warningValidatorIcon_);
		mainLayout->addLayout(validatorsLayout, 9, 0, 1, 2);

		setLayout(mainLayout);

		connect(undulatorTrackingButton_, SIGNAL(toggled(bool)),
		        this, SLOT(onUndulatorTrackingButtonToggled(bool)));
		connect(exitSlitTrackingButton_, SIGNAL(toggled(bool)),
		        this, SLOT(onExitSlitPositionTrackingButtonToggled(bool)));

		connect(energyControlSet_->undulatorTracking(), SIGNAL(valueChanged(double)),
		        this, SLOT(onEnergyControlUndulatorTrackingChanged(double)));
		connect(energyControlSet_->exitSlitPositionTracking(), SIGNAL(valueChanged(double)),
		        this, SLOT(onEnergyControlExitSlitTrackingChanged(double)));


	} else {

		gratingAngleEditor_ = 0;
		undulatorPositionEditor_ = 0;
		exitSlitPositionEditor_ = 0;

		undulatorTrackingButton_ = 0;
		exitSlitTrackingButton_ = 0;
		energyTrajectoryView_ = 0;

		QVBoxLayout* mainLayout = new QVBoxLayout();
		mainLayout->addWidget(energyEditor_);
		mainLayout->addWidget(stopEnergyButton_);
		mainLayout->addWidget(gratingTranslationModeEditor_);
		mainLayout->addWidget(gratingTranslationEditor_);
		mainLayout->addWidget(undulatorHarmonicEditor_);

		QHBoxLayout* validatorsLayout = new QHBoxLayout();
		validatorsLayout->addWidget(errorValidatorIcon_);
		validatorsLayout->addWidget(warningValidatorIcon_);
		mainLayout->addLayout(validatorsLayout);

		setLayout(mainLayout);
	}
}





