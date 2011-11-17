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

class AMDatabase;

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

	// 1. Lifecycle control
	//////////////////////////////
public slots:
	/// create and setup all of the application windows, widgets, communication connections, and data objects that are needed on program startup. Returns true on success.  If reimplementing, must call the base-class startup() as the first thing it does, or ensure all base-class functionality is covered:
	/*!
	  1) Load AMSettings and AMUserSettings.
	  2) Load Plugins (using the plugin folder paths in AMSettings)
	  3) Check if this is the first time a user runs Acquaman (ie: no user database found)
	  3.1) If no database, prompt for basic new user information and create+open one
	  3.2) If there is a database, open it and check if substantial upgrades are required
	  4) Register the user database and database classes with the AMDbObject system
	  5.1) If this is a new database, give a chance to initialize its contents (ex: storing the AMUser, creating facilities, etc.)
	  5.2) If this is an existing database, give a chance to retrieve information from it into memory (ex: AMUser() object)
	  6) Register exporters (\todo Move to plugin system one day?)
	  7) Create main window and setup connections
	  8) Install QActions in the menu bar

	  All of these components are in separate virtual functions, so that they may be overriden individually.
	  */
	virtual bool startup();
	virtual bool startupBeforeAnything() { return true; }
	virtual bool startupLoadSettings();
	virtual bool startupLoadPlugins();
	virtual bool startupIsFirstTime();
		/// Run on first time only:
		virtual bool startupOnFirstTime();
		/// Run on every time except the first time:
		virtual bool startupOnEveryTime();
		/// Run every time except the first time, to see if non-trivial database upgrades are necessary
		virtual bool startupDatabaseUpgrades();
	virtual bool startupRegisterDatabases();
		/// Run on first time only
		virtual bool startupPopulateNewDatabase();
		/// Run on every time except the first time
		virtual bool startupLoadFromExistingDatabase();
	virtual bool startupRegisterExporters();
	virtual bool startupBeforeUserInterface()  { return true; }
	virtual bool startupCreateUserInterface();
	virtual bool startupAfterUserInterface()  { return true; }
	virtual bool startupInstallActions();
	virtual bool startupAfterEverything()  { return true; }


	/// destroy all of the windows, widgets, and data objects created by applicationStartup(). Only call this if startup() has ran successfully.  If reimplementing, must call the base-class shutdown() as the last thing it does.
	virtual void shutdown();

	// 2. Lifecycle status
	//////////////////////////
public:
	/// Returns true when the application is starting up (ie: true prior to startup() finishing successfully, false afterwards)
	bool isStarting() { return isStarting_; }
	/// Returns true when the application is in the process of shutting down (ie: as soon as shutdown() has been called)
	bool isShuttingDown() { return isShuttingDown_; }
	/// Returns true when the application is running normally (ie: after startup() finishes succesfully, and before shutdown() is called)
	bool isRunning() { return isStarting_ == false && isShuttingDown_ == false; }


	// 3.  Convenience functions to access the open scan editors
	///////////////////////

	/// Number of scan editors currently open
	int scanEditorCount() const;
	/// Access a scan editor
	AMGenericScanEditor* scanEditorAt(int index) const;
	/// Close a scan editor. Returns false if can't be closed.
	bool closeScanEditor(int index);
	/// Close a scan editor. Returns false if can't be closed.
	bool closeScanEditor(AMGenericScanEditor* editor);
	/// Create and add a new scan editor. Returns the new scan editor
	AMGenericScanEditor* createNewScanEditor();

	/// If a scan with this \c id and \c database are currently open, returns the editor that has it open. Otherwise returns 0.
	AMGenericScanEditor* isScanOpenForEditing(int id, AMDatabase* db);


signals:

public slots:
	/// Open a list of scans, specified by a database URL, in the given \c editor. (If \c editor is 0, a new editor will be opened.)  The scans are checked to make sure that they're not already open, and that they're not still scanning somewhere else.
	/*! To open each scan in a new editor, set \c openInIndividualEditors to true. (The \c editor value will be ignored in this case.)

Returns true if at least one scan was opened successfully.

The Drag is accepted when:

	  - Drag Action = Qt::CopyAction
	  - One of the MIME types is "text/uri-list"... format is "amd://databaseConnectionName/tableName/id"
	  - There is at least one URL in the uri-list
	  - The URL scheme is "amd://"
	  - The database connection name returns a valid database, according to AMDatabase::dbByName(connectionName)
	  - The table is the main Objects table
	  - The id of the item can be found in the table
	  */
	bool dropScanURLs(const QList<QUrl>& urls, AMGenericScanEditor* editor = 0, bool openInIndividualEditors = false);
	/// Open a scan specified by a database URL, in the given \c editor. (If \c editor is 0, a new editor will be opened.)  The scans are checked to make sure that they're not already open, and that they're not still scanning somewhere else. Returns true if the scan was opened successfully.
	bool dropScanURL(const QUrl& url, AMGenericScanEditor* editor = 0);


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
	/// Helper function to go through all the scan editors and see if we can close all of them.
	bool canCloseScanEditors() const;

	/// Filters the close-event from the main window, to catch anything we need to check before closing the window.
	bool eventFilter(QObject *, QEvent *);

//	/// Helper function to process all events for \c ms milliseconds, but stay in the calling function.
//	void processEventsFor(int ms);





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

	/// The parent item of all scan editors we'll place in the window pane model.
	QStandardItem* scanEditorsParentItem_;

	/// Application state:
	bool isStarting_, isShuttingDown_;
	/// This will be set to true if this is the first time a user has run Acquaman
	bool isFirstTimeRun_;
};

#endif // AMDATAMANAPPCONTROLLER_H
