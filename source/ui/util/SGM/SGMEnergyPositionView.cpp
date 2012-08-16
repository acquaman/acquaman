/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "SGMEnergyPositionView.h"

#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QComboBox>
#include <QFormLayout>
#include <QPushButton>
#include <QLabel>
#include <QStringBuilder>

#include "dataman/database/AMDbObjectSupport.h"

SGMEnergyPositionView::SGMEnergyPositionView(SGMEnergyPosition *energyPosition, SGMEnergyPositionView::EnergyPositionViewMode alternateViewMode, QWidget *parent) :
	QGroupBox(parent)
{
	energyPosition_ = 0;
	alternateViewMode_ = alternateViewMode;
	setTitle("Energy Position");
	energySpinBox_ = new QDoubleSpinBox();
	energySpinBox_->setMaximum(2000.0);
	energySpinBox_->setMinimum(200.0);

	monoEncoderTargetSpinBox_ = new QSpinBox();
	monoEncoderTargetSpinBox_->setMaximum(0);
	monoEncoderTargetSpinBox_->setMinimum(-400000);

	undulatorStepSetpointSpinBox_ = new QSpinBox();
	undulatorStepSetpointSpinBox_->setMaximum(40000);
	undulatorStepSetpointSpinBox_->setMinimum(-160000);

	exitSlitDistanceSpinBox_ = new QDoubleSpinBox();
	exitSlitDistanceSpinBox_->setMaximum(690);
	exitSlitDistanceSpinBox_->setMinimum(1.0);

	sgmGratingComboBox_ = new QComboBox();
	QStringList sgmGratingValues;
	sgmGratingValues << "Low" << "Medium" << "High";
	sgmGratingComboBox_->addItems(sgmGratingValues);

	QHBoxLayout *tmpHL;
	QVBoxLayout *vl1 = new QVBoxLayout();

	energyLabel_ = new QLabel("Energy");
	tmpHL = new QHBoxLayout();
	tmpHL->addWidget(energyLabel_);
	tmpHL->addWidget(energySpinBox_);
	vl1->addLayout(tmpHL);

	monoEncoderLabel_ = new QLabel("Mono Encoder");
	tmpHL = new QHBoxLayout();
	tmpHL->addWidget(monoEncoderLabel_);
	tmpHL->addWidget(monoEncoderTargetSpinBox_);
	vl1->addLayout(tmpHL);

	undulatorStartStepLabel_ = new QLabel("Undulator Step");
	tmpHL = new QHBoxLayout();
	tmpHL->addWidget(undulatorStartStepLabel_);
	tmpHL->addWidget(undulatorStepSetpointSpinBox_);
	vl1->addLayout(tmpHL);

	exitSlitDistanceLabel_ = new QLabel("Exit Slit Position");
	tmpHL = new QHBoxLayout();
	tmpHL->addWidget(exitSlitDistanceLabel_);
	tmpHL->addWidget(exitSlitDistanceSpinBox_);
	vl1->addLayout(tmpHL);

	sgmGratingLabel_ = new QLabel("Energy");
	tmpHL = new QHBoxLayout();
	tmpHL->addWidget(sgmGratingLabel_);
	tmpHL->addWidget(sgmGratingComboBox_);
	vl1->addLayout(tmpHL);

	alternateViewModeButton_ = new QPushButton("More");

	vl2_ = new QVBoxLayout();
	vl2_->addStretch(10);
	vl2_->addWidget(alternateViewModeButton_);

	hl_ = new QHBoxLayout();
	hl_->addLayout(vl1);
	hl_->addLayout(vl2_);

	setLayout(hl_);

	if(alternateViewMode_ == SGMEnergyPositionView::ViewModeAll)
		alternateViewModeButton_->hide();
	else{
		connect(alternateViewModeButton_, SIGNAL(clicked()), this, SLOT(onAlternateViewModeClicked()));
		currentViewMode_ = alternateViewMode_;
		onViewModeChanged();
	}
	setEnergyPosition(energyPosition);
}

void SGMEnergyPositionView::onEnergyEditingFinished(){
	if(energyPosition_)
		energyPosition_->setEnergy(energySpinBox_->value());
}

void SGMEnergyPositionView::onMonoEditingFinished(){
	if(energyPosition_)
		energyPosition_->setMonoEncoderTarget(monoEncoderTargetSpinBox_->value());
}

void SGMEnergyPositionView::onUndulatorEditingFinished(){
	if(energyPosition_)
		energyPosition_->setUndulatorStepSetpoint(undulatorStepSetpointSpinBox_->value());
}

void SGMEnergyPositionView::onExitSlitEditingFinished(){
	if(energyPosition_)
		energyPosition_->setExitSlitDistance(exitSlitDistanceSpinBox_->value());
}

void SGMEnergyPositionView::setEnergyPosition(SGMEnergyPosition *energyPosition){
	if(energyPosition_){
		disconnect(energyPosition_, SIGNAL(energyChanged(double)), energySpinBox_, SLOT(setValue(double)));
		disconnect(energyPosition_, SIGNAL(monoEncoderTargetChanged(int)), monoEncoderTargetSpinBox_, SLOT(setValue(int)));
		disconnect(energyPosition_, SIGNAL(undulatorStepSetpointChanged(int)), undulatorStepSetpointSpinBox_, SLOT(setValue(int)));
		disconnect(energyPosition_, SIGNAL(exitSlitDistanceChanged(double)), exitSlitDistanceSpinBox_, SLOT(setValue(double)));
		disconnect(energyPosition_, SIGNAL(sgmGratingChanged(int)), sgmGratingComboBox_, SLOT(setCurrentIndex(int)));

		disconnect(energySpinBox_, SIGNAL(editingFinished()), this, SLOT(onEnergyEditingFinished()));
		disconnect(monoEncoderTargetSpinBox_, SIGNAL(editingFinished()), this, SLOT(onMonoEditingFinished()));
		disconnect(undulatorStepSetpointSpinBox_, SIGNAL(editingFinished()), this, SLOT(onUndulatorEditingFinished()));
		disconnect(exitSlitDistanceSpinBox_, SIGNAL(editingFinished()), this, SLOT(onExitSlitEditingFinished()));
		disconnect(sgmGratingComboBox_, SIGNAL(activated(int)), energyPosition_, SLOT(setSGMGrating(int)));
	}
	energyPosition_ = energyPosition;
	if(energyPosition_){
		if(!energyPosition_->name().isEmpty())
			setTitle(energyPosition_->descriptionFromName());

		energySpinBox_->setValue(energyPosition_->energy());
		monoEncoderTargetSpinBox_->setValue(energyPosition_->monoEncoderTarget());
		undulatorStepSetpointSpinBox_->setValue(energyPosition_->undulatorStepSetpoint());
		exitSlitDistanceSpinBox_->setValue(energyPosition_->exitSlitDistance());
		sgmGratingComboBox_->setCurrentIndex(energyPosition_->sgmGrating());

		connect(energyPosition_, SIGNAL(energyChanged(double)), energySpinBox_, SLOT(setValue(double)));
		connect(energyPosition_, SIGNAL(monoEncoderTargetChanged(int)), monoEncoderTargetSpinBox_, SLOT(setValue(int)));
		connect(energyPosition_, SIGNAL(undulatorStepSetpointChanged(int)), undulatorStepSetpointSpinBox_, SLOT(setValue(int)));
		connect(energyPosition_, SIGNAL(exitSlitDistanceChanged(double)), exitSlitDistanceSpinBox_, SLOT(setValue(double)));
		connect(energyPosition_, SIGNAL(sgmGratingChanged(int)), sgmGratingComboBox_, SLOT(setCurrentIndex(int)));

		connect(energySpinBox_, SIGNAL(editingFinished()), this, SLOT(onEnergyEditingFinished()));
		connect(monoEncoderTargetSpinBox_, SIGNAL(editingFinished()), this, SLOT(onMonoEditingFinished()));
		connect(undulatorStepSetpointSpinBox_, SIGNAL(editingFinished()), this, SLOT(onUndulatorEditingFinished()));
		connect(exitSlitDistanceSpinBox_, SIGNAL(editingFinished()), this, SLOT(onExitSlitEditingFinished()));
		connect(sgmGratingComboBox_, SIGNAL(activated(int)), energyPosition_, SLOT(setSGMGrating(int)));
	}
}

void SGMEnergyPositionView::onAlternateViewModeClicked(){
	if(alternateViewModeButton_->text() == "More"){
		alternateViewModeButton_->setText("Less");
		currentViewMode_ = SGMEnergyPositionView::ViewModeAll;
	}
	else{
		alternateViewModeButton_->setText("More");
		currentViewMode_ = alternateViewMode_;
	}
	onViewModeChanged();
}

void SGMEnergyPositionView::onViewModeChanged(){
	if(currentViewMode_ == SGMEnergyPositionView::ViewModeAll){
		monoEncoderLabel_->show();
		monoEncoderTargetSpinBox_->show();
		undulatorStartStepLabel_->show();
		undulatorStepSetpointSpinBox_->show();
		exitSlitDistanceLabel_->show();
		exitSlitDistanceSpinBox_->show();
		sgmGratingLabel_->show();
		sgmGratingComboBox_->show();
	}
	else if(currentViewMode_ == SGMEnergyPositionView::ViewModeStartOrEnd){
		monoEncoderLabel_->hide();
		monoEncoderTargetSpinBox_->hide();
		undulatorStartStepLabel_->show();
		undulatorStepSetpointSpinBox_->show();
		exitSlitDistanceLabel_->hide();
		exitSlitDistanceSpinBox_->hide();
		sgmGratingLabel_->hide();
		sgmGratingComboBox_->hide();
	}
	else{
		monoEncoderLabel_->hide();
		monoEncoderTargetSpinBox_->hide();
		undulatorStartStepLabel_->hide();
		undulatorStepSetpointSpinBox_->hide();
		exitSlitDistanceLabel_->show();
		exitSlitDistanceSpinBox_->show();
		sgmGratingLabel_->hide();
		sgmGratingComboBox_->hide();
	}
}

#include "beamline/SGM/SGMBeamline.h"

SGMEnergyPositionWBeamlineView::SGMEnergyPositionWBeamlineView(SGMEnergyPosition *energyPosition, SGMEnergyPositionView::EnergyPositionViewMode alternateViewMode, QWidget *parent) :
	SGMEnergyPositionView(energyPosition, alternateViewMode, parent)
{
	setFromBeamlineButton_ = new QPushButton("Set From\nBeamline");
	if(!SGMBeamline::sgm()->isConnected())
		setFromBeamlineButton_->hide();
	else
		connect(setFromBeamlineButton_, SIGNAL(clicked()), this, SLOT(onSetFromBeamlineButtonClicked()));

	vl2_->insertStretch(0, 10);
	vl2_->insertWidget(0, setFromBeamlineButton_);
}

void SGMEnergyPositionWBeamlineView::onSetFromBeamlineButtonClicked(){
	energyPosition_->setEnergy(SGMBeamline::sgm()->energy()->value());
	energyPosition_->setMonoEncoderTarget(SGMBeamline::sgm()->mono()->value());
	energyPosition_->setUndulatorStepSetpoint(SGMBeamline::sgm()->undulatorStep()->value());
	energyPosition_->setExitSlitDistance(SGMBeamline::sgm()->exitSlit()->value());
	energyPosition_->setSGMGrating(SGMBeamline::sgm()->grating()->value());
}

SGMEnergyPositionWBeamlineAndDatabaseView::SGMEnergyPositionWBeamlineAndDatabaseView(SGMEnergyPosition *energyPosition, SGMEnergyPositionView::EnergyPositionViewMode alternateViewMode, QWidget *parent) :
	SGMEnergyPositionWBeamlineView(energyPosition, alternateViewMode, parent)
{
	databaseUsedByLabel_ = new QLabel();
	setUsedByLabelHelper();
	disassociateButton_ = new QPushButton("Disassociate");

	QVBoxLayout *databaseVL = new QVBoxLayout();
	databaseVL->addWidget(databaseUsedByLabel_);
	databaseVL->addWidget(disassociateButton_);
	hl_->addLayout(databaseVL);

	connect(disassociateButton_, SIGNAL(clicked()), this, SLOT(onDisassociateButtonClicked()));
}

QStringList SGMEnergyPositionWBeamlineAndDatabaseView::alsoUsedByList() const{
	return alsoUsedByList_;
}

void SGMEnergyPositionWBeamlineAndDatabaseView::onDisassociateButtonClicked(){
	energyPosition_->dissociateFromDb();
	setUsedByLabelHelper();
}

void SGMEnergyPositionWBeamlineAndDatabaseView::setUsedByLabelHelper(){
	databaseUsedByLabel_->clear();
	QString usedByNames = "Used by:";
	if( (energyPosition_->id() > 0) && (energyPosition_->database()) ){
		QList<int> scanInfoMatchIDs = energyPosition_->database()->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMScanInfo>(), "start", QString("SGMEnergyPosition_table;%1").arg(energyPosition_->id()));
		scanInfoMatchIDs.append(energyPosition_->database()->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMScanInfo>(), "middle", QString("SGMEnergyPosition_table;%1").arg(energyPosition_->id())));
		scanInfoMatchIDs.append(energyPosition_->database()->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMScanInfo>(), "end", QString("SGMEnergyPosition_table;%1").arg(energyPosition_->id())));

		for(int x = 0; x < scanInfoMatchIDs.count(); x++){
			QList<int> fastScanParametersMatchIDs = energyPosition_->database()->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<SGMFastScanParameters>(), "scanInfo", QString("SGMScanInfo_table;%1").arg(scanInfoMatchIDs.at(x)));

			for(int y = 0; y < fastScanParametersMatchIDs.count(); y++){
				alsoUsedByList_.append(energyPosition_->database()->retrieve(fastScanParametersMatchIDs.at(y), AMDbObjectSupport::s()->tableNameForClass<SGMFastScanParameters>(), "name").toString());
				usedByNames.append("\n"%alsoUsedByList_.last());
			}
		}
	}
	else{
		usedByNames.append("\n<None>");
	}
	databaseUsedByLabel_->setText(usedByNames);
}
