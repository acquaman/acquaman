#include "AMScanEditorsCloseView.h"

#include <QTreeView>
#include <QStandardItemModel>
#include <QPushButton>

#include "dataman/AMScanEditorModelItem.h"
#include "ui/dataman/AMGenericScanEditor.h"

AMScanEditorsCloseView::AMScanEditorsCloseView(QStandardItem *scanEditorsParentItem, AMDatamanAppController *appController, AMGenericScanEditor *originator, QWidget *parent) :
	QWidget(parent)
{
	scanEditorsParentItem_ = scanEditorsParentItem;
	appController_ = appController;
	originator_ = originator;

	scanEditorsModel_ = new QStandardItemModel();

	AMScanEditorModelItem *scanEditorItem;
	AMGenericScanEditor *scanEditor;
	for(int x = 0, size = scanEditorsParentItem_->rowCount(); x < size; x++){
		scanEditor = qobject_cast<AMGenericScanEditor*>(scanEditorsParentItem_->child(x, 0)->data(AM::WidgetRole).value<QWidget*>());
		if(scanEditor){
			scanEditorItem = new AMScanEditorModelItem(scanEditor, appController_);
			scanEditorItem->setText(scanEditorItem->data(AMScanEditorModelItem::ScanTextRole).toString());
			scanEditorItem->setCheckable(true);
			scanEditorsModel_->appendRow(scanEditorItem);
		}
	}

	QHBoxLayout *mainHL = new QHBoxLayout();
	QVBoxLayout *buttonVL = new QVBoxLayout();

	selectAllButton_ = new QPushButton("Select All");
	selectAllOthersButton_ = new QPushButton("Select All Others");
	if(!originator_)
		selectAllOthersButton_->setEnabled(false);
	selectSavedButton_ = new QPushButton("Select Saved");
	selectMultiplesButton_ = new QPushButton("Select Multiple Scans");
	selectSinglesButton_ = new QPushButton("Select Single Scans");

	buttonVL->addWidget(selectAllButton_);
	buttonVL->addWidget(selectAllOthersButton_);
	buttonVL->addWidget(selectSavedButton_);
	buttonVL->addWidget(selectMultiplesButton_);
	buttonVL->addWidget(selectSinglesButton_);
	buttonVL->addStretch();

	treeView_ = new QTreeView();
	treeView_->setModel(scanEditorsModel_);
	treeView_->setAlternatingRowColors(true);
	treeView_->setRootIsDecorated(false);

	mainHL->addLayout(buttonVL);
	mainHL->addWidget(treeView_);

	QVBoxLayout *mainVL = new QVBoxLayout();
	QHBoxLayout *confirmButtonsHL = new QHBoxLayout();

	closeButton_ = new QPushButton("Close Selected");
	cancelButton_ = new QPushButton("Cancel");
	confirmButtonsHL->addStretch();
	confirmButtonsHL->addWidget(closeButton_);
	confirmButtonsHL->addWidget(cancelButton_);

	mainVL->addLayout(mainHL);
	mainVL->addLayout(confirmButtonsHL);

	setLayout(mainVL);

	connect(selectAllButton_, SIGNAL(clicked()), this, SLOT(onSelectAllButtonClicked()));
	connect(selectAllOthersButton_, SIGNAL(clicked()), this, SLOT(onSelectAllOthersButtonClicked()));
	connect(selectSavedButton_, SIGNAL(clicked()), this, SLOT(onSelectSavedButtonClicked()));
	connect(selectMultiplesButton_, SIGNAL(clicked()), this, SLOT(onSelectMultiplesButtonClicked()));
	connect(selectSinglesButton_, SIGNAL(clicked()), this, SLOT(onSelectSinglesButtonClicked()));

	connect(closeButton_, SIGNAL(clicked()), this, SLOT(onCloseButtonClicked()));
	connect(cancelButton_, SIGNAL(clicked()), this, SLOT(onCancelButtonClicked()));
}

AMScanEditorsCloseView::~AMScanEditorsCloseView()
{
	AMScanEditorModelItem *scanEditorItem;
	while(scanEditorsModel_->rowCount() > 0){
		scanEditorItem = static_cast<AMScanEditorModelItem*>(scanEditorsModel_->takeRow(scanEditorsModel_->rowCount()-1).at(0));
		delete scanEditorItem;
	}
	scanEditorsModel_->deleteLater();
}

void AMScanEditorsCloseView::onSelectAllButtonClicked(){
	for(int x = 0, size = scanEditorsModel_->rowCount(); x < size; x++){
		scanEditorsModel_->item(x)->setCheckState(Qt::Checked);
	}
}

void AMScanEditorsCloseView::onSelectAllOthersButtonClicked(){
	AMGenericScanEditor *scanEditor;
	for(int x = 0, size = scanEditorsModel_->rowCount(); x < size; x++){
		scanEditor = qobject_cast<AMGenericScanEditor*>(scanEditorsModel_->item(x)->data(AM::WidgetRole).value<QWidget*>());
		if(scanEditor && scanEditor == originator_)
			scanEditorsModel_->item(x)->setCheckState(Qt::Unchecked);
		else
			scanEditorsModel_->item(x)->setCheckState(Qt::Checked);
	}
}

void AMScanEditorsCloseView::onSelectSavedButtonClicked(){
	AMScanEditorModelItem *scanEditorItem;
	for(int x = 0, size = scanEditorsModel_->rowCount(); x < size; x++){
		scanEditorItem = static_cast<AMScanEditorModelItem*>(scanEditorsModel_->item(x));
		if(!(scanEditorItem->data(AMScanEditorModelItem::IsModifiedRole).toBool()))
			scanEditorItem->setCheckState(Qt::Checked);
		else
			scanEditorItem->setCheckState(Qt::Unchecked);
	}
}

void AMScanEditorsCloseView::onSelectMultiplesButtonClicked(){
	AMScanEditorModelItem *scanEditorItem;
	for(int x = 0, size = scanEditorsModel_->rowCount(); x < size; x++){
		scanEditorItem = static_cast<AMScanEditorModelItem*>(scanEditorsModel_->item(x));
		if(scanEditorItem->data(AMScanEditorModelItem::IsMultipleRole).toBool())
			scanEditorItem->setCheckState(Qt::Checked);
		else
			scanEditorItem->setCheckState(Qt::Unchecked);
	}
}

void AMScanEditorsCloseView::onSelectSinglesButtonClicked(){
	AMScanEditorModelItem *scanEditorItem;
	for(int x = 0, size = scanEditorsModel_->rowCount(); x < size; x++){
		scanEditorItem = static_cast<AMScanEditorModelItem*>(scanEditorsModel_->item(x));
		if(!(scanEditorItem->data(AMScanEditorModelItem::IsMultipleRole).toBool()))
			scanEditorItem->setCheckState(Qt::Checked);
		else
			scanEditorItem->setCheckState(Qt::Unchecked);
	}
}

void AMScanEditorsCloseView::onCloseButtonClicked(){
	QList<AMGenericScanEditor*> editorsToClose;
	AMScanEditorModelItem *scanEditorItem;
	for(int x = 0, size = scanEditorsModel_->rowCount(); x < size; x++){
		scanEditorItem = static_cast<AMScanEditorModelItem*>(scanEditorsModel_->item(x));
		if(scanEditorItem->checkState() == Qt::Checked)
			editorsToClose.append(scanEditorItem->editorWidget());
	}

	emit closeRequested(editorsToClose);
	close();
}

void AMScanEditorsCloseView::onCancelButtonClicked(){
	close();
}

void AMScanEditorsCloseView::closeEvent(QCloseEvent *e){
	emit closed();
	QWidget::closeEvent(e);
}
