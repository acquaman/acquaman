#include "AMDataViewWithActionButtons.h"

AMDataViewWithActionButtons::AMDataViewWithActionButtons(AMDatabase* database, QWidget *parent) :
	AMDataView(database, parent)
{
	QFrame* actionsBarFrame = new QFrame;
	ui_ = new Ui::AMDataViewActionsBar;
	ui_->setupUi(actionsBarFrame);

	verticalLayout->addWidget(actionsBarFrame);


	connect(ui_->sizeSlider, SIGNAL(valueChanged(int)), this, SLOT(setItemSize(int)));
	connect(ui_->expandAllButton, SIGNAL(clicked()), this, SLOT(expandAll()));
	connect(ui_->collapseAllButton, SIGNAL(clicked()), this, SLOT(collapseAll()));

	connect(ui_->openSameEditorButton, SIGNAL(clicked()), this, SLOT(onCompareScansAction()));
	connect(ui_->openSeparateEditorButton, SIGNAL(clicked()), this, SLOT(onEditScansAction()));
	connect(ui_->exportButton, SIGNAL(clicked()), this, SLOT(onExportScansAction()));

	connect(this, SIGNAL(sceneDoubleClicked()), this, SLOT(onDoubleClick()));
	connect(this, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
}

void AMDataViewWithActionButtons::onDoubleClick()
{
	updateSelectedUrls();

	if(selectedUrls_.count() > 0)
		emit selectionActivated(selectedUrls_);
}

void AMDataViewWithActionButtons::onSelectionChanged()
{
	if(numberOfSelectedItems() > 0) {
		ui_->openSeparateEditorButton->setEnabled(true);
		ui_->openSameEditorButton->setEnabled(true);
		ui_->exportButton->setEnabled(true);
	}
	else {
		ui_->openSeparateEditorButton->setEnabled(false);
		ui_->openSameEditorButton->setEnabled(false);
		ui_->exportButton->setEnabled(false);
	}
}
