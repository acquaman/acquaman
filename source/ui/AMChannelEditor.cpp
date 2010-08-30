#include "AMChannelEditor.h"
#include "ui/AMCloseItemDelegate.h"

AMChannelEditor::AMChannelEditor(AMScanSetModel* model, QWidget *parent) :
	QWidget(parent)
{
	ui_.setupUi(this);
	ui_.nameEdit->setReadOnly(true);	// you can't edit existing channel names.

	AMCloseItemDelegate* del = new AMCloseItemDelegate(this);
	del->setCloseButtonsEnabled(true);
	connect(del, SIGNAL(closeButtonClicked(QModelIndex)), this, SLOT(onCloseButtonClicked(QModelIndex)));
	ui_.scanSetView->setItemDelegate(del);
	model_ = model;
	ui_.scanSetView->setModel(model_);
	connect(ui_.scanSetView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onSetViewIndexChanged(QModelIndex,QModelIndex)));

	// Save a bit of room inside the
	ui_.scanSetView->setRootIsDecorated(false);

	ui_.addChannelButton->setDisabled(true);
	connect(ui_.addChannelButton, SIGNAL(clicked()), this, SLOT(onAddChannelButtonClicked()));


}

void AMChannelEditor::setCurrentScan(AMScan *scan) {
	if(!scan)
		setCurrentScan(-1);
	else
		setCurrentScan(model_->indexOf(scan));
}

void AMChannelEditor::setCurrentScan(int scanIndex) {
	if(scanIndex < 0 || scanIndex >= model_->numScans()) {
		ui_.scanSetView->setCurrentIndex(QModelIndex());
		ui_.scanSetView->collapseAll();
		return;
	}

	ui_.scanSetView->collapseAll();
	QModelIndex i = model_->indexForScan(scanIndex);
	ui_.scanSetView->expand(i);
	ui_.scanSetView->setCurrentIndex(i);

}

void AMChannelEditor::onSetViewIndexChanged(const QModelIndex &selected, const QModelIndex &deselected) {

	// Nothing selected?
	////////////////////////
	if(!selected.isValid()) {
		ui_.nameEdit->clear();
		ui_.descriptionEdit->clear();
		ui_.descriptionEdit->setReadOnly(true);
		ui_.expressionEdit->clear();
		ui_.expressionEdit->setReadOnly(true);
		ui_.insertButton->setDisabled(true);
		ui_.addChannelButton->setDisabled(true);
		return;
	}

	ui_.addChannelButton->setEnabled(true);

	// Scan selected? (No selected channel)
	/////////////////////////
	if(!selected.parent().isValid()) {
		ui_.nameEdit->clear();
		ui_.descriptionEdit->clear();
		ui_.descriptionEdit->setReadOnly(true);
		ui_.expressionEdit->clear();
		ui_.expressionEdit->setReadOnly(true);
		ui_.insertButton->setDisabled(true);
		return;
	}


	// Channel selected.
	/////////////////
	int si = selected.parent().row();
	int ci = selected.row();
	AMChannel* channel = model_->channelAt(si, ci);
	if(!channel)
		return;


	ui_.nameEdit->setText(channel->name());
	ui_.descriptionEdit->setText(channel->name());	/// \todo incorporate descriptions
	ui_.descriptionEdit->setReadOnly(false);
	ui_.expressionEdit->setPlainText(channel->expression());
	ui_.expressionEdit->setReadOnly(false);
	ui_.insertButton->setEnabled(true);

}

#include <QMessageBox>
void AMChannelEditor::onCloseButtonClicked(const QModelIndex &index) {
	/// channel-level indexes only:
	if(!index.parent().isValid())
		return;

	int channelIndex = index.row();
	int scanIndex = index.parent().row();

	AMScan* scan = model_->scanAt(scanIndex);
	if(!scan || channelIndex >= scan->numChannels())
		return;

	AMChannel* channel = scan->channel(channelIndex);
	int response = QMessageBox::question(this, "Remove Channel?", QString("Remove this channel? \n\n%1 = %2").arg(channel->name()).arg(channel->expression()), QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok);

	if(response == QMessageBox::Ok)
		scan->deleteChannel(channelIndex);
}

int AMChannelEditor::currentScanIndex() const {
	QModelIndex i = ui_.scanSetView->currentIndex();
	if(!i.isValid())
		return -1;

	if(!i.parent().isValid())
		return i.row();

	else
		return i.parent().row();

}
void AMChannelEditor::onAddChannelButtonClicked() {

	/// Channel names aren't editable after they've been created. Here we re-enable the nameEdit for editing shortly, and connect its editingFinished ... We'll complete the operation in onNewChannelNamed().
	ui_.nameEdit->setReadOnly(false);
	ui_.nameEdit->setText("newChannelName");
	ui_.nameEdit->setFocus();
	ui_.nameEdit->selectAll();

	connect(ui_.nameEdit, SIGNAL(editingFinished()), this, SLOT(onNewChannelNamed()));

}

#include "AMErrorMonitor.h"
void AMChannelEditor::onNewChannelNamed() {
	QString chName = ui_.nameEdit->text();
	disconnect(ui_.nameEdit, SIGNAL(editingFinished()), this, SLOT(onNewChannelNamed()));
	ui_.nameEdit->clearFocus();

	int si = currentScanIndex();
	if(si < 0 || si >= model_->numScans())
		return;

	if(model_->scanAt(si)->addChannel(chName, "1")) {
		int ci = model_->scanAt(si)->numChannels()-1;
		ui_.scanSetView->setCurrentIndex(model_->indexForChannel(si, ci));
		ui_.expressionEdit->setFocus();
	}
	else {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -1, QString("Couldn't create a new channel (plot curve) with the name %1. Make sure to choose a name that doesn't exist already.").arg(chName)));
		ui_.scanSetView->setCurrentIndex(ui_.scanSetView->currentIndex());
	}

}
