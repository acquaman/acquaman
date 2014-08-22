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


#ifndef AMSAMPLEPLATEBROWSERVIEW_H
#define AMSAMPLEPLATEBROWSERVIEW_H

#include <QWidget>
#include <QDialog>
#include <QModelIndex>

class QListView;
class QLineEdit;
class QLabel;
class QPushButton;

class AMSamplePlate;
class AMSamplePlateBrowser;

class AMSamplePlateBrowserView : public QWidget
{
Q_OBJECT
public:
	/// View for an AMSamplePlateBrowser
	AMSamplePlateBrowserView(AMSamplePlateBrowser *samplePlateBrowser, QWidget *parent = 0);

public slots:
	/// Called to clear the selection (when we reset the window)
	void clearViewSelection();

signals:
	/// Emitted when we've selected a valid sample plate
	void samplePlateSelected(AMSamplePlate *samplePlate);


protected slots:
	/// Handles the select button being clicked
	void onSelectButtonClicked();

	/// Handles updating the enabled state and selectionLabel text when we change which item is selected
	void onSelectionModelCurrentChanged(const QModelIndex &current, const QModelIndex &previous);

	/// Forwards double clicks direclty to onSelectButtonClicked()
	void onSamplePlatesListViewDoubleClicked(const QModelIndex &clicked);

protected:
	/// Handles closing the window if Qt::Key_Escape is pressed
	void keyPressEvent(QKeyEvent *event);

protected:
	/// Our underlying model
	AMSamplePlateBrowser *samplePlateBrowser_;
	/// List view for all of the sample plates
	QListView *samplePlatesListView_;
	/// Tells us what sample plate we're about to select
	QLabel *selectionLabel_;
	/// Clicked to select a new plate
	QPushButton *selectButton_;
	/// Cancels out and makes no changes
	QPushButton *cancelButton_;
};

class AMSamplePlateCreationDialog : public QDialog
{
Q_OBJECT
public:
	/// Simple dialog for creating a new sample plate
	AMSamplePlateCreationDialog(QWidget *parent = 0);

	/// After the dialog is successfull we can still query it for the new sample plate's name
	QString samplePlateName() const;

protected slots:
	/// Handles making sure that the sample plate name is valid
	void onSamplePlateNameLineEditTextChanged(const QString &name);

protected:
	virtual void keyPressEvent(QKeyEvent *event);

protected:
	/// Where the sample plate name is typed
	QLineEdit *samplePlateNameLineEdit_;
	/// Accept the dialog
	QPushButton *okButton_;
	/// Cancel and reject changes
	QPushButton *cancelButton_;
};

#endif // AMSAMPLEPLATEBROWSERVIEW_H
