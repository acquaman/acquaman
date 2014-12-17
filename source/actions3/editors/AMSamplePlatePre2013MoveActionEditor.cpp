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


#include "AMSamplePlatePre2013MoveActionEditor.h"

#include <QComboBox>
#include <QLabel>
#include <QBoxLayout>
#include <QCheckBox>
#include <QMessageBox>

#include "beamline/AMBeamline.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/AMSamplePlatePre2013.h"
#include "util/AMErrorMonitor.h"

AMSamplePlatePre2013MoveActionEditor::AMSamplePlatePre2013MoveActionEditor(AMSamplePlatePre2013MoveActionInfo *info, QWidget *parent) :
	QFrame(parent)
{
	info_ = info;

	QHBoxLayout *hl = new QHBoxLayout();
	samplePositionSelectorBox_ = new QComboBox();
	hl->addWidget(new QLabel("Sample: "));
	hl->addWidget(samplePositionSelectorBox_);
	changeSamplePlateCheckBox_ = new QCheckBox("Change sample plate");
	hl->addWidget(changeSamplePlateCheckBox_);
	samplePlateSelectorBox_ = new QComboBox();
	samplePlateSelectorBox_->setEnabled(false);
	hl->addWidget(new QLabel("Sample Plate: "));
	hl->addWidget(samplePlateSelectorBox_);
	hl->addStretch(0);

	setLayout(hl);

	populateSamplePlates();
	populateSamplePositions();

	connect(samplePositionSelectorBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onSamplePositionSelectorBoxCurrentIndexChanged(int)));
	connect(samplePlateSelectorBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onSamplePlateSeletectorBoxCurrentIndexChanged(int)));
	connect(changeSamplePlateCheckBox_, SIGNAL(toggled(bool)), this, SLOT(onChangeSamplePlateCheckBoxToggled(bool)));
}

AMSamplePlatePre2013MoveActionEditor::~AMSamplePlatePre2013MoveActionEditor(){}

void AMSamplePlatePre2013MoveActionEditor::onSamplePlateSeletectorBoxCurrentIndexChanged(int index){
	info_->setSamplePlateId(samplePlateSelectorBox_->itemData(index).toInt());

	if(info_->samplePlateId() < 0)
		samplePositionSelectorBox_->setEnabled(false);
	else{
		samplePositionSelectorBox_->setEnabled(true);
		if(info_->samplePlateId() != AMBeamline::bl()->currentSamplePlateId())
			QMessageBox::information(this, "Plate Mismatch", "You've chosen to use a sample plate that is different from the currently loaded beamline sample plate. You can do this; however, it may lead to unexpected results.", QMessageBox::Ok);
	}

	populateSamplePositions();
}

void AMSamplePlatePre2013MoveActionEditor::onSamplePositionSelectorBoxCurrentIndexChanged(int index){
	info_->setSamplePositionIndex(samplePositionSelectorBox_->itemData(index).toInt());

	/*
	if(info_->samplePlateId() > 0 && info_->samplePositionIndex() >= 0){
		qdebug() << "Wants " << info_->samplePlateId() << info_->samplePositionIndex() << "Would go to " << info_->samplePosition()->at(0).value() << info_->samplePosition()->at(1).value() << info_->samplePosition()->at(2).value();
	}
	*/
}

void AMSamplePlatePre2013MoveActionEditor::onChangeSamplePlateCheckBoxToggled(bool canChangePlate){
	samplePlateSelectorBox_->setEnabled(canChangePlate);
}

void AMSamplePlatePre2013MoveActionEditor::populateSamplePlates(){
	int currentPlateIndex = 0;

	samplePlateSelectorBox_->clear();

	QSqlQuery q2 = AMDatabase::database("user")->query();
	q2.prepare(QString("SELECT id,name,dateTime FROM %1 ORDER BY dateTime ASC").arg(AMDbObjectSupport::s()->tableNameForClass<AMSamplePlatePre2013>()));
	q2.exec();
	int id;
	QString name;
	samplePlateSelectorBox_->addItem("No Plate Selected", QVariant(-1));
	while(q2.next()) {
		id = q2.value(0).toInt();
		name = q2.value(1).toString();
		samplePlateSelectorBox_->addItem(name, QVariant(id));

		if(id == info_->samplePlateId())
			currentPlateIndex = samplePlateSelectorBox_->count()-1;
	}
	q2.finish();

	samplePlateSelectorBox_->blockSignals(true);
	samplePlateSelectorBox_->setCurrentIndex(currentPlateIndex);
	samplePlateSelectorBox_->blockSignals(false);

	if(currentPlateIndex <= 0)
		samplePositionSelectorBox_->setEnabled(false);
	else
		samplePositionSelectorBox_->setEnabled(true);
}

void AMSamplePlatePre2013MoveActionEditor::populateSamplePositions(){
	samplePositionSelectorBox_->clear();

	int currentPositionIndex = 0;

	if(samplePlateSelectorBox_->currentIndex() == 0)
		samplePositionSelectorBox_->addItem("No Plate Selected", QVariant(-1));
	else{
		samplePositionSelectorBox_->addItem("No Sample Selected", QVariant(-1));
		AMSamplePlatePre2013 samplePlate;
		if(!samplePlate.loadFromDb(AMDatabase::database("user"), samplePlateSelectorBox_->itemData(samplePlateSelectorBox_->currentIndex()).toInt()) ){
			AMErrorMon::alert(this, AMSAMPLEPLATEMOVEACTIONEDITOR_ATTEMPTED_TO_LOAD_BAD_PLATE, QString("The sample move action editor attempted to load a sample plate from the database that doesn't exist, id is %1.").arg(samplePlateSelectorBox_->itemData(samplePlateSelectorBox_->currentIndex()).toInt()));
			return;
		}

		QString sampleIdsSearchString;
		QMap<int, int> sampleIndexToPlateIndex;
		for(int x = 0; x < samplePlate.count(); x++){
			sampleIndexToPlateIndex.insert(samplePlate.at(x).sampleId(), x);
			sampleIdsSearchString.append( QString("%1,").arg(samplePlate.at(x).sampleId()) );
		}

		QSqlQuery q2 = AMDatabase::database("user")->select(AMDbObjectSupport::s()->tableNameForClass<AMSamplePre2013>(), "id,name", QString("id IN (%1)").arg(sampleIdsSearchString.remove(sampleIdsSearchString.count()-1, 1)) );
		q2.exec();
		QString name;
		int sampleId;
		while(q2.next()) {
			sampleId = q2.value(0).toInt();
			name = q2.value(1).toString();
			samplePositionSelectorBox_->addItem(name, sampleIndexToPlateIndex.value(sampleId));

			if(sampleIndexToPlateIndex.value(sampleId) == info_->samplePositionIndex())
				currentPositionIndex = samplePositionSelectorBox_->count()-1;
		}
		q2.finish();
	}

	samplePositionSelectorBox_->blockSignals(true);
	samplePositionSelectorBox_->setCurrentIndex(currentPositionIndex);
	samplePositionSelectorBox_->blockSignals(false);
}
