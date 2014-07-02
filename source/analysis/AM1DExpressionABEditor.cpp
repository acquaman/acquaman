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


#include "AM1DExpressionABEditor.h"
#include <QToolButton>
#include <QLabel>
#include <QBoxLayout>
#include <QFormLayout>
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
	insertButton_->setText("Insert ");

	expressionEdit_ = new AMWrappingLineEdit();
	expressionEdit_->setFixedHeight(46);
	xExpressionEdit_ = new AMWrappingLineEdit();
	xExpressionEdit_->setFixedHeight(46);

	QLabel* label = new QLabel("expression");
	label->setStyleSheet("font: bold \"Lucida Grande\";\ncolor: rgb(121,121,121);");

	QFormLayout* formLayout = new QFormLayout();
	formLayout->addRow("y values", expressionEdit_);
	QHBoxLayout* hl2 = new QHBoxLayout();
	hl2->addWidget(xExpressionEdit_);
	hl2->addWidget(insertButton_);
	formLayout->addRow("x values", hl2);

	setLayout(formLayout);

	/*
	QVBoxLayout* col1Layout = new QVBoxLayout();
	QHBoxLayout* insertSpacerLayout = new QHBoxLayout();
	insertSpacerLayout->addSpacerItem(new QSpacerItem(2, 2, QSizePolicy::Expanding));
	insertSpacerLayout->addWidget(insertButton_);
	col1Layout->addWidget(label);
	col1Layout->addLayout(insertSpacerLayout);
	QHBoxLayout* mainLayout = new QHBoxLayout();
	mainLayout->addLayout(col1Layout);
	QVBoxLayout* expressionEditorsLayout = new QVBoxLayout();
	expressionEditorsLayout->addWidget(expressionEdit_);
	expressionEditorsLayout->addWidget(xExpressionEdit_);
	mainLayout->addLayout(expressionEditorsLayout);
	setLayout(mainLayout);
	*/


	// create menus for the insert button:
	insertMenu_ = new QMenu(this);
	rawDataMenu_ = insertMenu_->addMenu("Raw Data");
	functionMenu_ = insertMenu_->addMenu("Function");
	insertButton_->setMenu(insertMenu_);
	connect(rawDataMenu_, SIGNAL(triggered(QAction*)), this, SLOT(onRawDataMenuTriggered(QAction*)));
	connect(functionMenu_, SIGNAL(triggered(QAction*)), this, SLOT(onFunctionMenuTriggered(QAction*)));
	populateFunctionMenu();

	// set the initial expression. Note: assuming expressionBlock_ is valid.  Since these editors are created within AM1DExpressionAB::createEditorWidget(), it should never happen that we have an editor created with an invalid expression block.
	expressionEdit_->setText(expressionBlock_->expression());
	xExpressionEdit_->setText(expressionBlock_->xExpression());
	populateExpressionMenu();

	// connect editor widgets to test/apply changes:
	connect(expressionEdit_, SIGNAL(textChanged()), this, SLOT(expressionEditingChanged()));
	connect(xExpressionEdit_, SIGNAL(textChanged()), this, SLOT(xExpressionEditingChanged()));
	connect(expressionEdit_, SIGNAL(editingFinished(int)), this, SLOT(expressionEditingFinished(int)));
	connect(xExpressionEdit_, SIGNAL(editingFinished(int)), this, SLOT(expressionEditingFinished(int)));

}

AM1DExpressionABEditor::~AM1DExpressionABEditor() {

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

void AM1DExpressionABEditor::xExpressionEditingChanged() {
	editingXExpression_ = true;

	if(expressionBlock_->checkExpressionValidity(xExpressionEdit_->text().trimmed()))
		xExpressionEdit_->setStyleSheet("color: rgb(0,128,0);");
	else
		xExpressionEdit_->setStyleSheet("color: red");
}

#include <QTextCursor>
#include <QApplication>
void AM1DExpressionABEditor::finalizeExpressionEdit() {

	if(editingExpression_) {

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
			QApplication::beep();	// let the user know that something went wrong/changes were not applied.
			expressionEdit_->setText(expressionBlock_->expression());
		}
	}

	if(editingXExpression_) {
		editingXExpression_ = false;
		xExpressionEdit_->setStyleSheet("color: black;");

		QString newExpression = xExpressionEdit_->text().trimmed();

		/// is it good? set new value for channel
		if(expressionBlock_->checkExpressionValidity(newExpression)) {
			expressionBlock_->setXExpression(newExpression);
			xExpressionEdit_->setText(newExpression);
		}
		/// revert editor to old value of channel
		else {
			QApplication::beep();	// let the user know that something went wrong/changes were not applied.
			xExpressionEdit_->setText(expressionBlock_->xExpression());
		}
	}
}

void AM1DExpressionABEditor::abortExpressionEdit() {

	if(editingExpression_) {

		editingExpression_ = false;
		expressionEdit_->setStyleSheet("color: black;");

		// restore old/current actual expression in UI editor text box.
		expressionEdit_->setText(expressionBlock_->expression());
	}

	if(editingXExpression_) {
		editingXExpression_ = false;
		xExpressionEdit_->setStyleSheet("color: black;");

		xExpressionEdit_->setText(expressionBlock_->xExpression());
	}
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
	if(editingXExpression_) {
		xExpressionEdit_->insertPlainText(action->text());
		xExpressionEdit_->setFocus();
	}
	else {
		expressionEdit_->insertPlainText(action->text());
		//if(editingExpression_)
			expressionEdit_->setFocus();
	}
}

void AM1DExpressionABEditor::onFunctionMenuTriggered(QAction *action) {
	/// \todo: instead of just inserting, wrap the function brackets around the currently selected item, and place the cursor just inside the right bracket.
	if(editingXExpression_) {
		xExpressionEdit_->insertPlainText(action->text());
		xExpressionEdit_->setFocus();
	}
	else {
		expressionEdit_->insertPlainText(action->text());
		//if(editingExpression_)
			expressionEdit_->setFocus();
	}
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
