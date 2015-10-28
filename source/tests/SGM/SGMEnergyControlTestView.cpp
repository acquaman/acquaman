#include "SGMEnergyControlTestView.h"
#include "beamline/SGM/energy/SGMEnergyControl.h"

#include <QGridLayout>
#include <QLabel>
#include <QDebug>
SGMEnergyControlTestView::SGMEnergyControlTestView(QWidget *parent) : QWidget(parent)
{
    isInitialized_ = false;
    energyControl_ = new SGMEnergyControl(SGMUndulatorSupport::FirstHarmonic, this);

    setupUi();
    if(energyControl_->isConnected()) {
        initializeOnConnection();
    } else {

        connect(energyControl_, SIGNAL(connected(bool)), this, SLOT(initializeOnConnection()));
    }
}

void SGMEnergyControlTestView::initializeOnConnection()
{
    if(energyControl_->isConnected() && !isInitialized_) {

        isInitialized_ = true;
        setupData();
        setupConnections();
    }
}

void SGMEnergyControlTestView::onUndulatorHarmonicSelectionChanged(int index)
{
    SGMUndulatorSupport::UndulatorHarmonic correspondingHarmonic;

    switch(index) {

    case 0:
        correspondingHarmonic = SGMUndulatorSupport::FirstHarmonic;
        break;
    default:
        correspondingHarmonic = SGMUndulatorSupport::ThirdHarmonic;
    }

    energyControl_->setUndulatorHarmonic(correspondingHarmonic);
}

void SGMEnergyControlTestView::onUndulatorOffsetValueChanged(double value)
{
    energyControl_->setUndulatorOffset(value);

}

void SGMEnergyControlTestView::onUndulatorTrackingCheckBoxChanged(bool isChecked)
{
    energyControl_->setUndulatorTracking(isChecked);
}

void SGMEnergyControlTestView::onGratingOptimizationSelectionChanged(int index)
{
    SGMEnergyPosition::GratingTranslationOptimizationMode correspondingMode;

    switch(index) {

    case 0:
        correspondingMode = SGMEnergyPosition::OptimizeFlux;
        break;
    case 1:
        correspondingMode = SGMEnergyPosition::OptimizeResolution;
        break;
    default:
        correspondingMode = SGMEnergyPosition::ManualMode;
    }

    energyControl_->setGratingTranslationOptimizationMode(correspondingMode);
}

void SGMEnergyControlTestView::onGratingTranslationSelectionChanged(int index)
{
	if(index != -1) {
		double correspondingGratingTranslationStepValue;

		switch(index) {

		case 0:
			correspondingGratingTranslationStepValue = SGMGratingSupport::enumToEncoderCount(SGMGratingSupport::LowGrating);
			break;
		case 1:
			correspondingGratingTranslationStepValue = SGMGratingSupport::enumToEncoderCount(SGMGratingSupport::MediumGrating);
			break;
		default:
			correspondingGratingTranslationStepValue = SGMGratingSupport::enumToEncoderCount(SGMGratingSupport::HighGrating);
		}

		//qDebug() << "Move grating translation control to" << correspondingGratingTranslationStepValue << "steps";
		energyControl_->gratingTranslationControl()->move(correspondingGratingTranslationStepValue);
	}
}

void SGMEnergyControlTestView::onExitSlitTrackingCheckBoxChanged(bool isChecked)
{
    energyControl_->setExitSlitPositionTracking(isChecked);
}

void SGMEnergyControlTestView::onStartTrajectoryButtonPushed()
{
	if(timeTakenSpinBox_->value() > 0) {
		double startEnergy = startEnergySpinBox_->value();
		double endEnergy = endEnergySpinBox_->value();

		double velocity = qAbs(endEnergy - startEnergy) / timeTakenSpinBox_->value();
		energyControl_->move(startEnergy, endEnergy, velocity);
	}
}

void SGMEnergyControlTestView::onControlGratingOptimizationChanged(SGMEnergyPosition::GratingTranslationOptimizationMode optimizationMode)
{

	gratingTranslationComboBox_->setEnabled(optimizationMode == SGMEnergyPosition::ManualMode);
}

void SGMEnergyControlTestView::onControlGratingTranslationChanged(double value)
{
	int correspondingGratingTranslationIndex;

	switch(SGMGratingSupport::encoderCountToEnum(value)) {

	case SGMGratingSupport::LowGrating:
		correspondingGratingTranslationIndex = 0;
		break;
	case SGMGratingSupport::MediumGrating:
		correspondingGratingTranslationIndex = 1;
		break;
	case SGMGratingSupport::HighGrating:
		correspondingGratingTranslationIndex = 2;
		break;
	default:
		correspondingGratingTranslationIndex = -1;
	}

	gratingTranslationComboBox_->setCurrentIndex(correspondingGratingTranslationIndex);
}

void SGMEnergyControlTestView::setupUi()
{
    QGridLayout* mainLayout = new QGridLayout();
    setLayout(mainLayout);

    energyControlEditor_ = new AMExtendedControlEditor(energyControl_);

    mainLayout->addWidget(energyControlEditor_, 0, 0, 1, 2);

    undulatorHarmonic_ = new QComboBox();
    undulatorHarmonic_->addItem("First");
    undulatorHarmonic_->addItem("Third");
    mainLayout->addWidget(new QLabel("Undulator Harmonic"), 1, 0);
    mainLayout->addWidget(undulatorHarmonic_, 1, 1);

    undulatorOffset_ = new QDoubleSpinBox();
    mainLayout->addWidget(new QLabel("Undulator Offset"),2, 0);
    mainLayout->addWidget(undulatorOffset_, 2, 1);

	undulatorTrackingCheckBox_ = new QCheckBox("Undulator Tracking");
	mainLayout->addWidget(undulatorTrackingCheckBox_, 3, 0,1,2);

    gratingOptimizationComboBox_= new QComboBox();
    gratingOptimizationComboBox_->addItem("Optimize Flux");
    gratingOptimizationComboBox_->addItem("Optimize Resolution");
    gratingOptimizationComboBox_->addItem("Manual");
    gratingOptimizationComboBox_->setCurrentIndex(2);
    mainLayout->addWidget(new QLabel("Grating Selection Mode"), 4, 0);
    mainLayout->addWidget(gratingOptimizationComboBox_, 4, 1);

	gratingTranslationComboBox_ = new QComboBox();
	gratingTranslationComboBox_->addItem("Low");
	gratingTranslationComboBox_->addItem("Medium");
	gratingTranslationComboBox_->addItem("High");
	mainLayout->addWidget(new QLabel("Grating Translation"), 5, 0);
	mainLayout->addWidget(gratingTranslationComboBox_, 5, 1);

    exitSlitTrackingCheckBox_ = new QCheckBox("Exit Slit Tracking");
	mainLayout->addWidget(exitSlitTrackingCheckBox_,6,0,1,2);

    gratingAngleValueControlEditor_ = new AMExtendedControlEditor(0, 0, true);
    mainLayout->addWidget(gratingAngleValueControlEditor_, 7, 0, 1, 2);

    gratingTranslationControlEditor_ = new AMExtendedControlEditor(0, 0, true);
    mainLayout->addWidget(gratingTranslationControlEditor_, 8, 0, 1, 2);

    undulatorValueControlEditor_ = new AMExtendedControlEditor(0, 0, true);
    mainLayout->addWidget(undulatorValueControlEditor_, 9, 0, 1, 2);

    exitSlitValueControlEditor_ = new AMExtendedControlEditor(0, 0, true);
    mainLayout->addWidget(exitSlitValueControlEditor_, 10, 0, 1, 2);

	startEnergySpinBox_ = new QDoubleSpinBox();
	startEnergySpinBox_->setRange(0, 2500);
	mainLayout->addWidget(new QLabel("Start Energy"), 11, 0);
	mainLayout->addWidget(startEnergySpinBox_, 11, 1);

	endEnergySpinBox_ = new QDoubleSpinBox();
	endEnergySpinBox_->setRange(0, 2500);
	mainLayout->addWidget(new QLabel("End Energy"), 12, 0);
	mainLayout->addWidget(endEnergySpinBox_, 12, 1);

	timeTakenSpinBox_ = new QDoubleSpinBox();
	timeTakenSpinBox_->setRange(0, 120);
	mainLayout->addWidget(new QLabel("Time"), 13, 0);
	mainLayout->addWidget(timeTakenSpinBox_, 13, 1);

	startTrajectoryButton_ = new QPushButton("Start");
	mainLayout->addWidget(startTrajectoryButton_, 14, 0, 1, 2);



    setLayout(mainLayout);
}

void SGMEnergyControlTestView::setupData()
{
    int correspondingHarmonicComboIndex;
    switch(energyControl_->undulatorHarmonic()) {

    case SGMUndulatorSupport::FirstHarmonic:
        correspondingHarmonicComboIndex = 0;
        break;
    default:
        correspondingHarmonicComboIndex = 1;
    }

    undulatorHarmonic_->setCurrentIndex(correspondingHarmonicComboIndex);

    undulatorOffset_->setRange(DBL_MIN, DBL_MAX);
    undulatorOffset_->setValue(energyControl_->undulatorOffset());

    undulatorTrackingCheckBox_->setChecked(energyControl_->isUndulatorTracking());

    int correspondingOptimizationComboIndex;
    switch(energyControl_->gratingTranslationOptimizationMode()) {
    case SGMEnergyPosition::OptimizeFlux:
        correspondingOptimizationComboIndex = 0;
        break;
    case SGMEnergyPosition::OptimizeResolution:
        correspondingOptimizationComboIndex = 1;
        break;
    default:
        correspondingOptimizationComboIndex = 2;
    }

    gratingOptimizationComboBox_->setCurrentIndex(correspondingOptimizationComboIndex);

	int correspondingTranslationComboIndex;
    switch(SGMGratingSupport::encoderCountToEnum(energyControl_->gratingTranslationControl()->value())) {
	case SGMGratingSupport::LowGrating:
		correspondingTranslationComboIndex = 0;
		break;
	case SGMGratingSupport::MediumGrating:
		correspondingTranslationComboIndex = 1;
		break;
	default:
		correspondingTranslationComboIndex = 2;
	}

	gratingTranslationComboBox_->setCurrentIndex(correspondingTranslationComboIndex);
	gratingTranslationComboBox_->setEnabled(energyControl_->gratingTranslationOptimizationMode() == SGMEnergyPosition::ManualMode);

    exitSlitTrackingCheckBox_->setChecked(energyControl_->isExitSlitPositionTracking());

    gratingAngleValueControlEditor_->setControl(energyControl_->gratingAngleControl());
	gratingAngleValueControlEditor_->setPrecision(12);
    gratingTranslationControlEditor_->setControl(energyControl_->gratingTranslationControl());
	gratingTranslationControlEditor_->setPrecision(12);
    exitSlitValueControlEditor_->setControl(energyControl_->exitSlitPositionControl());
	exitSlitValueControlEditor_->setPrecision(6);
    undulatorValueControlEditor_->setControl(energyControl_->undulatorPositionControl());
	undulatorValueControlEditor_->setPrecision(6);

}

void SGMEnergyControlTestView::setupConnections()
{
    connect(energyControl_, SIGNAL(gratingTranslationOptimizationModeChanged(SGMEnergyPosition::GratingTranslationOptimizationMode)),
            this, SLOT(onControlGratingOptimizationChanged(SGMEnergyPosition::GratingTranslationOptimizationMode)));
	connect(energyControl_->gratingTranslationControl(), SIGNAL(valueChanged(double)),
			this, SLOT(onControlGratingTranslationChanged(double)));


	connect(startTrajectoryButton_, SIGNAL(clicked()), this, SLOT(onStartTrajectoryButtonPushed()));
	connect(undulatorHarmonic_, SIGNAL(currentIndexChanged(int)),
			this, SLOT(onUndulatorHarmonicSelectionChanged(int)));
	connect(undulatorOffset_, SIGNAL(valueChanged(double)),
			this, SLOT(onUndulatorOffsetValueChanged(double)));
	connect(undulatorTrackingCheckBox_, SIGNAL(toggled(bool)),
			this, SLOT(onUndulatorTrackingCheckBoxChanged(bool)));
	connect(gratingOptimizationComboBox_, SIGNAL(currentIndexChanged(int)),
			this, SLOT(onGratingOptimizationSelectionChanged(int)));
	connect(gratingTranslationComboBox_, SIGNAL(currentIndexChanged(int)),
			this, SLOT(onGratingTranslationSelectionChanged(int)));
}

