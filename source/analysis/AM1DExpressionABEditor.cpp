#include "AM1DExpressionABEditor.h"
#include <QToolButton>
#include <QLabel>
#include <QBoxLayout>
#include <QMenu>
#include "ui/AMWrappingLineEdit.h"
#include "analysis/AM1DExpressionAB.h"

AM1DExpressionABEditor::AM1DExpressionABEditor(AM1DExpressionAB* expressionBlock, QWidget* parent) :
	QWidget(parent)
{

	expressionBlock_ = expressionBlock;

	// create GUI objects and layout
	///////////////////////////////
	insertButton_ = new QToolButton();
	insertButton_->setText("Insert");

	expressionEdit_ = new AMWrappingLineEdit();

	QLabel* label = new QLabel("expression");
	label->setStyleSheet("font: bold \"Lucida Grande\";\ncolor: rgb(121,121,121);");

	QVBoxLayout* col1Layout = new QVBoxLayout();
	QHBoxLayout* insertSpacerLayout = new QHBoxLayout();
	insertSpacerLayout->addSpacerItem(new QSpacerItem(2, 2, QSizePolicy::Expanding));
	insertSpacerLayout->addWidget(insertButton_);
	col1Layout->addWidget(label);
	col1Layout->addLayout(insertSpacerLayout);
	QHBoxLayout* mainLayout = new QHBoxLayout();
	mainLayout->addLayout(col1Layout);
	mainLayout->addWidget(expressionEdit_);
	setLayout(mainLayout);


	// create menus for the insert button:
	insertMenu_ = new QMenu(this);
	rawDataMenu_ = insertMenu_->addMenu("Raw Data");
	functionMenu_ = insertMenu_->addMenu("Function");
	insertButton_->setMenu(insertMenu_);
	connect(rawDataMenu_, SIGNAL(triggered(QAction*)), this, SLOT(onRawDataMenuTriggered(QAction*)));
	connect(functionMenu_, SIGNAL(triggered(QAction*)), this, SLOT(onFunctionMenuTriggered(QAction*)));
	populateFunctionMenu();


	// connect editor widgets to test/apply changes:
	connect(expressionEdit_, SIGNAL(textChanged()), this, SLOT(expressionEditingChanged()));
	connect(expressionEdit_, SIGNAL(editingFinished(int)), this, SLOT(expressionEditingFinished(int)));

}

AM1DExpressionABEditor::~AM1DExpressionABEditor() {
	abortExpressionEdit();
}


void AM1DExpressionABEditor::expressionEditingFinished(int reason) {
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


void AM1DExpressionABEditor::expressionEditingChanged() {

	editingExpression_ = true;

	if(expressionBlock_->checkExpressionValidity(expressionEdit_->text().trimmed()))
		expressionEdit_->setStyleSheet("color: rgb(0,128,0);");
	else
		expressionEdit_->setStyleSheet("color: red");

}

#include <QTextCursor>
void AM1DExpressionABEditor::finalizeExpressionEdit() {

	if(!editingExpression_)
		return;

	editingExpression_ = false;
	expressionEdit_->setStyleSheet("color: black;");

	QString newExpression = expressionEdit_->text().trimmed();

	/// is it good? set new value for channel
	if(expressionBlock_->checkExpressionValidity(newExpression)) {
		expressionBlock_->setExpression(newExpression);
		expressionEdit_->setText(newExpression);
	}
	/// revert editor to old value of channel
	else {
		/// \todo Play system beep to let them know something went wrong
		expressionEdit_->setText(expressionBlock_->expression());
	}

}

void AM1DExpressionABEditor::abortExpressionEdit() {

	if(!editingExpression_)
		return;

	editingExpression_ = false;
	expressionEdit_->setStyleSheet("color: black;");

	// restore old/current actual expression in UI editor text box.
	expressionEdit_->setText(expressionBlock_->expression());

}

void AM1DExpressionABEditor::populateExpressionMenu() {

	rawDataMenu_->clear();

	for(int i=0; i<expressionBlock_->inputDataSourceCount(); i++) {
		rawDataMenu_->addAction(expressionBlock_->inputDataSourceAt(i)->name());
	}
	rawDataMenu_->addSeparator();
	for(int i=0; i<expressionBlock_->inputDataSourceCount(); i++) {
		rawDataMenu_->addAction(expressionBlock_->inputDataSourceAt(i)->name() + ".x");	/// \todo Look at what these actually are, by examining the AMAxisInfo settings, instead of just providing "dataSourceName.x".
	}
}

#include <QAction>
void AM1DExpressionABEditor::onRawDataMenuTriggered(QAction *action) {
	expressionEdit_->insertPlainText(action->text());
	if(editingExpression_)
		expressionEdit_->setFocus();
}

void AM1DExpressionABEditor::onFunctionMenuTriggered(QAction *action) {
	/// \todo: instead of just inserting, wrap the function brackets around the currently selected item, and place the cursor just inside the right bracket.
	expressionEdit_->insertPlainText(action->text());
	if(editingExpression_)
		expressionEdit_->setFocus();
}


void AM1DExpressionABEditor::populateFunctionMenu() {

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
