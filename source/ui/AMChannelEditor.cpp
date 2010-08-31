#include "AMChannelEditor.h"
#include "ui/AMCloseItemDelegate.h"
#include <QRegExpValidator>

AMChannelEditor::AMChannelEditor(AMScanSetModel* model, QWidget *parent) :
		QWidget(parent)
{
	ui_.setupUi(this);

	// Modify and adjust UI components
	ui_.nameEdit->setReadOnly(true);	// you can't edit existing channel names.
	// When making new names for channels, they better be mathematically sound variable names (No spaces, alphabetic character at beginning, no funky symbols, etc...)
	ui_.nameEdit->setValidator(new QRegExpValidator(QRegExp("[A-Za-z_]\\w*"), this));

	// create menus for the insert button:
	insertMenu_ = new QMenu(this);
	rawDataMenu_ = insertMenu_->addMenu("Raw Data");
	functionMenu_ = insertMenu_->addMenu("Function");
	ui_.insertButton->setMenu(insertMenu_);
	connect(rawDataMenu_, SIGNAL(triggered(QAction*)), this, SLOT(onRawDataMenuTriggered(QAction*)));
	connect(functionMenu_, SIGNAL(triggered(QAction*)), this, SLOT(onFunctionMenuTriggered(QAction*)));
	populateFunctionMenu();




	// apply 'close item' boxes onto the list of channels
	AMCloseItemDelegate* del = new AMCloseItemDelegate(this);
	del->setCloseButtonsEnabled(true);
	connect(del, SIGNAL(closeButtonClicked(QModelIndex)), this, SLOT(onCloseButtonClicked(QModelIndex)));
	ui_.scanSetView->setItemDelegate(del);

	// apply the existing scan set model to the channel list view:
	model_ = model;
	ui_.scanSetView->setModel(model_);
	connect(ui_.scanSetView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onSetViewIndexChanged(QModelIndex,QModelIndex)));

	// Save a bit of room inside the list of channels; don't show the drop-down arrows for scans
	ui_.scanSetView->setRootIsDecorated(false);

	ui_.addChannelButton->setDisabled(true);
	connect(ui_.addChannelButton, SIGNAL(clicked()), this, SLOT(onAddChannelButtonClicked()));


	editingNewChannel_ =  false;

	// connect editor widgets to test/apply changes:
	connect(ui_.expressionEdit, SIGNAL(textChanged()), this, SLOT(expressionEditingChanged()));
	connect(ui_.expressionEdit, SIGNAL(editingFinished(int)), this, SLOT(expressionEditingFinished(int)));

}

void AMChannelEditor::setCurrentScan(AMScan *scan) {
	if(!scan)
		setCurrentScan(-1);
	else
		setCurrentScan(model_->indexOf(scan));
}

void AMChannelEditor::setCurrentScan(int scanIndex) {

	abortExpressionEdit();

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

	abortExpressionEdit();

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
		populateExpressionMenu(-1);
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
		populateExpressionMenu(selected.row());
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
	ui_.expressionEdit->setText(channel->expression());
	ui_.expressionEdit->setReadOnly(false);
	ui_.insertButton->setEnabled(true);
	populateExpressionMenu(si);

}

#include <QMessageBox>
void AMChannelEditor::onCloseButtonClicked(const QModelIndex &index) {

	abortExpressionEdit();

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


int AMChannelEditor::currentChannelIndex() const {
	QModelIndex i = ui_.scanSetView->currentIndex();
	if(!i.parent().isValid())
		return -1;

	return i.row();
}

void AMChannelEditor::onAddChannelButtonClicked() {

	/// First things first. If we add a channel while editing another channel expression, there could be a mix-up. Abort that first
	abortExpressionEdit();

	/// Channel names aren't editable after they've been created. Here we re-enable the nameEdit for editing shortly, and connect its editingFinished ... We'll complete the operation in onNewChannelNamed().
	editingNewChannel_ = true;
	ui_.nameEdit->setReadOnly(false);
	ui_.nameEdit->setText("newChannelName");
	ui_.nameEdit->setFocus();
	ui_.nameEdit->selectAll();

	ui_.descriptionEdit->clear();
	ui_.expressionEdit->clear();


	connect(ui_.nameEdit, SIGNAL(editingFinished()), this, SLOT(onNewChannelNamed()));

}

#include "AMErrorMonitor.h"
void AMChannelEditor::onNewChannelNamed() {

	if(!editingNewChannel_)
		return;

	editingNewChannel_ = false;
	QString chName = ui_.nameEdit->text();
	disconnect(ui_.nameEdit, SIGNAL(editingFinished()), this, SLOT(onNewChannelNamed()));
	ui_.nameEdit->clearFocus();
	ui_.nameEdit->setReadOnly(true);


	int si = currentScanIndex();
	if(si < 0 || si >= model_->numScans()) {
		// this should have never happened. How did we get here? You shouldn't have been able to press the addNewChannel button without a current scan.
		return;
	}

	if(!chName.isEmpty() && model_->scanAt(si)->addChannel(chName, "1")) {
		int ci = model_->scanAt(si)->numChannels()-1;
		ui_.scanSetView->setCurrentIndex(model_->indexForChannel(si, ci));
		ui_.expressionEdit->setFocus();
	}
	else {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -1, QString("Couldn't create a new channel (plot curve) with the name \"%1\". Make sure to choose a valid name that doesn't exist already.").arg(chName)));
		ui_.scanSetView->setCurrentIndex(ui_.scanSetView->currentIndex());
	}

}



void AMChannelEditor::expressionEditingFinished(int reason) {
	// whoa now... If they just jumped over to the 'insert' button, we don't want to do all this finalization.

	switch(reason) {
	case AMWrappingLineEdit::EscapePressedReason:
		abortExpressionEdit();
		break;

	case Qt::PopupFocusReason:	// this is triggered when the popup menu for the menu options is pressed.
		// leave it in editingExpression_ mode... We want to keep working on this expression.
		break;

	case AMWrappingLineEdit::EnterPressedReason:
	default:	// should default (ie: click/tab to another widget) be abort or finalize? I'm going with finalize for now.
		finalizeExpressionEdit();
		break;


	}

}

void AMChannelEditor::expressionEditingChanged() {

	int ci = currentChannelIndex();
	int si = currentScanIndex();
	if( si<0 || ci<0 )
		return;	// who are you? Don't know which channel/scan to edit.

	editingExpression_ = true;

	AMChannel tempChannel(model_->scanAt(si), "testChannel", ui_.expressionEdit->text().trimmed() );
	if(tempChannel.isValid())
		ui_.expressionEdit->setStyleSheet("color: rgb(0,128,0);");
	else
		ui_.expressionEdit->setStyleSheet("color: red");

}

#include <QTextCursor>
void AMChannelEditor::finalizeExpressionEdit() {

	if(!editingExpression_)
		return;

	editingExpression_ = false;
	ui_.expressionEdit->setStyleSheet("color: black;");

	int ci = currentChannelIndex();
	int si = currentScanIndex();
	if( si<0 || ci<0 )
		return;	// who are you? Don't know which channel/scan to edit.

	QString newExpression = ui_.expressionEdit->text().trimmed();

	AMChannel tempChannel(model_->scanAt(si), "testChannel", newExpression );
	/// is it good? set new value for channel
	if(tempChannel.isValid()) {
		model_->channelAt(si, ci)->setExpression(newExpression);
		ui_.expressionEdit->setText(newExpression);
	}
	/// revert editor to old value of channel
	else {
		/// \todo Play system beep to let them know something went wrong
		ui_.expressionEdit->setText(model_->channelAt(si,ci)->expression());
	}

}

void AMChannelEditor::abortExpressionEdit() {

	if(!editingExpression_)
		return;

	editingExpression_ = false;
	ui_.expressionEdit->setStyleSheet("color: black;");

	int ci = currentChannelIndex();
	int si = currentScanIndex();
	if( si<0 || ci<0 )
		ui_.expressionEdit->setText("");
	else
		ui_.expressionEdit->setText(model_->channelAt(si,ci)->expression());

}


void AMChannelEditor::descriptionEditingFinished() {

}


void AMChannelEditor::populateExpressionMenu(int scanIndex) {

	rawDataMenu_->clear();

	if(scanIndex < 0 || scanIndex >= model_->numScans())
		return;


	QStringList colNames = model_->scanAt(scanIndex)->rawDataColumnNames();
	foreach(QString c, colNames)
		rawDataMenu_->addAction(c);

}

#include <QAction>
void AMChannelEditor::onRawDataMenuTriggered(QAction *action) {
	ui_.expressionEdit->insertPlainText(action->text());
	if(editingExpression_)
		ui_.expressionEdit->setFocus();
}

void AMChannelEditor::onFunctionMenuTriggered(QAction *action) {
	/// \todo: instead of just inserting, wrap the function brackets around the currently selected item, and place the cursor just inside the right bracket.
	ui_.expressionEdit->insertPlainText(action->text());
	if(editingExpression_)
		ui_.expressionEdit->setFocus();
}


void AMChannelEditor::populateFunctionMenu() {


	functionMenu_->addAction("exp()");
	functionMenu_->addAction("ln()");
	functionMenu_->addAction("log()");
	functionMenu_->addSeparator();
	functionMenu_->addAction("sqrt()");
	functionMenu_->addSeparator();
	functionMenu_->addAction("sin()");
	functionMenu_->addAction("cos()");
	functionMenu_->addAction("tan()");
	functionMenu_->addSeparator();
	functionMenu_->addAction("asin()");
	functionMenu_->addAction("acos()");
	functionMenu_->addAction("atan()");

}
