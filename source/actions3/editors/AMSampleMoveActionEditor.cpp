#include "AMSampleMoveActionEditor.h"

#include <QComboBox>
#include <QLabel>
#include <QBoxLayout>

#include "beamline/AMBeamline.h"
#include "dataman/AMSamplePlate.h"

AMSampleMoveActionEditor::AMSampleMoveActionEditor(AMSampleMoveActionInfo *info, QWidget *parent) :
	QFrame(parent)
{
	info_ = info;

	QHBoxLayout *hl = new QHBoxLayout(this);
	sampleSelectorBox_ = new QComboBox();
	hl->addWidget(new QLabel("Sample: "));
	hl->addWidget(sampleSelectorBox_);
	hl->addStretch(0);

	populateSamples();

	connect(sampleSelectorBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onSampleSeletectorBoxCurrentIndexChanged(int)));
}

AMSampleMoveActionEditor::~AMSampleMoveActionEditor()
{
}

void AMSampleMoveActionEditor::onSampleSeletectorBoxCurrentIndexChanged(int index){
	AMSamplePlate *currentPlate = AMBeamline::bl()->samplePlate();
	if(currentPlate && info_)
		info_->setSample(currentPlate->sampleAt(sampleSelectorBox_->itemData(index).toInt()));
}

void AMSampleMoveActionEditor::populateSamples(){
	if(!AMBeamline::bl()->samplePlate()){
		sampleSelectorBox_->addItem(QString("No Sample Plate"), QVariant(-1));
		sampleSelectorBox_->setEnabled(false);
	}
	else if(AMBeamline::bl()->samplePlate()->sampleCount() == 0){
		sampleSelectorBox_->addItem(QString("No Samples on Plate"), QVariant(-1));
		sampleSelectorBox_->setEnabled(false);
	}
	else{
		AMSamplePlate *currentPlate = AMBeamline::bl()->samplePlate();
		for(int x = 0, size = currentPlate->sampleCount(); x < size; x++)
			sampleSelectorBox_->addItem(QString(currentPlate->sampleAt(x)->name()), QVariant(x));
	}
}
