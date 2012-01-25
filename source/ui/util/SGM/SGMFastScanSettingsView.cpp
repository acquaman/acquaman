#include "SGMFastScanSettingsView.h"

#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QComboBox>
#include <QFormLayout>

SGMFastScanSettingsView::SGMFastScanSettingsView(SGMFastScanSettings *fastScanSettings, QWidget *parent) :
	QGroupBox(parent)
{
	fastScanSettings_ = 0;
	setTitle("Fast Scan Settings");

	runSecondsSpinBox_ = new QDoubleSpinBox();
	runSecondsSpinBox_->setMaximum(60);
	runSecondsSpinBox_->setMinimum(5);

	motorSettingsSpinBox_ = new QSpinBox();
	motorSettingsSpinBox_->setMaximum(80000);
	motorSettingsSpinBox_->setMinimum(1);

	baseLineSpinBox_ = new QSpinBox();
	baseLineSpinBox_->setMaximum(5000);
	baseLineSpinBox_->setMinimum(1);

	undulatorVelocitySpinBox_ = new QSpinBox();
	undulatorVelocitySpinBox_->setMaximum(8000);
	undulatorVelocitySpinBox_->setMinimum(1);

	setFastScanSettings(fastScanSettings);

	QFormLayout *fl = new QFormLayout();
	fl->addRow("Run Time", runSecondsSpinBox_);
	fl->addRow("Motor Settings", motorSettingsSpinBox_);
	fl->addRow("Base Line", baseLineSpinBox_);
	fl->addRow("Undulator Velocity", undulatorVelocitySpinBox_);

	setLayout(fl);
}

void SGMFastScanSettingsView::onRunSecondsEditingFinished(){
	if(fastScanSettings_)
		fastScanSettings_->setRunSeconds(runSecondsSpinBox_->value());
}

void SGMFastScanSettingsView::onMotorSettingsEditingFinished(){
	if(fastScanSettings_)
		fastScanSettings_->setMotorSettings(motorSettingsSpinBox_->value());
}

void SGMFastScanSettingsView::onBaseLineEditingFinished(){
	if(fastScanSettings_)
		fastScanSettings_->setBaseLine(baseLineSpinBox_->value());
}

void SGMFastScanSettingsView::onUndulatorVelocityEditingFinished(){
	if(fastScanSettings_)
		fastScanSettings_->setUndulatorVelocity(undulatorVelocitySpinBox_->value());
}

void SGMFastScanSettingsView::setFastScanSettings(SGMFastScanSettings *fastScanSettings){
	if(fastScanSettings_){
		disconnect(fastScanSettings_, SIGNAL(runSecondsChanged(double)), runSecondsSpinBox_, SLOT(setValue(double)));
		disconnect(fastScanSettings_, SIGNAL(motorSettingsChanged(int)), motorSettingsSpinBox_, SLOT(setValue(int)));
		disconnect(fastScanSettings_, SIGNAL(baseLineChanged(int)), baseLineSpinBox_, SLOT(setValue(int)));
		disconnect(fastScanSettings_, SIGNAL(undulatorVelocityChanged(int)), undulatorVelocitySpinBox_, SLOT(setValue(int)));

		disconnect(runSecondsSpinBox_, SIGNAL(editingFinished()), this, SLOT(onRunSecondsEditingFinished()));
		disconnect(motorSettingsSpinBox_, SIGNAL(editingFinished()), this, SLOT(onMotorSettingsEditingFinished()));
		disconnect(baseLineSpinBox_, SIGNAL(editingFinished()), this, SLOT(onBaseLineEditingFinished()));
		disconnect(undulatorVelocitySpinBox_, SIGNAL(editingFinished()), this, SLOT(onUndulatorVelocityEditingFinished()));
	}
	fastScanSettings_ = fastScanSettings;
	if(fastScanSettings_){
		if(!fastScanSettings_->name().isEmpty())
			setTitle(fastScanSettings_->name());

		runSecondsSpinBox_->setValue(fastScanSettings_->runSeconds());
		motorSettingsSpinBox_->setValue(fastScanSettings_->motorSettings());
		baseLineSpinBox_->setValue(fastScanSettings_->baseLine());
		undulatorVelocitySpinBox_->setValue(fastScanSettings_->undulatorVelocity());

		connect(fastScanSettings_, SIGNAL(runSecondsChanged(double)), runSecondsSpinBox_, SLOT(setValue(double)));
		connect(fastScanSettings_, SIGNAL(motorSettingsChanged(int)), motorSettingsSpinBox_, SLOT(setValue(int)));
		connect(fastScanSettings_, SIGNAL(baseLineChanged(int)), baseLineSpinBox_, SLOT(setValue(int)));
		connect(fastScanSettings_, SIGNAL(undulatorVelocityChanged(int)), undulatorVelocitySpinBox_, SLOT(setValue(int)));

		connect(runSecondsSpinBox_, SIGNAL(editingFinished()), this, SLOT(onRunSecondsEditingFinished()));
		connect(motorSettingsSpinBox_, SIGNAL(editingFinished()), this, SLOT(onMotorSettingsEditingFinished()));
		connect(baseLineSpinBox_, SIGNAL(editingFinished()), this, SLOT(onBaseLineEditingFinished()));
		connect(undulatorVelocitySpinBox_, SIGNAL(editingFinished()), this, SLOT(onUndulatorVelocityEditingFinished()));
	}
}
