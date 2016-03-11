#include "SGMEnergyPositionTestView.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QGroupBox>

#include "beamline/SGM/energy/SGMEnergyPosition.h"

SGMEnergyPositionTestView::SGMEnergyPositionTestView(SGMEnergyPosition *energyPosition, QWidget *parent) : QWidget(parent)
{
	energyPosition_ = energyPosition;
	setupUi();
	setupData();
	makeConnections();
}

void SGMEnergyPositionTestView::onEnergyPositionGratingAngleChanged(double gratingAngleSteps)
{
	gratingAngleSpinBox_->setValue(gratingAngleSteps);
}

void SGMEnergyPositionTestView::onEnergyPositionGratingTranslationChanged(SGMGratingSupport::GratingTranslation gratingTranslation)
{
	int gratingInteger = int(gratingTranslation);

	gratingTranslationComboBox_->setCurrentIndex(gratingInteger);
}

void SGMEnergyPositionTestView::onEnergyPositionUndulatorTrackingChanged(bool isTracking)
{
	undulatorTrackingCheckBox_->setChecked(isTracking);
}

void SGMEnergyPositionTestView::onEnergyUndulatorPositionChanged(double undulatorPosition)
{
	undulatorPositionSpinBox_->setValue(undulatorPosition);
}

void SGMEnergyPositionTestView::onEnergyPositionUndulatorOffsetChanged(double undulatorOffset)
{
	undulatorDetuneOffsetSpinBox_->setValue(undulatorOffset);
}

void SGMEnergyPositionTestView::onEnergyPositionUndulatorHarmonicChanged(SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic)
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

void SGMEnergyPositionTestView::onEnergyPositionExitSlitTrackingChanged(bool isTracking)
{
	exitSlitTrackingCheckBox_->setChecked(isTracking);
}

void SGMEnergyPositionTestView::onEnergyPositionExitSlitPositionChanged(double exitSlitPosition)
{
	exitSlitPositionSpinBox_->setValue(exitSlitPosition);
}

void SGMEnergyPositionTestView::onEnergyPositionEnergyChanged(double energy)
{
	energySpinBox_->setValue(energy);
}

void SGMEnergyPositionTestView::onEnergyPositionWarningCountChanged(int warningCount)
{
	if(warningCount == 0) {
		warningsTextEdit_->setVisible(false);
	} else {

		warningsTextEdit_->setVisible(true);
		QString warningString = energyPosition_->warningValidator()->fullFailureMessage();

		warningsTextEdit_->setText(warningString);
	}

}

void SGMEnergyPositionTestView::onEnergyPositionErrorCountChanged(int errorCount)
{
	if(errorCount == 0) {
		errorsTextEdit_->setVisible(false);
	} else {

		errorsTextEdit_->setVisible(true);
		QString errorString = energyPosition_->errorValidator()->fullFailureMessage();

		errorsTextEdit_->setText(errorString);
	}
}

void SGMEnergyPositionTestView::onGratingAngleSpinBoxChanged()
{
	double value = gratingAngleSpinBox_->value();
	if(qAbs(value - energyPosition_->gratingAngle()) > 0.001) {
		energyPosition_->setGratingAngle(value);
	}
}

void SGMEnergyPositionTestView::onGratingSelectionModeComboBoxChanged(int index)
{
	gratingTranslationComboBox_->setEnabled(index == 2);

	SGMGratingSupport::GratingTranslationOptimizationMode correspondingOptimizeMode;
	switch(index) {
	case 0:
		correspondingOptimizeMode = SGMGratingSupport::OptimizeFlux;
		break;
	case 1:
		correspondingOptimizeMode = SGMGratingSupport::OptimizeResolution;
		break;
	default:
		correspondingOptimizeMode = SGMGratingSupport::ManualMode;
		break;
	}

	energyPosition_->setGratingTranslationOptimizationMode(correspondingOptimizeMode);

}

void SGMEnergyPositionTestView::onGratingTranslationComboBoxChanged(int index)
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
	        correspondingGrating != energyPosition_->gratingTranslation()) {

		energyPosition_->setGratingTranslation(correspondingGrating);
	}
}

void SGMEnergyPositionTestView::onUndulatorTrackingCheckBoxChanged(bool value)
{
	if(energyPosition_->isUndulatorTracking() != value) {
		energyPosition_->setUndulatorTracking(value);
	}
}

void SGMEnergyPositionTestView::onUndulatorPositionSpinBoxChanged()
{
	double value = undulatorPositionSpinBox_->value();
	if(qAbs(energyPosition_->undulatorPosition() - value) > 0.001) {

		energyPosition_->setUndulatorPosition(value);
	}
}

void SGMEnergyPositionTestView::onUndulatorOffsetSpinBoxChanged()
{
	double value = undulatorDetuneOffsetSpinBox_->value();
	if(qAbs(energyPosition_->undulatorOffset() - value) > 0.001) {
		energyPosition_->setUndulatorOffset(value);
	}
}

void SGMEnergyPositionTestView::onAutoDetectUndulatorHarmonicCheckBoxChanged(bool value)
{
	energyPosition_->setAutoDetectUndulatorHarmonic(value);
	undulatorHarmonicComboBox_->setEnabled(!value);
}

void SGMEnergyPositionTestView::onUndulatorHarmonicComboBoxChanged(int index)
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
	        correspondingUndulatorHarmonic != energyPosition_->undulatorHarmonic()) {

		energyPosition_->setUndulatorHarmonic(correspondingUndulatorHarmonic);
	}
}

void SGMEnergyPositionTestView::onExitSlitTrackingCheckBoxChanged(bool value)
{
	if(energyPosition_->isExitSlitPositionTracking() != value) {

		energyPosition_->setExitSlitPositionTracking(value);
	}
}

void SGMEnergyPositionTestView::onExitSlitPositionSpinBoxChanged()
{
	double value = exitSlitPositionSpinBox_->value();
	if(qAbs(energyPosition_->exitSlitPosition() - value ) > 0.001) {

		energyPosition_->setExitSlitPosition(value);
	}
}

void SGMEnergyPositionTestView::onEnergySpinBoxChanged()
{
	double value = energySpinBox_->value();
	if(qAbs(energyPosition_->resultantEnergy() - value) > 0.001) {

		if(gratingSelectionModeComboBox_->currentIndex() == 2) {
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

			energyPosition_->requestEnergy(value, correspondingGrating);

		} else {
			// Auto selection mode

			energyPosition_->requestEnergy(value);
		}

	}
}

void SGMEnergyPositionTestView::setupUi()
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
	gratingSelectionModeComboBox_->addItem("Manual");
	gratingSelectionModeComboBox_->setCurrentIndex(2);
	gratingLayout->addWidget(new QLabel("Grating Selection Mode"), 0, 0);
	gratingLayout->addWidget(gratingSelectionModeComboBox_, 0, 1);


	gratingTranslationComboBox_ = new QComboBox();
	gratingTranslationComboBox_->addItem("Low");
	gratingTranslationComboBox_->addItem("Medium");
	gratingTranslationComboBox_->addItem("High");
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

void SGMEnergyPositionTestView::makeConnections()
{
	// Mono to View
	connect(energyPosition_, SIGNAL(gratingAngleChanged(double)), this, SLOT(onEnergyPositionGratingAngleChanged(double)));
	connect(energyPosition_, SIGNAL(gratingTranslationChanged(SGMGratingSupport::GratingTranslation)),
	        this, SLOT(onEnergyPositionGratingTranslationChanged(SGMGratingSupport::GratingTranslation)));

	connect(energyPosition_, SIGNAL(undulatorTrackingChanged(bool)), this, SLOT(onEnergyPositionUndulatorTrackingChanged(bool)));
	connect(energyPosition_, SIGNAL(undulatorPositionChanged(double)), this, SLOT(onEnergyUndulatorPositionChanged(double)));
	connect(energyPosition_, SIGNAL(undulatorOffsetChanged(double)), this, SLOT(onEnergyPositionUndulatorOffsetChanged(double)));
	connect(energyPosition_, SIGNAL(undulatorHarmonicChanged(SGMUndulatorSupport::UndulatorHarmonic)),
	        this, SLOT(onEnergyPositionUndulatorHarmonicChanged(SGMUndulatorSupport::UndulatorHarmonic)));

	connect(energyPosition_, SIGNAL(exitSlitTrackingChanged(bool)), this, SLOT(onEnergyPositionExitSlitTrackingChanged(bool)));
	connect(energyPosition_, SIGNAL(exitSlitPositionChanged(double)), this, SLOT(onEnergyPositionExitSlitPositionChanged(double)));
	connect(energyPosition_, SIGNAL(energyChanged(double)), this, SLOT(onEnergyPositionEnergyChanged(double)));
	connect(energyPosition_->errorValidator(), SIGNAL(failureCountChanged(int)), this, SLOT(onEnergyPositionErrorCountChanged(int)));
	connect(energyPosition_->warningValidator(), SIGNAL(failureCountChanged(int)), this, SLOT(onEnergyPositionWarningCountChanged(int)));

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

void SGMEnergyPositionTestView::setupData()
{
	onEnergyPositionGratingAngleChanged(energyPosition_->gratingAngle());
	onEnergyPositionGratingTranslationChanged(energyPosition_->gratingTranslation());
	onEnergyPositionUndulatorTrackingChanged(energyPosition_->isUndulatorTracking());
	onEnergyUndulatorPositionChanged(energyPosition_->undulatorPosition());
	onEnergyPositionUndulatorOffsetChanged(energyPosition_->undulatorOffset());
	onEnergyPositionUndulatorHarmonicChanged(energyPosition_->undulatorHarmonic());
	onEnergyPositionExitSlitTrackingChanged(energyPosition_->isExitSlitPositionTracking());
	onEnergyPositionExitSlitPositionChanged(energyPosition_->exitSlitPosition());
	onEnergyPositionEnergyChanged(energyPosition_->resultantEnergy());
	onEnergyPositionWarningCountChanged(energyPosition_->warningValidator()->failureCount());
	onEnergyPositionErrorCountChanged(energyPosition_->errorValidator()->failureCount());
	autoDetectUndulatorHarmonicCheckBox_->setChecked(energyPosition_->autoDetectUndulatorHarmonic());
	undulatorHarmonicComboBox_->setEnabled(!energyPosition_->autoDetectUndulatorHarmonic());
}
