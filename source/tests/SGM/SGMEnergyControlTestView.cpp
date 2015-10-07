#include "SGMEnergyControlTestView.h"
#include "beamline/SGM/monochromator/SGMEnergyControl.h"

#include <QGridLayout>
#include <QLabel>
SGMEnergyControlTestView::SGMEnergyControlTestView(QWidget *parent) : QWidget(parent)
{
    isInitialized_ = false;
    energyControl_ = new SGMEnergyControl(SGMUndulatorSupport::FirstHarmonic,this);

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

void SGMEnergyControlTestView::onExitSlitTRackingCheckBoxChanged(bool isChecked)
{
    energyControl_->setExitSlitPositionTracking(isChecked);
}

void SGMEnergyControlTestView::setupUi()
{
    QGridLayout* mainLayout = new QGridLayout();
    setLayout(mainLayout);

    energyControlEditor_ = new AMExtendedControlEditor(energyControl_);
    mainLayout->addWidget(new QLabel("Energy"), 0, 0);
    mainLayout->addWidget(energyControlEditor_, 0, 1);

    undulatorHarmonic_ = new QComboBox();
    undulatorHarmonic_->addItem("First");
    undulatorHarmonic_->addItem("Third");
    mainLayout->addWidget(new QLabel("Undulator Harmonic"), 1, 0);
    mainLayout->addWidget(undulatorHarmonic_, 1, 1);

    undulatorOffset_ = new QDoubleSpinBox();
    mainLayout->addWidget(new QLabel("Undulator Offset"),2, 0);
    mainLayout->addWidget(undulatorOffset_, 2, 1);

    undulatorTrackingCheckBox_ = new QCheckBox();
    mainLayout->addWidget(new QLabel("Undulator Tracking"), 3, 0);
    mainLayout->addWidget(undulatorTrackingCheckBox_, 3, 1);

    gratingOptimizationComboBox_= new QComboBox();
    gratingOptimizationComboBox_->addItem("Optimize Flux");
    gratingOptimizationComboBox_->addItem("Optimize Resolution");
    gratingOptimizationComboBox_->addItem("Manual");
    gratingOptimizationComboBox_->setCurrentIndex(2);
    mainLayout->addWidget(new QLabel("Grating Selection Mode"), 4, 0);
    mainLayout->addWidget(gratingOptimizationComboBox_, 4, 1);

    exitSlitTrackingCheckBox_ = new QCheckBox("Exit Slit Tracking");
    mainLayout->addWidget(exitSlitTrackingCheckBox_,5,0,1,2);

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

    exitSlitTrackingCheckBox_->setChecked(energyControl_->isExitSlitPositionTracking());
}

void SGMEnergyControlTestView::setupConnections()
{

}

