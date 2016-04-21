/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#include "AM1DCalibrationABEditor.h"
#include "ui/dataman/AMChooseScanDialog.h"
#include "dataman/AMScan.h"
#include "util/AMErrorMonitor.h"

#include <QLabel>
#include <QFormLayout>
#include <QProgressDialog>
#include <QUrl>

AM1DCalibrationABEditor::AM1DCalibrationABEditor(AM1DCalibrationAB *analysisBlock, QWidget *parent)
	: QWidget(parent)
{
	analysisBlock_ = analysisBlock;
	chooseScanDialog_ = 0;
	dataNames_ = new QComboBox;
	NormalizationNames_ = new QComboBox;
	populateComboBox();

	/// Spinners for Energy calibration
	energyCalibrationOffsetBox_ = new QDoubleSpinBox;
	energyCalibrationOffsetBox_->setSingleStep(0.1);
	energyCalibrationOffsetBox_->setRange(-1000,1000);
	energyCalibrationOffsetBox_->setValue(analysisBlock_->energyCalibrationOffset());
	energyCalibrationReferenceBox_ = new QDoubleSpinBox;
	energyCalibrationReferenceBox_->setSingleStep(0.1);
	energyCalibrationReferenceBox_->setRange(-1000,20000);
	energyCalibrationReferenceBox_->setValue(analysisBlock_->energyCalibrationReference());
	energyCalibrationScalingBox_ = new QDoubleSpinBox;
	energyCalibrationScalingBox_->setSingleStep(0.01);
	energyCalibrationScalingBox_->setRange(0.01,5);
	energyCalibrationScalingBox_->setValue(analysisBlock_->energyCalibrationScaling());

	isTransmissionCheckbox_ = new QCheckBox;
	isTransmissionCheckbox_->setChecked(analysisBlock_->isTransmission());
	toEdgeJumpCheckbox_ = new QCheckBox;
	toEdgeJumpCheckbox_->setChecked(analysisBlock_->toEdgeJump());
	preEdgePointSlider_ = new QSlider(Qt::Horizontal);
	preEdgePointSlider_->setValue(analysisBlock_->preEdgePoint());
	preEdgePointSlider_->setRange(-1,analysisBlock_->size(0)-1);
	preEdgePointSlider_->setEnabled(analysisBlock_->toEdgeJump());
	postEdgePointSlider_ = new QSlider(Qt::Horizontal);
	postEdgePointSlider_->setValue(analysisBlock_->postEdgePoint());
	postEdgePointSlider_->setRange(-1,analysisBlock_->size(0)-1);
	postEdgePointSlider_->setEnabled(analysisBlock_->toEdgeJump());




		/// Apply to sources/scans button
	applyToAllSourcesButton_ = new QPushButton("Apply to All Sources");
	applyToScansButton_ = new QPushButton("Apply to Scans...");


	connect(dataNames_, SIGNAL(currentIndexChanged(int)), this, SLOT(onDataNameChoiceChanged(int)));
	connect(NormalizationNames_, SIGNAL(currentIndexChanged(int)), this, SLOT(onNormalizationNameChoiceChanged(int)));
	connect(analysisBlock_, SIGNAL(inputSourcesChanged()), this, SLOT(populateComboBox()));
	connect(energyCalibrationOffsetBox_, SIGNAL(valueChanged(double)), this, SLOT(onEnergyCalibrationOffsetChanged(double)));
	connect(energyCalibrationScalingBox_, SIGNAL(valueChanged(double)), this, SLOT(onEnergyCalibrationScalingChanged(double)));
	connect(energyCalibrationReferenceBox_, SIGNAL(valueChanged(double)), this, SLOT(onEnergyCalibrationReferenceChanged(double)));
	connect(applyToScansButton_, SIGNAL(clicked()), this, SLOT(onApplyToScansButtonClicked()));

	connect(isTransmissionCheckbox_, SIGNAL(clicked(bool)), this, SLOT(onIsTransmissionChanged(bool)));
	connect(toEdgeJumpCheckbox_, SIGNAL(clicked(bool)), this, SLOT(onToEdgeJumpChanged(bool)));
	connect(preEdgePointSlider_, SIGNAL(valueChanged(int)), this, SLOT(onPreEdgePointChanged(int)));
	connect(postEdgePointSlider_, SIGNAL(valueChanged(int)), this, SLOT(onPostEdgePointChanged(int)));

	preEdgePointLabel_ = new QLabel("Set to zero at:");
	postEdgePointLabel_ = new QLabel("Set to one at");

	if (analysisBlock_->inputDataSourceCount() > 0){

		dataNames_->setCurrentIndex(dataNames_->findData(analysisBlock_->dataName()));
		NormalizationNames_->setCurrentIndex(NormalizationNames_->findData(analysisBlock_->normalizationName()));
	}

	onPreEdgePointChanged(analysisBlock->preEdgePoint());  //to set widget labels
	onPostEdgePointChanged(analysisBlock->postEdgePoint());

	QFormLayout *layout = new QFormLayout;
	layout->addRow("Data:", dataNames_);
	layout->addRow("Normalization:", NormalizationNames_);
	layout->addRow("Transmisssion Spectra:", isTransmissionCheckbox_);
	layout->addRow("Normalize to Edge:", toEdgeJumpCheckbox_);
	layout->addRow(preEdgePointLabel_, preEdgePointSlider_);
	layout->addRow(postEdgePointLabel_, postEdgePointSlider_);
	layout->addRow("Offset:", energyCalibrationOffsetBox_);
	layout->addRow("Scaling:",energyCalibrationScalingBox_);
	layout->addRow("Reference:",energyCalibrationReferenceBox_);
	layout->addRow("", applyToScansButton_);

	setLayout(layout);

}

void AM1DCalibrationABEditor::populateComboBox()
{
	AMDataSource *tempSource = 0;
	int inputCount = analysisBlock_->inputDataSourceCount();

	for (int i = 0; i < inputCount; i++){

		tempSource = analysisBlock_->inputDataSourceAt(i);

		if (analysisBlock_->name() != tempSource->name()){

			dataNames_->addItem(tempSource->description(), tempSource->name());
			NormalizationNames_->addItem(tempSource->description(), tempSource->name());
		}
	}
}

void AM1DCalibrationABEditor::onDataNameChoiceChanged(int index)
{
	QString name = dataNames_->itemData(index).toString();
	analysisBlock_->setDataName(name);
	preEdgePointSlider_->setRange(-1,analysisBlock_->size(0)-1);
	postEdgePointSlider_->setRange(-1,analysisBlock_->size(0)-1);
}

void AM1DCalibrationABEditor::onEnergyCalibrationOffsetChanged(double offset)
{
	if(offset == analysisBlock_->energyCalibrationOffset())
		return;

	analysisBlock_->setEnergyCalibrationOffset(offset);
	applyToAllSources();

}

void AM1DCalibrationABEditor::onEnergyCalibrationScalingChanged(double scaling)
{
	if(scaling == analysisBlock_->energyCalibrationScaling())
		return;

	analysisBlock_->setEnergyCalibrationScaling(scaling);
	applyToAllSources();
}

void AM1DCalibrationABEditor::onEnergyCalibrationReferenceChanged(double reference)
{
	if(reference == analysisBlock_->energyCalibrationReference())
		return;
	analysisBlock_->setEnergyCalibrationReference(reference);
	applyToAllSources();
}

void AM1DCalibrationABEditor::onNormalizationNameChoiceChanged(int index)
{
	QString name = NormalizationNames_->itemData(index).toString();
	analysisBlock_->setNormalizationName(name);
	preEdgePointSlider_->setRange(-1,analysisBlock_->size(0)-1);
	postEdgePointSlider_->setRange(-1,analysisBlock_->size(0)-1);
	applyToAllSources();
}

void AM1DCalibrationABEditor::onIsTransmissionChanged(bool isTransmission)
{
	if(isTransmission == analysisBlock_->isTransmission())
		return;
	analysisBlock_->setIsTransmission(isTransmission);

}
void AM1DCalibrationABEditor::onToEdgeJumpChanged(bool toEdgeJump)
{
	if(toEdgeJump == analysisBlock_->toEdgeJump())
		return;
	analysisBlock_->setToEdgeJump(toEdgeJump);
	preEdgePointSlider_->setEnabled(toEdgeJump);
	postEdgePointSlider_->setEnabled(toEdgeJump);
	applyToAllSources();
}
void AM1DCalibrationABEditor::onPreEdgePointChanged(int preEdgePoint)
{
	if(preEdgePoint == -1)
		preEdgePointLabel_->setText(QString("Set to zero at first point:"));
	else
		preEdgePointLabel_->setText(QString("Set to zero at %1 eV:").arg(int(analysisBlock_->axisValue(0,preEdgePoint))));
	if(preEdgePoint == analysisBlock_->preEdgePoint())
		return;
	analysisBlock_->setPreEdgePoint(preEdgePoint);
	applyToAllSources();
}
void AM1DCalibrationABEditor::onPostEdgePointChanged(int postEdgePoint)
{
	if(postEdgePoint == -1)
		postEdgePointLabel_->setText(QString("Set to one at last point:"));
	else
		postEdgePointLabel_->setText(QString("Set one at %1 eV:").arg(int(analysisBlock_->axisValue(0,postEdgePoint))));
	if(postEdgePoint == analysisBlock_->postEdgePoint())
		return;
	analysisBlock_->setPostEdgePoint(postEdgePoint);

	applyToAllSources();
}






void AM1DCalibrationABEditor::applyToAllSources()
{

	AMScan* scan = analysisBlock_->scan();

	for(int i=0, cc=scan->analyzedDataSourceCount(); i<cc; ++i) {
		AM1DCalibrationAB* CalibrationAB = qobject_cast<AM1DCalibrationAB*>(scan->analyzedDataSources()->at(i));
		if(CalibrationAB && CalibrationAB != analysisBlock_) {
				CalibrationAB->setEnergyCalibrationOffset(analysisBlock_->energyCalibrationOffset());
				CalibrationAB->setEnergyCalibrationReference(analysisBlock_->energyCalibrationReference());
				CalibrationAB->setEnergyCalibrationScaling(analysisBlock_->energyCalibrationScaling());
				CalibrationAB->setToEdgeJump(analysisBlock_->toEdgeJump());
				CalibrationAB->setPreEdgePoint(analysisBlock_->preEdgePoint());
				CalibrationAB->setPostEdgePoint(analysisBlock_->postEdgePoint());


		}
	}
}




void AM1DCalibrationABEditor::onApplyToScansButtonClicked()
{
	if(!chooseScanDialog_) {
		chooseScanDialog_ = new AMChooseScanDialog(AMDatabase::database("user"), "Choose scans...", "Choose the scans you want to apply these analysis parameters to.", this);
		chooseScanDialog_->setAttribute(Qt::WA_DeleteOnClose, false);
	}
	connect(chooseScanDialog_, SIGNAL(accepted()), this, SLOT(onApplyToOtherScansChosen()));

	chooseScanDialog_->setPrompt(QString("Choose the scans you want to apply these analysis parameters to."));
	chooseScanDialog_->show();
}


void AM1DCalibrationABEditor::onApplyToOtherScansChosen()
{
	disconnect(chooseScanDialog_, SIGNAL(accepted()), this, SLOT(onApplyToOtherScansChosen()));

	QList<QUrl> scans = chooseScanDialog_->getSelectedScans();
	chooseScanDialog_->clearSelection();
	if(scans.isEmpty()) {
		chooseScanDialog_->close();
		return;
	}

	QProgressDialog* progressDialog = new QProgressDialog(QString("Applying analysis parameters to %1 scans...").arg(scans.count()), "Stop", 0, scans.count(), chooseScanDialog_);
	progressDialog->setMinimumDuration(0);

	int scansModified = 0;

	for(int s=0; s<scans.count(); ++s) {
		progressDialog->setValue(s);
		QUrl scanUrl = scans.at(s);
		bool isScanning;
		QString scanName;
		AMScan* scan = AMScan::createFromDatabaseUrl(scanUrl, false, &isScanning, &scanName);
		if(isScanning) {
			AMErrorMon::alert(this, -333, QString("Batch calibrating: Cannot apply calibration parameters to scan '%1' because it's still acquiring data.").arg(scanName));
			continue;
		}

		if(!scan) {
			AMErrorMon::alert(this, -334, QString("Batch calibrating: Could not open the scan at '%1'. This looks like a problem in the database; please report this problem to the REIXS Acquaman developers.").arg(scanUrl.toString()));
			continue;
		}

		bool calibrationABFound = false;
		for(int i=0, cc=scan->analyzedDataSourceCount(); i<cc; ++i) {
			AM1DCalibrationAB* CalibrationAB = qobject_cast<AM1DCalibrationAB*>(scan->analyzedDataSources()->at(i));
			if(CalibrationAB) {
				calibrationABFound = true;
					CalibrationAB->setEnergyCalibrationOffset(analysisBlock_->energyCalibrationOffset());
					CalibrationAB->setEnergyCalibrationReference(analysisBlock_->energyCalibrationReference());
					CalibrationAB->setEnergyCalibrationScaling(analysisBlock_->energyCalibrationScaling());
					CalibrationAB->setToEdgeJump(analysisBlock_->toEdgeJump());
					CalibrationAB->setPreEdgePoint(analysisBlock_->preEdgePoint());
					CalibrationAB->setPostEdgePoint(analysisBlock_->postEdgePoint());
			}
		}

		if(!calibrationABFound) {
			AMErrorMon::alert(this, -335, QString("Batch calibrating: Could not find a calibration tool in the scan '%1'").arg(scan->fullName()));
		}
		else {
			scansModified++;
			scan->storeToDb(scan->database());
		}

		scan->deleteLater();	// delete scan.
	}

	progressDialog->setValue(scans.count());
	progressDialog->deleteLater();

	AMErrorMon::information(this, 0, QString("Batch Calibrating: set the calibration parameters successfully for %1 XAS scans.").arg(scansModified));
	chooseScanDialog_->close();
}
