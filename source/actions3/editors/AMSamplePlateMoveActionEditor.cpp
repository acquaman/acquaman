#include "AMSamplePlateMoveActionEditor.h"

#include <QComboBox>
#include <QLabel>
#include <QBoxLayout>
#include <QCheckBox>

#include <QDebug>

#include "beamline/AMBeamline.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/AMSamplePlate.h"
#include "util/AMErrorMonitor.h"

AMSamplePlateMoveActionEditor::AMSamplePlateMoveActionEditor(AMSamplePlateMoveActionInfo *info, QWidget *parent) :
	QFrame(parent)
{
	info_ = info;

	QHBoxLayout *hl = new QHBoxLayout(this);
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

	populateSamplePlates();
	populateSamplePositions();

	connect(samplePositionSelectorBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onSamplePositionSelectorBoxCurrentIndexChanged(int)));
	connect(samplePlateSelectorBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onSamplePlateSeletectorBoxCurrentIndexChanged(int)));
	connect(changeSamplePlateCheckBox_, SIGNAL(toggled(bool)), this, SLOT(onChangeSamplePlateCheckBoxToggled(bool)));
}

void AMSamplePlateMoveActionEditor::onSamplePlateSeletectorBoxCurrentIndexChanged(int index){
	info_->setSamplePlateId(samplePlateSelectorBox_->itemData(index).toInt());

	if(info_->samplePlateId() < 0)
		samplePositionSelectorBox_->setEnabled(false);
	else
		samplePositionSelectorBox_->setEnabled(true);

	populateSamplePositions();
}

void AMSamplePlateMoveActionEditor::onSamplePositionSelectorBoxCurrentIndexChanged(int index){
	info_->setSamplePositionIndex(samplePositionSelectorBox_->itemData(index).toInt());
}

void AMSamplePlateMoveActionEditor::onChangeSamplePlateCheckBoxToggled(bool canChangePlate){
	samplePlateSelectorBox_->setEnabled(canChangePlate);
}

void AMSamplePlateMoveActionEditor::populateSamplePlates(){
	int currentPlateIndex = 0;

	samplePlateSelectorBox_->clear();

	QSqlQuery q2 = AMDatabase::database("user")->query();
	q2.prepare(QString("SELECT id,name,dateTime FROM %1 ORDER BY dateTime ASC").arg(AMDbObjectSupport::s()->tableNameForClass<AMSamplePlate>()));
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

void AMSamplePlateMoveActionEditor::populateSamplePositions(){
	samplePositionSelectorBox_->clear();

	int currentPositionIndex = 0;

	if(samplePlateSelectorBox_->currentIndex() == 0)
		samplePositionSelectorBox_->addItem("No Plate Selected", QVariant(-1));
	else{
		samplePositionSelectorBox_->addItem("No Sample Selected", QVariant(-1));
		AMSamplePlate samplePlate;
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

		QSqlQuery q2 = AMDatabase::database("user")->select(AMDbObjectSupport::s()->tableNameForClass<AMSample>(), "id,name", QString("id IN (%1)").arg(sampleIdsSearchString.remove(sampleIdsSearchString.count()-1, 1)) );
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
