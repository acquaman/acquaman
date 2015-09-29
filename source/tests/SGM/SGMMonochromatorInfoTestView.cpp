#include "SGMMonochromatorInfoTestView.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QGroupBox>

#include "beamline/SGM/monochromator/SGMMonochromatorInfo.h"

SGMMonochromatorInfoTestView::SGMMonochromatorInfoTestView(SGMMonochromatorInfo *mono, QWidget *parent) : QWidget(parent)
{
    sgmMonochromatorInfo_ = mono;
    setupUi();
    setupData();
    makeConnections();
}

void SGMMonochromatorInfoTestView::onMonoGratingAngleChanged(double gratingAngleSteps)
{
    gratingAngleSpinBox_->setValue(gratingAngleSteps);
}

void SGMMonochromatorInfoTestView::onMonoGratingTranslationChanged(SGMGratingSupport::GratingTranslation gratingTranslation)
{
    int gratingInteger = int(gratingTranslation);

    gratingTranslationComboBox_->setCurrentIndex(gratingInteger);
}

void SGMMonochromatorInfoTestView::onMonoUndulatorTrackingChanged(bool isTracking)
{
    undulatorTrackingCheckBox_->setChecked(isTracking);
}

void SGMMonochromatorInfoTestView::onMonoUndulatorPositionChanged(double undulatorPosition)
{
    undulatorPositionSpinBox_->setValue(undulatorPosition);
}

void SGMMonochromatorInfoTestView::onMonoUndulatorOffsetChanged(double undulatorOffset)
{
    undulatorDetuneOffsetSpinBox_->setValue(undulatorOffset);
}

void SGMMonochromatorInfoTestView::onMonoUndulatorHarmonicChanged(SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic)
{
    switch(undulatorHarmonic) {
    case SGMUndulatorSupport::FirstHarmonic:
        undulatorHarmonicComboBox_->setCurrentIndex(0);
        break;
    case SGMUndulatorSupport::ThirdHarmonic:
        undulatorHarmonicComboBox_->setCurrentIndex(1);
        break;
    case SGMUndulatorSupport::UnknownUndulatorHarmonic:
        break;
    }
}

void SGMMonochromatorInfoTestView::onMonoExitSlitTrackingChanged(bool isTracking)
{
    exitSlitTrackingCheckBox_->setChecked(isTracking);
}

void SGMMonochromatorInfoTestView::onMonoExitSlitPositionChanged(double exitSlitPosition)
{
    exitSlitPositionSpinBox_->setValue(exitSlitPosition);
}

void SGMMonochromatorInfoTestView::onMonoEnergyChanged(double energy)
{
    energySpinBox_->setValue(energy);
}

void SGMMonochromatorInfoTestView::onMonoWarningCountChanged(int warningCount)
{
    if(warningCount == 0) {
        warningsTextEdit_->setVisible(false);
    } else {

        warningsTextEdit_->setVisible(true);
        QString warningString;

        foreach(QString message, sgmMonochromatorInfo_->warningMessage()) {

            warningString.append(QString("\n%1").arg(message));
        }

        warningsTextEdit_->setText(warningString);
    }

}

void SGMMonochromatorInfoTestView::onMonoErrorCountChanged(int errorCount)
{
    if(errorCount == 0) {
        errorsTextEdit_->setVisible(false);
            } else {
        errorsTextEdit_->setVisible(true);
        QString errorString;

        foreach(QString message, sgmMonochromatorInfo_->errorMessage()) {

            errorString.append(QString("\n%1").arg(message));
        }

        errorsTextEdit_->setText(errorString);
    }
}

void SGMMonochromatorInfoTestView::onGratingAngleSpinBoxChanged()
{
    double value = gratingAngleSpinBox_->value();
    if(qAbs(value - sgmMonochromatorInfo_->gratingAngle()) > 0.001) {
        sgmMonochromatorInfo_->setGratingAngle(value);
    }
}

void SGMMonochromatorInfoTestView::onGratingSelectionModeComboBoxChanged(int index)
{
    gratingTranslationComboBox_->setEnabled(index == 3);
    if(index != 3) {

        SGMMonochromatorInfo::GratingTranslationOptimizationMode correspondingOptimizeMode;
        switch(index) {
        case 0:
            correspondingOptimizeMode = SGMMonochromatorInfo::OptimizeFlux;
            break;
        case 1:
            correspondingOptimizeMode = SGMMonochromatorInfo::OptimizeResolution;
            break;
        case 2:
        default:
            correspondingOptimizeMode = SGMMonochromatorInfo::OptimizeMinimalMovement;
            break;
        }


        sgmMonochromatorInfo_->requestEnergy(this->energySpinBox_->value(), correspondingOptimizeMode);
    }
}

void SGMMonochromatorInfoTestView::onGratingTranslationComboBoxChanged(int index)
{
    SGMGratingSupport::GratingTranslation correspondingGrating;
    switch(index) {
    case 0:
        correspondingGrating = SGMGratingSupport::LowGrating;
        break;
    case 1:
        correspondingGrating = SGMGratingSupport::MediumGrating;
        break;
    case 2:
        correspondingGrating = SGMGratingSupport::HighGrating;
        break;
    default:
        correspondingGrating = SGMGratingSupport::UnknownGrating;
    }

    if(correspondingGrating != SGMGratingSupport::UnknownGrating &&
            correspondingGrating != sgmMonochromatorInfo_->gratingTranslation()) {

        sgmMonochromatorInfo_->setGratingTranslation(correspondingGrating);
    }
}

void SGMMonochromatorInfoTestView::onUndulatorTrackingCheckBoxChanged(bool value)
{
    if(sgmMonochromatorInfo_->isUndulatorTracking() != value) {
        sgmMonochromatorInfo_->setUndulatorTracking(value);
    }
}

void SGMMonochromatorInfoTestView::onUndulatorPositionSpinBoxChanged()
{
    double value = undulatorPositionSpinBox_->value();
    if(qAbs(sgmMonochromatorInfo_->undulatorPosition() - value) > 0.001) {

        sgmMonochromatorInfo_->setUndulatorPosition(value);
    }
}

void SGMMonochromatorInfoTestView::onUndulatorOffsetSpinBoxChanged()
{
    double value = undulatorDetuneOffsetSpinBox_->value();
    if(qAbs(sgmMonochromatorInfo_->undulatorOffset() - value) > 0.001) {
                sgmMonochromatorInfo_->setUndulatorOffset(value);
    }
}

void SGMMonochromatorInfoTestView::onAutoDetectUndulatorHarmonicCheckBoxChanged(bool value)
{
    sgmMonochromatorInfo_->setAutoDetectUndulatorHarmonic(value);
    undulatorHarmonicComboBox_->setEnabled(!value);
}

void SGMMonochromatorInfoTestView::onUndulatorHarmonicComboBoxChanged(int index)
{
    SGMUndulatorSupport::UndulatorHarmonic correspondingUndulatorHarmonic;

    switch(index) {

    case 0:
        correspondingUndulatorHarmonic = SGMUndulatorSupport::FirstHarmonic;
        break;
    case 1:
        correspondingUndulatorHarmonic = SGMUndulatorSupport::ThirdHarmonic;
        break;
    default:
        correspondingUndulatorHarmonic = SGMUndulatorSupport::UnknownUndulatorHarmonic;
    }

    if(correspondingUndulatorHarmonic != SGMUndulatorSupport::UnknownUndulatorHarmonic &&
            correspondingUndulatorHarmonic != sgmMonochromatorInfo_->undulatorHarmonic()) {

        sgmMonochromatorInfo_->setUndulatorHarmonic(correspondingUndulatorHarmonic);
    }
}

void SGMMonochromatorInfoTestView::onExitSlitTrackingCheckBoxChanged(bool value)
{
    if(sgmMonochromatorInfo_->isExitSlitPositionTracking() != value) {

        sgmMonochromatorInfo_->setExitSlitPositionTracking(value);
    }
}

void SGMMonochromatorInfoTestView::onExitSlitPositionSpinBoxChanged()
{
    double value = exitSlitPositionSpinBox_->value();
    if(qAbs(sgmMonochromatorInfo_->exitSlitPosition() - value ) > 0.001) {

        sgmMonochromatorInfo_->setExitSlitPosition(value);
    }
}

void SGMMonochromatorInfoTestView::onEnergySpinBoxChanged()
{
    double value = energySpinBox_->value();
    if(qAbs(sgmMonochromatorInfo_->resultantEnergy() - value) > 0.001) {

        if(gratingSelectionModeComboBox_->currentIndex() == 3) {
            //Manual selection mode

            SGMGratingSupport::GratingTranslation correspondingGrating;
            switch(gratingTranslationComboBox_->currentIndex()) {
            case 0:
                correspondingGrating = SGMGratingSupport::LowGrating;
                break;
            case 1:
                correspondingGrating = SGMGratingSupport::MediumGrating;
                break;
            case 2:
                correspondingGrating = SGMGratingSupport::HighGrating;
                break;
            default:
                correspondingGrating = SGMGratingSupport::UnknownGrating;
            }

            sgmMonochromatorInfo_->requestEnergy(value, correspondingGrating);

        } else {
            // Auto selection mode

            SGMMonochromatorInfo::GratingTranslationOptimizationMode optimizationMode;
            switch(gratingSelectionModeComboBox_->currentIndex()) {

            case 0:
                optimizationMode = SGMMonochromatorInfo::OptimizeFlux;
                break;
            case 1:
                optimizationMode = SGMMonochromatorInfo::OptimizeResolution;
                break;
            case 2:
            default:
                optimizationMode = SGMMonochromatorInfo::OptimizeMinimalMovement;
            }

            sgmMonochromatorInfo_->requestEnergy(value, optimizationMode);
        }

    }
}

void SGMMonochromatorInfoTestView::setupUi()
{
    QVBoxLayout* mainLayout = new QVBoxLayout();

    QGroupBox* gratingGroupBox = new QGroupBox("Grating");
    QGroupBox* undulatorGroupBox = new QGroupBox("Undulator");
    QGroupBox* exitSlitGroupBox = new QGroupBox("Exit Slit");

    QGridLayout* gratingLayout = new QGridLayout();
    QGridLayout* undulatorLayout = new QGridLayout();
    QGridLayout* exitSlitLayout = new QGridLayout();

    gratingGroupBox->setLayout(gratingLayout);
    undulatorGroupBox->setLayout(undulatorLayout);
    exitSlitGroupBox->setLayout(exitSlitLayout);

    energySpinBox_  = new QDoubleSpinBox();
    energySpinBox_->setRange(0, 10000);
    energySpinBox_->setSuffix("eV");

    mainLayout->addWidget(energySpinBox_);
    mainLayout->addWidget(gratingGroupBox);
    mainLayout->addWidget(undulatorGroupBox);
    mainLayout->addWidget(exitSlitGroupBox);

    gratingSelectionModeComboBox_ = new QComboBox();
    gratingSelectionModeComboBox_->addItem("Optimize Flux");
    gratingSelectionModeComboBox_->addItem("Optimize Resolution");
    gratingSelectionModeComboBox_->addItem("Minimize Movement");
    gratingSelectionModeComboBox_->addItem("Manual");
    gratingSelectionModeComboBox_->setCurrentIndex(2);
    gratingLayout->addWidget(new QLabel("Grating Selection Mode"), 0, 0);
    gratingLayout->addWidget(gratingSelectionModeComboBox_, 0, 1);


    gratingTranslationComboBox_ = new QComboBox();
    gratingTranslationComboBox_->addItem("Low");
    gratingTranslationComboBox_->addItem("Medium");
    gratingTranslationComboBox_->addItem("High");
    gratingTranslationComboBox_->setEnabled(false);
    gratingLayout->addWidget(new QLabel("Grating Selection"), 1, 0);
    gratingLayout->addWidget(gratingTranslationComboBox_, 1, 1);

    gratingAngleSpinBox_ = new QDoubleSpinBox();
    gratingAngleSpinBox_->setSuffix(" steps");
    gratingAngleSpinBox_->setRange(-999999999, 999999999);
    gratingLayout->addWidget(new QLabel("Grating Angle"), 2, 0);
    gratingLayout->addWidget(gratingAngleSpinBox_, 2, 1);

    undulatorPositionSpinBox_ = new QDoubleSpinBox();
    undulatorPositionSpinBox_->setSuffix(" mm");
    undulatorPositionSpinBox_->setRange(0, 500);
    undulatorLayout->addWidget(new QLabel("Undulator Position"), 0, 0);
    undulatorLayout->addWidget(undulatorPositionSpinBox_, 0, 1);

    undulatorDetuneOffsetSpinBox_ = new QDoubleSpinBox();
    undulatorDetuneOffsetSpinBox_->setSuffix(" mm");
    undulatorDetuneOffsetSpinBox_->setRange(-1000, 1000);
    undulatorLayout->addWidget(new QLabel("Undulator Detune Offset"), 1, 0);
    undulatorLayout->addWidget(undulatorDetuneOffsetSpinBox_, 1, 1);

    undulatorTrackingCheckBox_ = new QCheckBox("Undulator Tracking");
    undulatorLayout->addWidget(undulatorTrackingCheckBox_, 2, 0);

    autoDetectUndulatorHarmonicCheckBox_ = new QCheckBox("Auto-detect Undulator Harmonic");
    undulatorLayout->addWidget(autoDetectUndulatorHarmonicCheckBox_, 3,0);

    undulatorHarmonicComboBox_ = new QComboBox();
    undulatorHarmonicComboBox_->addItem("First Harmonic");
    undulatorHarmonicComboBox_->addItem("Third Harmonic");
    undulatorLayout->addWidget(new QLabel("Undulator Harmonic"), 4, 0);
    undulatorLayout->addWidget(undulatorHarmonicComboBox_);

    exitSlitPositionSpinBox_ = new QDoubleSpinBox();
    exitSlitPositionSpinBox_->setSuffix(" ??");
    exitSlitPositionSpinBox_->setRange(-1000000, 1000000);
    exitSlitLayout->addWidget(new QLabel("Exit Slit Position"), 0, 0);
    exitSlitLayout->addWidget(exitSlitPositionSpinBox_,0, 1);

    exitSlitTrackingCheckBox_ = new QCheckBox("Exit Slit Tracking");
    exitSlitLayout->addWidget(exitSlitTrackingCheckBox_, 1, 0);

    warningsTextEdit_ = new QTextEdit();
    mainLayout->addWidget(warningsTextEdit_);

    errorsTextEdit_ = new QTextEdit();
    mainLayout->addWidget(errorsTextEdit_);


    setLayout(mainLayout);

}

void SGMMonochromatorInfoTestView::makeConnections()
{
    // Mono to View
    connect(sgmMonochromatorInfo_, SIGNAL(gratingAngleChanged(double)), this, SLOT(onMonoGratingAngleChanged(double)));
    connect(sgmMonochromatorInfo_, SIGNAL(gratingTranslationChanged(SGMGratingSupport::GratingTranslation)),
            this, SLOT(onMonoGratingTranslationChanged(SGMGratingSupport::GratingTranslation)));

    connect(sgmMonochromatorInfo_, SIGNAL(undulatorTrackingChanged(bool)), this, SLOT(onMonoUndulatorTrackingChanged(bool)));
    connect(sgmMonochromatorInfo_, SIGNAL(undulatorPositionChanged(double)), this, SLOT(onMonoUndulatorPositionChanged(double)));
    connect(sgmMonochromatorInfo_, SIGNAL(undulatorOffsetChanged(double)), this, SLOT(onMonoUndulatorOffsetChanged(double)));
    connect(sgmMonochromatorInfo_, SIGNAL(undulatorHarmonicChanged(SGMUndulatorSupport::UndulatorHarmonic)),
            this, SLOT(onMonoUndulatorHarmonicChanged(SGMUndulatorSupport::UndulatorHarmonic)));

    connect(sgmMonochromatorInfo_, SIGNAL(exitSlitTrackingChanged(bool)), this, SLOT(onMonoExitSlitTrackingChanged(bool)));
    connect(sgmMonochromatorInfo_, SIGNAL(exitSlitPositionChanged(double)), this, SLOT(onMonoExitSlitPositionChanged(double)));
    connect(sgmMonochromatorInfo_, SIGNAL(energyChanged(double)), this, SLOT(onMonoEnergyChanged(double)));
    connect(sgmMonochromatorInfo_, SIGNAL(errorCountChanged(int)), this, SLOT(onMonoErrorCountChanged(int)));
    connect(sgmMonochromatorInfo_, SIGNAL(warningCountChanged(int)), this, SLOT(onMonoWarningCountChanged(int)));

    // View to Mono
    connect(gratingAngleSpinBox_, SIGNAL(editingFinished()), this, SLOT(onGratingAngleSpinBoxChanged()));
    connect(gratingSelectionModeComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onGratingSelectionModeComboBoxChanged(int)));
    connect(gratingTranslationComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onGratingTranslationComboBoxChanged(int)));
    connect(undulatorTrackingCheckBox_, SIGNAL(toggled(bool)), this, SLOT(onUndulatorTrackingCheckBoxChanged(bool)));
    connect(undulatorPositionSpinBox_, SIGNAL(editingFinished()), this, SLOT(onUndulatorPositionSpinBoxChanged()));
    connect(undulatorDetuneOffsetSpinBox_, SIGNAL(editingFinished()), this, SLOT(onUndulatorOffsetSpinBoxChanged()));
    connect(undulatorHarmonicComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onUndulatorHarmonicComboBoxChanged(int)));
    connect(autoDetectUndulatorHarmonicCheckBox_, SIGNAL(toggled(bool)), this, SLOT(onAutoDetectUndulatorHarmonicCheckBoxChanged(bool)));
    connect(exitSlitTrackingCheckBox_, SIGNAL(toggled(bool)), this, SLOT(onExitSlitTrackingCheckBoxChanged(bool)));
    connect(exitSlitPositionSpinBox_, SIGNAL(editingFinished()), this, SLOT(onExitSlitPositionSpinBoxChanged()));
    connect(energySpinBox_, SIGNAL(editingFinished()), this, SLOT(onEnergySpinBoxChanged()));
}

void SGMMonochromatorInfoTestView::setupData()
{
    onMonoGratingAngleChanged(sgmMonochromatorInfo_->gratingAngle());
    onMonoGratingTranslationChanged(sgmMonochromatorInfo_->gratingTranslation());
    onMonoUndulatorTrackingChanged(sgmMonochromatorInfo_->isUndulatorTracking());
    onMonoUndulatorPositionChanged(sgmMonochromatorInfo_->undulatorPosition());
    onMonoUndulatorOffsetChanged(sgmMonochromatorInfo_->undulatorOffset());
    onMonoUndulatorHarmonicChanged(sgmMonochromatorInfo_->undulatorHarmonic());
    onMonoExitSlitTrackingChanged(sgmMonochromatorInfo_->isExitSlitPositionTracking());
    onMonoExitSlitPositionChanged(sgmMonochromatorInfo_->exitSlitPosition());
    onMonoEnergyChanged(sgmMonochromatorInfo_->resultantEnergy());
    onMonoWarningCountChanged(sgmMonochromatorInfo_->warningMessage().count());
    onMonoErrorCountChanged(sgmMonochromatorInfo_->errorMessage().count());
    autoDetectUndulatorHarmonicCheckBox_->setChecked(sgmMonochromatorInfo_->autoDetectUndulatorHarmonic());
    undulatorHarmonicComboBox_->setEnabled(!sgmMonochromatorInfo_->autoDetectUndulatorHarmonic());
}
