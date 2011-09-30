/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef AMDATAMANAPPCONTROLLER_H
#define AMDATAMANAPPCONTROLLER_H

#include <QObject>
#include <QUrl>
#include <QList>
#include <QModelIndex>

class AMMainWindow;
class AMBottomBar;
class AMDataViewWithActionButtons;
class AMRunExperimentInsert;
class AMGenericScanEditor;
class AMSettingsMasterView;

class QMenuBar;
class QMenu;
class QStandardItem;

/// This class takes the role of the main application controller for your particular version of the Acquaman program. It marshalls communication between separate widgets/objects, handles menus and menu actions, and all other cross-cutting issues that don't reside within a specific view or controller.  It creates and knows about all top-level GUI objects, and manages them within an AMMainWindow.
/// This is the bare bones version of the GUI framework because it has no acquisition code inside and therefore forms the basis of a take home Dataman program for users.  It contains the ability to scan through the database, create experiments, and view scans using the scan editor.
/*! The AMMainWindow class is a reusable GUI framework class that should not contain application-specific code.  Instead, you should subclass this class for your specific version of Acquaman.

  \todo Add an API for managing the scan editor windows, opening a new scan editor, checking out which editors are open, which scans they have, etc.

*/

class AMDatamanAppController : public QObject
{
	Q_OBJECT
public:
	/// This constructor is empty. Call AMDatamanAppController::startup() to create all of the application windows, widgets, and data objects that are needed on program startup.
	explicit AMDatamanAppController(QObject *parent = 0);

	/// This destructor automatically calls shutdown() if required. (ie: if startup() has run successfully, and shutdown() hasn't been called yet.)
	virtual ~AMDatamanAppController();

	/// create and setup all of the application windows, widgets, communication connections, and data objects that are needed on program startup. Returns true on success.  If reimplementing, must call the base-class startup() as the first thing it does.
	virtual bool startup();

	/// destroy all of the windows, widgets, and data objects created by applicationStartup(). Only call this if startup() has ran successfully.  If reimplementing, must call the base-class shutdown() as the last thing it does.
	virtual void shutdown();

	/// Returns true when the application is starting up (ie: true prior to startup() finishing successfully, false afterwards)
	bool isStarting() { return isStarting_; }
	/// Returns true when the application is in the process of shutting down (ie: as soon as shutdown() has been called)
	bool isShuttingDown() { return isShuttingDown_; }
	/// Returns true when the application is running normally (ie: after startup() finishes succesfully, and before shutdown() is called)
	bool isRunning() { return isStarting_ == false && isShuttingDown_ == false; }

signals:

public slots:
	/// Calling this slot activates the Import Data wizard.
	void onActionImport();

	/// Calling this slot activates the Settings View
	void onActionSettings();

	/// this slot is called when the "add something" button is pushed. For now, it just creates a new experiment. This could be expanded to a drop-down menu that offers creating a new experiment, a new scan, a new run...
	void onAddButtonClicked();

	/// Calling this updates the master progress bar
	void onProgressUpdated(double elapsed, double total);


protected slots:

	/// This slot catches changes in the current widget of the AMMainWindow. \c pane is the new current widget.  Re-implement to catch any widget-specific responses that you need here.
	/*! \note This only applies to panes that are currently docked within the main window.  If a pane has been undocked, no notification will be received when it becomes raised or activated by the user.
	  */
	virtual void onCurrentPaneChanged(QWidget* pane);

	/// This slot is called when a set of items in a data view are activated (for example, by double clicking). We use it to open this items in a new scan editor.
	/*! the \c itemUrls are in the same format as the uri-list mime type used for drag-and-drop: "amd://databaseConnectionName/tableName/id"
	  */
	virtual void onDataViewItemsActivated(const QList<QUrl>& itemUrls);
	/// This slot is called when a set of items in a data view are activated (for example, by double clicking). We use it to open this items in a new scan editor.
	/*! the \c itemUrls are in the same format as the uri-list mime type used for drag-and-drop: "amd://databaseConnectionName/tableName/id"
	  */
	virtual void onDataViewItemsActivatedSeparateWindows(const QList<QUrl>& itemUrls);
	/// This slot is called when a set of items in a data view are exported (or, more accurately, request to be exported).  We use it to fire up an AMExportController.
	/*! the \c itemUrls are in the same format as the uri-list mime type used for drag-and-drop: "amd://databaseConnectionName/tableName/id"
	  */
	virtual void onDataViewItemsExported(const QList<QUrl>& itemUrls);

	/// This slot is called when an 'alias' item is clicked in the sidebar of the main window.  Alias items are links that contain additional information that needs to be delivered to the widget.
	virtual void onMainWindowAliasItemActivated(QWidget* target, const QString& key, const QVariant& value);

	/// This is called when we detect that a new experiment has finished being created. We call it to select that experiment and start editing its name.
	/*! \c index is the model index of the experiment item in the main window's AMWindowPaneModel. */
	virtual void onNewExperimentAdded(const QModelIndex& index);

	/// This is called when the user clicks any of the available "close" buttons in the main window's sidebar. For now, this could involve closing a scan editor window, or deleting an experiment.
	virtual void onWindowPaneCloseButtonClicked(const QModelIndex& index);




protected:
	/// UI structure components
	AMMainWindow* mw_;

	/// Menus
	QMenuBar* menuBar_;
	QMenu* fileMenu_;

	/// Top-level panes in the main window
	AMBottomBar* bottomBar_;
	AMDataViewWithActionButtons* dataView_;
	AMRunExperimentInsert* runExperimentInsert_;

	/// Persistent view for AMSettings
	AMSettingsMasterView *settingsMasterView_;

	/// The parent item for all runs and experiments we'll place in the window pane model
	QStandardItem* runsParentItem_, *experimentsParentItem_;

	/// The parent item of all scan editors we'll placed in the window pane model.
	QStandardItem* scanEditorsParentItem_;

	/// Application state:
	bool isStarting_, isShuttingDown_;
};

#endif // AMDATAMANAPPCONTROLLER_H
