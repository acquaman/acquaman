/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef AM1DEXPRESSIONABEDITOR_H
#define AM1DEXPRESSIONABEDITOR_H

#include <QWidget>

class QMenu;
class AMWrappingLineEdit;
class QToolButton;
class AM1DExpressionAB;

/// This class provides an editor widget to set the X and Y expressions for an AM1DExpressionAB analysis block. \todo Connect to AMAnalysisBlock::inputDataSourcesChanged() signal, in case the input data sources change while the editor is open.
class AM1DExpressionABEditor : public QWidget
{
	Q_OBJECT
public:
	explicit AM1DExpressionABEditor(AM1DExpressionAB* expressionBlock, QWidget* parent = 0);
	virtual ~AM1DExpressionABEditor();

signals:

public slots:


protected slots:

	// Signals from GUI components to edit the current values
	void expressionEditingFinished(int reason);
	void expressionEditingChanged();
	void xExpressionEditingChanged();

	/// Called when a menu item in the raw data menu is clicked:
	void onRawDataMenuTriggered(QAction* action);
	/// Called when a menu item in the function menu is clicked:
	void onFunctionMenuTriggered(QAction* action);


protected:
	/// Helper function: fills the available options in the 'insert' menu based on the block's input sources.
	void populateExpressionMenu();

	/// Helper function: called once to fill the list of functions.  This should really be tied into the muParser stuff, but it's here for now.
	void populateFunctionMenu();

	/// You can end channel expression editing in two ways: by accepting it (if its valid), or...
	/*! This function is designed to do nothing if a channel expression edit is not in progress.*/
	void finalizeExpressionEdit();
	/// You can end channel expression editing by aborting it completely:
	/*! This function is designed to do nothing if a channel expression edit is not in progress.*/
	void abortExpressionEdit();

	/// Flag to indicate that we're currently editing the expression.
	bool editingExpression_;
	/// Flag to indicate that we're currently editing the x-expression.
	bool editingXExpression_;


	/// Menu containing the insert items for the expression editor
	QMenu* insertMenu_, *rawDataMenu_, *functionMenu_;

	/// UI elements:
	AMWrappingLineEdit* expressionEdit_;
	AMWrappingLineEdit* xExpressionEdit_;
	QToolButton* insertButton_;

	/// The actual AM1DExpressionAB analysis block that we provide editing capability for
	AM1DExpressionAB* expressionBlock_;

};

#endif // AM1DEXPRESSIONABEDITOR_H
