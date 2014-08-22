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

#ifndef AMSETTINGSVIEW_H
#define AMSETTINGSVIEW_H

#include <QWidget>
#include <QGroupBox>

class QLineEdit;
class QFormLayout;
class QVBoxLayout;
class QPushButton;


/// A class to view the contents of AMUserSettings. Currently the class is read only, no changes are possible. Set up in a group box with "User Settings" as the title. The size fits either the longest string or a QLineEdit of 400 pixels, whichever is smaller.
class AMUserSettingsView : public QGroupBox
{
	Q_OBJECT
public:
	/// Standard constructor
 	virtual ~AMUserSettingsView();
	AMUserSettingsView(QWidget *parent = 0);

protected:
	/// Interface to the userDataFolder
	QLineEdit *userDataFolderLineEdit_;
	/// Interface to the userDatabaseFileName
	QLineEdit *userDatabaseFileNameLineEdit_;
	/// Interface to the remote backup location
	QLineEdit* remoteDataFolderLineEdit_;

	/// Form layout
	QFormLayout *fl_;
};

/// A class to view the contents of AMSettings. Currently only the fileLoaderPluginsFolder can be changed. Set up in a group box with "Public Settings" as the title. The size fits either the longest string or a QLineEdit of 400 pixels, whichever is smaller.
class AMSettingsView : public QGroupBox
{
	Q_OBJECT

public:
	/// Standard constructor
 	virtual ~AMSettingsView();
	AMSettingsView(QWidget *parent = 0);

	/// Returns whether or not there are unsaved changes (see unsavedChanges_ member variable)
	bool hasUnsavedChanges();

public slots:
	/// Applies any changes (that is, saves changes using AMSettings::s()->save() and reloads with AMSettings::s()->load() )
	void applyChanges();
	/// Discard any changes and sets the line edits back to the AMSettings values
	void discardChanges();

signals:
	/// Emitted whenever the value of unsavedChanges changes from true to false or vice versa
	void unsavedChanges(bool hasUnsavedChanges);

protected slots:
	/// Slot connected to each QLineEdit to determine if changes have been made
	void onLineEditsChanged();

protected:
	/// Show event is reimplemented to save the initial state to check against future changes
	virtual void showEvent(QShowEvent *);

	/// Internal function to record the strings in the QLineEdits and reset unsavedChanges
	void storeInitialState();

protected:
	/// Holds a cache of the startup values of the QLineEdits (to check against for future changes)
	QStringList initialLineEdits_;
	/// Holds whether or not there are actual changes (compared to the initialLineEdits)
	bool unsavedChanges_;

	/// Interface to the publicDataFolder string
	QLineEdit *publicDataFolderLineEdit_;
	/// Interface to the publicDatabaseFileName string
	QLineEdit *publicDatabaseFileNameLineEdit_;
	/// Interface to the fileLoaderPluginsFolder string
	QLineEdit *fileLoaderPluginsFolderLineEdit_;

	/// Form layout
	QFormLayout *fl_;
};

/// A collection widget for both the AMSettingsView and the AMUserSettingsView. Might be changed to tabs later, as the settings options expand.
class AMSettingsMasterView : public QWidget
{
	Q_OBJECT
public:
	/// Standard Constructor
 	virtual ~AMSettingsMasterView();
	AMSettingsMasterView(QWidget *parent = 0);

protected slots:
	/// Slot to listen to changes in the unsavedChanges (right now only listening to AMSettingsView)
	void onUnsavedChanges(bool hasUnsavedChanges);
	/// Applies changes but does not close window
	void onApplyButtonClicked();
	/// Discards changes and closes the window
	void onCancelButtonClicked();
	/// Applies changes and closes the window
	void onOkButtonClicked();

protected:
	/// The closeEvent is reimplemented to offer a dialog option to save/discard/cancel if changes have been made
	virtual void closeEvent(QCloseEvent *);

protected:
	/// Instance of AMUserSettingsView
	AMUserSettingsView *userSettingsView_;
	/// Instance of AMSettingsView
	AMSettingsView *settingsView_;

	/// Button to apply changes but not close
	QPushButton *applyButton_;
	/// Button to discard changes and close
	QPushButton *cancelButton_;
	/// Button to apply changes and close
	QPushButton *okButton_;

	/// Vertical box layout
	QVBoxLayout *vl_;
};

#endif // AMSETTINGSVIEW_H
