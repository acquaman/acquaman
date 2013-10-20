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


#ifndef SGMSETTINGSMASTERVIEW_H
#define SGMSETTINGSMASTERVIEW_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>

#include <QGroupBox>
#include <QLineEdit>
#include <QFormLayout>
#include "dataman/database/AMDatabase.h"
#include "util/SGM/SGMPluginsLocation.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "application/AMPluginsManager.h"

class SGMPluginsLocationView : public QGroupBox
{
Q_OBJECT

public:
	SGMPluginsLocationView(QWidget *parent = 0);

	/// Returns whether or not there are unsaved changed (see unsavedChanges_ member variable)
	bool hasUnsavedChanges();

public slots:
	/// Applies any changes (this is, saves changes to the SGM Beamline database)
	void applyChanges();
	/// Discards any changes and sets the line edits back to the database values
	void discardChanges();

signals:
	/// Emitted whenever the value unsavedChanges changes from true to false or vice versa
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

	/// Interface to the file loader plugins folder path
	QLineEdit *fileLoaderPluginsFolderLineEdit_;
	/// Interface to the analysis block plugins folder path
	QLineEdit *analysisBlockPluginsFolderLineEdit_;

	QFormLayout *fl_;
};

#include <QList>

class SGMDacqConfigurationFileView : public QGroupBox
{
Q_OBJECT

public:
	SGMDacqConfigurationFileView(QWidget *parent = 0);

	/// Returns whether or not there are unsaved changed (see unsavedChanges_ member variable)
	bool hasUnsavedChanges();

public slots:
	/// Applies any changes (this is, saves changes to the SGM Beamline database)
	void applyChanges();
	/// Discards any changes and sets the line edits back to the database values
	void discardChanges();

signals:
	/// Emitted whenever the value unsavedChanges changes from true to false or vice versa
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

	/// Interface to the line edits
	QList<QLineEdit*> configurationFileLineEdits_;
	QList<int> configurationFileIDs_;

	QFormLayout *fl_;
};

class AMOldDetector;
class SGMDetectorsMasterView : public QGroupBox
{
Q_OBJECT
public:
	SGMDetectorsMasterView(QWidget *parent = 0);

	/// Returns whether or not there are unsaved changed (see unsavedChanges_ member variable)
	bool hasUnsavedChanges() const;

public slots:
	/// Applies any changes (this is, saves changes for this run-time instance)
	void applyChanges();
	/// Discards any changes and sets the line edits back to the last values
	void discardChanges();

signals:
	/// Emitted whenever the value unsavedChanges changes from true to false or vice versa
	void unsavedChanges(bool hasUnsavedChanges) const;

protected slots:
	/// Slot connected to each QCheckBox to determine if changes have been made
	void onCheckBoxesChanged(bool toggled);
	/// Slot connected to each detector to determine the connectivity state
	void onDetectorAvailabilityChanged(AMOldDetector *detector, bool isAvailable);

protected:
	/// Show event is reimplemented to save the initial state to check against future changes
	virtual void showEvent(QShowEvent *);

	/// Internal function to record the strings in the QLineEdits and reset unsavedChanges
	void storeInitialState();

protected:
	QFormLayout *fl_;

	/// Holds a cache of the startup values of the QCheckBoxes (to check against for future changes)
	QList<bool> initialRequiredDetectors_;
	/// Holds whether or not there are actual changes (compared to the initialLineEdits)
	bool unsavedChanges_;
};

class SGMSettingsMasterView : public QWidget
{
Q_OBJECT

public:
	SGMSettingsMasterView(QWidget *parent = 0);

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
	/// Instance of SGMPluginsLocationView
	SGMPluginsLocationView *sgmPluginsLocationView_;
	/// Instance of SGMDacqConfigurationFileView
	SGMDacqConfigurationFileView *sgmDacqConfigurationFileView_;

	/// Instance of SGMDetectorMasterView
	SGMDetectorsMasterView *sgmDetectorsMasterView_;

	/// Button to apply changes but not close
	QPushButton *applyButton_;
	/// Button to discard changes and close
	QPushButton *cancelButton_;
	/// Button to apply changes and close
	QPushButton *okButton_;

	QVBoxLayout *vl_;
};

#endif // SGMSETTINGSMASTERVIEW_H
