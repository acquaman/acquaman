/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "SGMFastScanConfigurationView.h"

//#include "SGMSidebar.h"

#include <QGridLayout>
#include <QComboBox>
#include <QLabel>
#include <QFileDialog>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QFormLayout>

#include "ui/AMTopFrame.h"

SGMFastScanConfigurationView::SGMFastScanConfigurationView(SGMFastScanConfiguration *sfsc, QWidget *parent) :
		AMScanConfigurationView(parent)
{
	cfg_ = NULL;

	if(SGMBeamline::sgm()->isConnected()){
		cfg_ = sfsc;

		topFrame_ = new AMTopFrame("Configure a Fast Scan to Run Later");
		topFrame_->setIcon(QIcon(":/utilities-system-monitor.png"));

		presetsComboBox_ = new QComboBox();
		presetsComboBox_->addItems(sfsc->presets());
		connect(presetsComboBox_, SIGNAL(currentIndexChanged(int)), sfsc, SLOT(setParametersFromPreset(int)));

		elementLabel_ = new QLabel("Element");
		runTimeLabel_ = new QLabel("Run Time");
		startEnergyLabel_ = new QLabel("Start Energy");
		energyMidpointLabel_ = new QLabel("Energy Midpoint");
		endEnergyLabel_ = new QLabel("End Energy");
		motorSettingsLabel_ = new QLabel("Motor Settings");
		baseLineLabel_ = new QLabel("Baseline Counts");
		undulatorStartStepLabel_ = new QLabel("Undulator Start");
		undulatorRelativeStepLabel_ = new QLabel("Undulator Move");
		undulatorVelocityLabel_ = new QLabel("Undulator Velocity");
		exitSlitDistanceLabel_ = new QLabel("Exit Slit Position");
		warningsLabel_ = new QLabel("");
		QFont warningsFont;
		warningsFont.setPointSize(48);
		warningsLabel_->setFont(warningsFont);
		warningsLabel_->setStyleSheet( "QLabel{ color: red }" );

		elementEdit_ = new QLineEdit();
		elementEdit_->setText(sfsc->element());
		runTimeDSB_ = new QDoubleSpinBox();
		runTimeDSB_->setMinimum(0);
		runTimeDSB_->setMaximum(100);
		runTimeDSB_->setValue(sfsc->runTime());
		startEnergyDSB_ = new QDoubleSpinBox();
		startEnergyDSB_->setMinimum(200);
		startEnergyDSB_->setMaximum(2000);
		startEnergyDSB_->setValue(sfsc->energyStart());
		energyMidpointDSB_ = new QDoubleSpinBox();
		energyMidpointDSB_->setMinimum(200);
		energyMidpointDSB_->setMaximum(2000);
		energyMidpointDSB_->setValue(sfsc->energyMidpoint());
		endEnergyDSB_ = new QDoubleSpinBox();
		endEnergyDSB_->setMinimum(200);
		endEnergyDSB_->setMaximum(2000);
		endEnergyDSB_->setValue(sfsc->energyEnd());
		motorSettingsSB_ = new QSpinBox();
		motorSettingsSB_->setMinimum(100);
		motorSettingsSB_->setMaximum(50000);
		motorSettingsSB_->setValue(sfsc->velocity());
		baseLineSB_ = new QSpinBox();
		baseLineSB_->setMinimum(0);
		baseLineSB_->setMaximum(50000);
		baseLineSB_->setValue(sfsc->baseLine());
		undulatorStartStepSB_ = new QSpinBox();
		undulatorStartStepSB_->setMinimum(-150000);
		undulatorStartStepSB_->setMaximum(150000);
		undulatorStartStepSB_->setValue(sfsc->undulatorStartStep());
		undulatorRelativeStepSB_ = new QSpinBox();
		undulatorRelativeStepSB_->setMinimum(-50000);
		undulatorRelativeStepSB_->setMaximum(50000);
		undulatorRelativeStepSB_->setValue(sfsc->undulatorRelativeStep());
		undulatorVelocitySB_ = new QSpinBox();
		undulatorVelocitySB_->setMinimum(10);
		undulatorVelocitySB_->setMaximum(11811);
		undulatorVelocitySB_->setValue(sfsc->undulatorVelocity());
		exitSlitDistanceDSB_ = new QDoubleSpinBox();
		exitSlitDistanceDSB_->setMinimum(0.0);
		exitSlitDistanceDSB_->setMaximum(700.0);
		exitSlitDistanceDSB_->setValue(sfsc->exitSlitDistance());

		connect(sfsc, SIGNAL(onElementChanged(QString)), elementEdit_, SLOT(setText(QString)));
		connect(sfsc, SIGNAL(onRunSecondsChanged(double)), runTimeDSB_, SLOT(setValue(double)));
		connect(sfsc, SIGNAL(onEnergyStartChanged(double)), startEnergyDSB_, SLOT(setValue(double)));
		connect(sfsc, SIGNAL(onEnergyMidpointChanged(double)), energyMidpointDSB_, SLOT(setValue(double)));
		connect(sfsc, SIGNAL(onEnergyEndChanged(double)), endEnergyDSB_, SLOT(setValue(double)));
		connect(sfsc, SIGNAL(onVelocityChanged(int)), motorSettingsSB_, SLOT(setValue(int)));
		connect(sfsc, SIGNAL(onBaseLineChanged(int)), baseLineSB_, SLOT(setValue(int)));
		connect(sfsc, SIGNAL(undulatorStartStepChanged(int)), undulatorStartStepSB_, SLOT(setValue(int)));
		connect(sfsc, SIGNAL(undulatorRelativeStepChanged(int)), undulatorRelativeStepSB_, SLOT(setValue(int)));
		connect(sfsc, SIGNAL(undulatorVelocityChanged(int)), undulatorVelocitySB_, SLOT(setValue(int)));
		connect(sfsc, SIGNAL(exitSlitDistanceChanged(double)), exitSlitDistanceDSB_, SLOT(setValue(double)));

		connect(elementEdit_, SIGNAL(textEdited(QString)), sfsc, SLOT(setElement(QString)));
		connect(runTimeDSB_, SIGNAL(valueChanged(double)), sfsc, SLOT(setRunSeconds(double)));
		connect(runTimeDSB_, SIGNAL(valueChanged(double)), sfsc, SLOT(setScalerTime(double)));
		connect(startEnergyDSB_, SIGNAL(valueChanged(double)), sfsc, SLOT(setEnergyStart(double)));
		connect(energyMidpointDSB_, SIGNAL(valueChanged(double)), sfsc, SLOT(setEnergyMidpoint(double)));
		connect(endEnergyDSB_, SIGNAL(valueChanged(double)), sfsc, SLOT(setEnergyEnd(double)));
		connect(motorSettingsSB_, SIGNAL(valueChanged(int)), sfsc, SLOT(setVelocity(int)));
		connect(motorSettingsSB_, SIGNAL(valueChanged(int)), sfsc, SLOT(setVelocityBase(int)));
		connect(motorSettingsSB_, SIGNAL(valueChanged(int)), sfsc, SLOT(setAcceleration(int)));
		connect(baseLineSB_, SIGNAL(valueChanged(int)), sfsc, SLOT(setBaseLine(int)));
		connect(undulatorStartStepSB_, SIGNAL(valueChanged(int)), sfsc, SLOT(setUndulatorStartStep(int)));
		connect(undulatorRelativeStepSB_, SIGNAL(valueChanged(int)), sfsc, SLOT(setUndulatorRelativeStep(int)));
		connect(undulatorVelocitySB_, SIGNAL(valueChanged(int)), sfsc, SLOT(setUndulatorVelocity(int)));
		connect(exitSlitDistanceDSB_, SIGNAL(valueChanged(double)), sfsc, SLOT(setExitSlitDistance(double)));

		fl_ = new QFormLayout();
		fl_->addRow(elementLabel_, elementEdit_);
		fl_->addRow(runTimeLabel_, runTimeDSB_);
		fl_->addRow(startEnergyLabel_, startEnergyDSB_);
		fl_->addRow(energyMidpointLabel_, energyMidpointDSB_);
		fl_->addRow(endEnergyLabel_, endEnergyDSB_);
		fl_->addRow(motorSettingsLabel_, motorSettingsSB_);
		fl_->addRow(baseLineLabel_, baseLineSB_);
		fl_->addRow(undulatorStartStepLabel_, undulatorStartStepSB_);
		fl_->addRow(undulatorRelativeStepLabel_, undulatorRelativeStepSB_);
		fl_->addRow(undulatorVelocityLabel_, undulatorVelocitySB_);
		fl_->addRow(exitSlitDistanceLabel_, exitSlitDistanceDSB_);

		gl_ = new QGridLayout();
		gl_->addWidget(presetsComboBox_,	0, 0, 1, 1, Qt::AlignCenter);
		gl_->addLayout(fl_,			0, 1, 1, 1, Qt::AlignCenter);
		gl_->addWidget(warningsLabel_,		0, 0, 1, 2, Qt::AlignCenter);
		gl_->setRowStretch(1, 10);
		QVBoxLayout *vl = new QVBoxLayout();
		vl->addWidget(topFrame_);
		vl->addLayout(gl_);
		vl->setContentsMargins(0,0,0,0);
		vl->setSpacing(1);
		gl_->setContentsMargins(10, 0, 10, 0);
		this->setLayout(vl);
		this->setMaximumSize(700, 800);

		connect(SGMBeamline::sgm(), SIGNAL(criticalControlsConnectionsChanged()), this, SLOT(onSGMBeamlineCriticalControlsConnectedChanged()));
		onSGMBeamlineCriticalControlsConnectedChanged();
	}
}

const AMScanConfiguration* SGMFastScanConfigurationView::configuration() const{
	return cfg_;
}

void SGMFastScanConfigurationView::onSGMBeamlineCriticalControlsConnectedChanged(){
	if(SGMBeamline::sgm()->isConnected()){
		presetsComboBox_->setEnabled(true);
		elementLabel_->setEnabled(true);
		runTimeLabel_->setEnabled(true);
		startEnergyLabel_->setEnabled(true);
		energyMidpointLabel_->setEnabled(true);
		endEnergyLabel_->setEnabled(true);
		motorSettingsLabel_->setEnabled(true);
		baseLineLabel_->setEnabled(true);
		elementEdit_->setEnabled(true);
		runTimeDSB_->setEnabled(true);
		startEnergyDSB_->setEnabled(true);
		energyMidpointDSB_->setEnabled(true);
		endEnergyDSB_->setEnabled(true);
		motorSettingsSB_->setEnabled(true);
		baseLineSB_->setEnabled(true);
		warningsLabel_->setText("");
	}
	else{
		presetsComboBox_->setEnabled(false);
		elementLabel_->setEnabled(false);
		runTimeLabel_->setEnabled(false);
		startEnergyLabel_->setEnabled(false);
		energyMidpointLabel_->setEnabled(false);
		endEnergyLabel_->setEnabled(false);
		motorSettingsLabel_->setEnabled(false);
		baseLineLabel_->setEnabled(false);
		elementEdit_->setEnabled(false);
		runTimeDSB_->setEnabled(false);
		startEnergyDSB_->setEnabled(false);
		energyMidpointDSB_->setEnabled(false);
		endEnergyDSB_->setEnabled(false);
		motorSettingsSB_->setEnabled(false);
		baseLineSB_->setEnabled(false);
		warningsLabel_->setText("SGM Beamline Unavailable");
	}
}
