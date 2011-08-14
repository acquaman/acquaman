#include "AMDataViewWithActionButtons.h"
#include <QBoxLayout>

AMDataViewWithActionButtons::AMDataViewWithActionButtons(AMDatabase* database, QWidget *parent) :
	QWidget(parent)
{
	QVBoxLayout* vl = new QVBoxLayout;
	dataView_ = new AMDataView(database);

	QFrame* actionsBarFrame = new QFrame;
	ui_ = new Ui::AMDataViewActionsBar;
	ui_->setupUi(actionsBarFrame);

	vl->setContentsMargins(0,0,0,0);
	vl->setSpacing(0);

	vl->addWidget(dataView_);
	vl->addWidget(actionsBarFrame);
	setLayout(vl);

	connect(ui_->sizeSlider, SIGNAL(valueChanged(int)), dataView_, SLOT(setItemSize(int)));
	connect(ui_->expandAllButton, SIGNAL(clicked()), dataView_, SLOT(expandAll()));
	connect(ui_->collapseAllButton, SIGNAL(clicked()), dataView_, SLOT(collapseAll()));

	connect(ui_->openSameEditorButton, SIGNAL(clicked()), this, SLOT(onCompareScansAction()));
	connect(ui_->openSeparateEditorButton, SIGNAL(clicked()), this, SLOT(onEditScansAction()));
	connect(ui_->exportButton, SIGNAL(clicked()), this, SLOT(onExportScansAction()));

	connect(dataView_, SIGNAL(sceneDoubleClicked()), this, SLOT(onDoubleClick()));
	connect(dataView_, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
}

void AMDataViewWithActionButtons::onDoubleClick()
{
	QList<QUrl> urls = dataView_->selectedItems();

	if(urls.count() > 0)
		emit selectionActivated(urls);
}

void AMDataViewWithActionButtons::onSelectionChanged()
{
	if(dataView_->numberOfSelectedItems() > 0) {
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
