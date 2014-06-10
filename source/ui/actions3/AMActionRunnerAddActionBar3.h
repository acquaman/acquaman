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


#ifndef AMACTIONRUNNERADDACTIONBAR3_H
#define AMACTIONRUNNERADDACTIONBAR3_H

#include <QWidget>

class AMAction3;
class QPushButton;
class QLabel;
class QRadioButton;
class QVBoxLayout;

/// This widget provides a common, standard set of buttons to show below a typical "action configuration" panel. The buttons let the user run the action immediately (if AMActionRunner isn't already running an action), or add it to the action runner's queue. The buttons are automatically enabled/disabled based on the state of the action runner.
/*! To use this class, you need to re-implement the virtual protected function createAction() to provide the action that should be run/added to the queue.

  This widget leaves room in its layout for any kind action configuration widget to be added directly above it... Call addWidget() to insert that widget.*/

class AMActionRunnerAddActionBar3 : public QWidget
{
	Q_OBJECT
public:
	/// Constructor.  If the main "Start ____" should say something other than "Start Action", you can specify that name here.  (ex: "Scan", "Move", etc.)
 	virtual ~AMActionRunnerAddActionBar3();
	explicit AMActionRunnerAddActionBar3(const QString& actionCategoryName = "Action", QWidget *parent = 0);

	/// Adds a widget to our layout, directly above the buttons.  We take ownership of that \c widget.
	void addWidget(QWidget* widget);

signals:
	/// Emitted to request the main application controller show the workflow pane.
	void showWorkflowRequested();

protected slots:
	/// Called when the "Start Scan" button is clicked
	void onStartActionRequested();
	/// Called when the "Add to Queue" button is clicked.
	void onAddToQueueRequested();

	/// This re-evaluates the state of the workflow and provides user feedback on the "Start [action]" button as required.
	void reviewStartActionButtonState();

protected:
	/// This function is called to get the action that should be added to the queue. The base class implementation returns 0 for no valid action, so it must be re-implemented.
	virtual AMAction3* createAction() { return 0; }

	/// UI elements
	QLabel *whenDoneLabel_;
	QPushButton* startActionButton_, *addToQueueButton_;
	QRadioButton* goToWorkflowOption_, *setupAnotherActionOption_;
	QVBoxLayout* layout_;
	QString actionCategoryName_;

};

#endif // AMACTIONRUNNERADDACTIONBAR3_H
