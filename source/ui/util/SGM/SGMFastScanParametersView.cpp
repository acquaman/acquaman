#include "SGMFastScanParametersView.h"

#include <QPushButton>
#include <QHBoxLayout>

#include "util/SGM/SGMPeriodicTable.h"

SGMFastScanParametersView::SGMFastScanParametersView(SGMFastScanParameters *fastScanParameters, QWidget *parent) :
	QWidget(parent)
{
	fastScanParameters_ = fastScanParameters;

	startPositionCopy_ = fastScanParameters_->scanInfo().start();
	middlePositionCopy_ = fastScanParameters_->scanInfo().middle();
	endPositionCopy_ = fastScanParameters_->scanInfo().end();
	fastScanSettingsCopy_ = fastScanParameters_->fastScanSettings();

	//startPositionView_ = new SGMEnergyPositionView(&startPositionCopy_, SGMEnergyPositionView::ViewModeStartOrEnd);
	//middlePositionView_ = new SGMEnergyPositionView(&middlePositionCopy_, SGMEnergyPositionView::ViewModeMiddle);
	//endPositionView_ = new SGMEnergyPositionView(&endPositionCopy_, SGMEnergyPositionView::ViewModeStartOrEnd);
	startPositionView_ = new SGMEnergyPositionWBeamlineView(&startPositionCopy_, SGMEnergyPositionView::ViewModeStartOrEnd);
	middlePositionView_ = new SGMEnergyPositionWBeamlineView(&middlePositionCopy_, SGMEnergyPositionView::ViewModeMiddle);
	endPositionView_ = new SGMEnergyPositionWBeamlineView(&endPositionCopy_, SGMEnergyPositionView::ViewModeStartOrEnd);
	fastScanSettingsView_ = new SGMFastScanSettingsView(&fastScanSettingsCopy_);

	copyButton_ = new QPushButton("Copy");
	editButton_ = new QPushButton("Edit");

	saveButton_ = new QPushButton("Save");
	cancelButton_ = new QPushButton("Cancel");

	startPositionView_->setEnabled(false);
	middlePositionView_->setEnabled(false);
	endPositionView_->setEnabled(false);
	fastScanSettingsView_->setEnabled(false);

	if(fastScanParameters_->database()->isReadOnly())
		editButton_->setEnabled(false);

	QVBoxLayout *settingsLayout = new QVBoxLayout();
	settingsLayout->addWidget(fastScanSettingsView_);
	settingsLayout->addStretch(8);
	settingsLayout->setContentsMargins(5, 0, 5, 0);

	QVBoxLayout *positionsLayout = new QVBoxLayout();
	positionsLayout->addWidget(startPositionView_);
	positionsLayout->addWidget(middlePositionView_);
	positionsLayout->addWidget(endPositionView_);
	positionsLayout->addStretch(8);
	positionsLayout->setContentsMargins(5, 0, 5, 0);

	QHBoxLayout *hl = new QHBoxLayout();
	hl->addLayout(settingsLayout);
	hl->addLayout(positionsLayout);

	buttonsHL1_ = new QHBoxLayout();
	buttonsHL1_->addStretch(8);
	buttonsHL1_->addWidget(editButton_);
	buttonsHL1_->addWidget(copyButton_);

	buttonsHL2_ = new QHBoxLayout();
	buttonsHL2_->addStretch(8);
	buttonsHL2_->addWidget(saveButton_);
	buttonsHL2_->addWidget(cancelButton_);

	masterVL_ = new QVBoxLayout();
	masterVL_->addLayout(hl);
	masterVL_->addLayout(buttonsHL1_);

	setLayout(masterVL_);

	connect(copyButton_, SIGNAL(clicked()), this, SLOT(onCopyButtonClicked()));
	connect(editButton_, SIGNAL(clicked()), this, SLOT(onEditButtonClicked()));
	connect(saveButton_, SIGNAL(clicked()), this, SLOT(onSaveButtonClicked()));
	connect(cancelButton_, SIGNAL(clicked()), this, SLOT(onCancelButtonClicked()));

	connect(&startPositionCopy_, SIGNAL(energyPositionChanged()), this, SLOT(onStartPositionCopyChanged()));
	connect(&middlePositionCopy_, SIGNAL(energyPositionChanged()), this, SLOT(onMiddlePositionCopyChanged()));
	connect(&endPositionCopy_, SIGNAL(energyPositionChanged()), this, SLOT(onEndPositionCopyChanged()));
	connect(&fastScanSettingsCopy_, SIGNAL(fastScanSettingsChanged()), this, SLOT(onFastScanSettingsCopyChanged()));
}

void SGMFastScanParametersView::onEditButtonClicked(){
	startPositionView_->setEnabled(true);
	middlePositionView_->setEnabled(true);
	endPositionView_->setEnabled(true);
	fastScanSettingsView_->setEnabled(true);

	masterVL_->removeItem(buttonsHL1_);
	editButton_->hide();
	copyButton_->hide();

	masterVL_->addLayout(buttonsHL2_);
	saveButton_->show();
	saveButton_->setEnabled(false);
	cancelButton_->show();
}

void SGMFastScanParametersView::onCopyButtonClicked(){

}

void SGMFastScanParametersView::onSaveButtonClicked(){
	bool isModified = false;
	if(fastScanParameters_->scanInfo().start() != startPositionCopy_){
		fastScanParameters_->setStartPosition(startPositionCopy_);
		isModified = true;
	}
	if(fastScanParameters_->scanInfo().middle() != middlePositionCopy_){
		fastScanParameters_->setMiddlePosition(middlePositionCopy_);
		isModified = true;
	}
	if(fastScanParameters_->scanInfo().end() != endPositionCopy_){
		fastScanParameters_->setEndPosition(endPositionCopy_);
		isModified = true;
	}
	if(fastScanParameters_->fastScanSettings() != fastScanSettingsCopy_){
		fastScanParameters_->setFastScanSettings(fastScanSettingsCopy_);
		isModified = true;
	}

	if(isModified)
		fastScanParameters_->storeToDb(fastScanParameters_->database());
}

void SGMFastScanParametersView::onCancelButtonClicked(){
	if(fastScanParameters_->scanInfo().start() != startPositionCopy_)
		startPositionCopy_ = fastScanParameters_->scanInfo().start();
	if(fastScanParameters_->scanInfo().middle() != middlePositionCopy_)
		middlePositionCopy_ = fastScanParameters_->scanInfo().middle();
	if(fastScanParameters_->scanInfo().end() != endPositionCopy_)
		endPositionCopy_ = fastScanParameters_->scanInfo().end();
	if(fastScanParameters_->fastScanSettings() != fastScanSettingsCopy_)
		fastScanSettingsCopy_ = fastScanParameters_->fastScanSettings();

	startPositionView_->setEnabled(false);
	middlePositionView_->setEnabled(false);
	endPositionView_->setEnabled(false);
	fastScanSettingsView_->setEnabled(false);

	masterVL_->removeItem(buttonsHL2_);
	saveButton_->hide();
	cancelButton_->hide();

	masterVL_->addLayout(buttonsHL1_);
	editButton_->show();
	copyButton_->show();
}

void SGMFastScanParametersView::onStartPositionCopyChanged(){
	saveButton_->setEnabled(true);
}

void SGMFastScanParametersView::onMiddlePositionCopyChanged(){
	saveButton_->setEnabled(true);
}

void SGMFastScanParametersView::onEndPositionCopyChanged(){
	saveButton_->setEnabled(true);
}

void SGMFastScanParametersView::onFastScanSettingsCopyChanged(){
	saveButton_->setEnabled(true);
}
