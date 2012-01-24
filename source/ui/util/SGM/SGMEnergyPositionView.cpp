#include "SGMEnergyPositionView.h"

#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QComboBox>
#include <QFormLayout>

SGMEnergyPositionView::SGMEnergyPositionView(SGMEnergyPosition *energyPosition, QWidget *parent) :
	QGroupBox(parent)
{
	energyPosition_ = energyPosition;
	setTitle("Energy Position");
	if(energyPosition_){
		if(!energyPosition_->name().isEmpty())
			setTitle(energyPosition_->name());

		energySpinBox_ = new QDoubleSpinBox();
		energySpinBox_->setMaximum(2000.0);
		energySpinBox_->setMinimum(200.0);
		energySpinBox_->setValue(energyPosition_->energy());

		monoEncoderTargetSpinBox_ = new QSpinBox();
		monoEncoderTargetSpinBox_->setMaximum(0);
		monoEncoderTargetSpinBox_->setMinimum(-400000);
		monoEncoderTargetSpinBox_->setValue(energyPosition_->monoEncoderTarget());

		undulatorStepSetpointSpinBox_ = new QSpinBox();
		undulatorStepSetpointSpinBox_->setMaximum(40000);
		undulatorStepSetpointSpinBox_->setMinimum(-160000);
		undulatorStepSetpointSpinBox_->setValue(energyPosition_->undulatorStepSetpoint());

		exitSlitDistanceSpinBox_ = new QDoubleSpinBox();
		exitSlitDistanceSpinBox_->setMaximum(690);
		exitSlitDistanceSpinBox_->setMinimum(1.0);
		exitSlitDistanceSpinBox_->setValue(energyPosition_->exitSlitDistance());

		sgmGratingComboBox_ = new QComboBox();
		QStringList sgmGratingValues;
		sgmGratingValues << "Low" << "Medium" << "High";
		sgmGratingComboBox_->addItems(sgmGratingValues);

		connect(energyPosition_, SIGNAL(energyChanged(double)), energySpinBox_, SLOT(setValue(double)));
		connect(energyPosition_, SIGNAL(monoEncoderTargetChanged(int)), monoEncoderTargetSpinBox_, SLOT(setValue(int)));
		connect(energyPosition_, SIGNAL(undulatorStepSetpointChanged(int)), undulatorStepSetpointSpinBox_, SLOT(setValue(int)));
		connect(energyPosition_, SIGNAL(exitSlitDistanceChanged(double)), exitSlitDistanceSpinBox_, SLOT(setValue(double)));
		connect(energyPosition_, SIGNAL(sgmGratingChanged(int)), sgmGratingComboBox_, SLOT(setCurrentIndex(int)));

		connect(energySpinBox_, SIGNAL(valueChanged(double)), energyPosition_, SLOT(setEnergy(double)));
		connect(monoEncoderTargetSpinBox_, SIGNAL(valueChanged(int)), energyPosition_, SLOT(setMonoEncoderTarget(int)));
		connect(undulatorStepSetpointSpinBox_, SIGNAL(valueChanged(int)), energyPosition_, SLOT(setUndulatorStepSetpoint(int)));
		connect(exitSlitDistanceSpinBox_, SIGNAL(valueChanged(double)), energyPosition_, SLOT(setExitSlitDistance(double)));
		connect(sgmGratingComboBox_, SIGNAL(activated(int)), energyPosition_, SLOT(setSGMGrating(int)));

	}
}
