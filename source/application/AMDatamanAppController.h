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


#ifndef AMDATAMANAPPCONTROLLER_H
#define AMDATAMANAPPCONTROLLER_H

#include <QObject>
#include <QUrl>
#include <QList>
#include <QLabel>
#include <QModelIndex>
#include <QStringList>
#include <QProgressBar>

#include "util/AMOrderedSet.h"
#include "util/AMStorageInfo.h"

class AMMainWindow;
class AMBottomPanel;
class AMRunExperimentInsert;
class AMGenericScanEditor;
class AMSettingsMasterView;
class AMGithubIssueSubmissionView;
class AMDatamanStartupSplashScreen;
class AMScanDataView;

class QMenuBar;
class QMenu;
class QAction;
class QStandardItem;

class AMDatabase;
class AMDbUpgrade;
class AMScan;
class AMDirectorySynchronizerDialog;
class AMScanEditorsCloseView;
class AMScanConfigurationView;

#define AMDATAMANAPPCONTROLLER_STARTUP_MESSAGES 42001
#define AMDATAMANAPPCONTROLLER_STARTUP_FINISHED 42002
#define AMDATAMANAPPCONTROLLER_STARTUP_SUBTEXT 42003
#define AMDATAMANAPPCONTROLLER_STARTUP_MODECHANGE 42004

#define AMDATAMANAPPCONTROLLER_DB_UPGRADE_FIRSTTIME_LOAD_FAILURE 270201
#define AMDATAMANAPPCONTROLLER_DB_UPGRADE_FIRSTTIME_REQUIREMENT_FAILURE 270202
#define AMDATAMANAPPCONTROLLER_DB_UPGRADE_FIRSTTIME_SHARED_DB_FAILURE 270230
#define AMDATAMANAPPCONTROLLER_DB_UPGRADE_FIRSTTIME_UPGRADE_TABLE_FAILURE 270203
#define AMDATAMANAPPCONTROLLER_DB_UPGRADE_LOAD_FAILURE 270204
#define AMDATAMANAPPCONTROLLER_DB_UPGRADE_BACKUPS_DIRECTORY_NOT_FOUND 270205
#define AMDATAMANAPPCONTROLLER_DB_UPGRADE_BACKUPS_SUBDIRECTORY_NOT_FOUND 270206
#define AMDATAMANAPPCONTROLLER_DB_UPGRADE_BACKUP_FAILURE 270207
#define AMDATAMANAPPCONTROLLER_DB_UPGRADE_UPGRADE_FAILURE 270208
#define AMDATAMANAPPCONTROLLER_DB_UPGRADE_UPGRADE_TABLE_FAILURE 270209

#define AMDATAMANAPPCONTROLLER_STARTUP_ERROR_LOADING_SETTING 270210
#define AMDATAMANAPPCONTROLLER_STARTUP_ERROR_LOADING_PLUGINS 270211
#define AMDATAMANAPPCONTROLLER_STARTUP_ERROR_HANDLING_FIRST_TIME_USER 270212
#define AMDATAMANAPPCONTROLLER_STARTUP_ERROR_HANDING_NON_FIRST_TIME_USER 270213
#define AMDATAMANAPPCONTROLLER_STARTUP_ERROR_REGISTERING_DATABASES 270214
#define AMDATAMANAPPCONTROLLER_STARTUP_ERROR_POPULATING_NEW_USER_DATABASE 270215
#define AMDATAMANAPPCONTROLLER_STARTUP_ERROR_REVIEWING_EXISTING_USER_DATABASE 270216
#define AMDATAMANAPPCONTROLLER_STARTUP_ERROR_REGISTERING_EXPORTERS 270217
#define AMDATAMANAPPCONTROLLER_STARTUP_ERROR_SETTING_UP_USER_INTERFACE 270218
#define AMDATAMANAPPCONTROLLER_STARTUP_ERROR_INSTALLING_MENU_ACTIONS 270219
#define AMDATAMANAPPCONTROLLER_STARTUP_ERROR_BEFORE_ANYTHING 270220
#define AMDATAMANAPPCONTROLLER_STARTUP_ERROR_BEFORE_USER_INTERFACE 270221
#define AMDATAMANAPPCONTROLLER_STARTUP_ERROR_AFTER_USER_INTERFACE 270222
#define AMDATAMANAPPCONTROLLER_STARTUP_ERROR_AFTER_EVERYTHING 270223

#define AMDATAMANAPPCONTROLLER_DB_UPGRADE_FIRST_TIME_UPGRADES_FAILED 270224
#define AMDATAMANAPPCONTROLLER_DB_UPGRADE_EVERY_TIME_UPGRADES_FAILED 270225

#define AMDATAMANAPPCONTROLLER_USER_SETTINGS_STARTUP_ERROR 270226
#define AMDATAMANAPPCONTROLLER_DATA_DIR_BACKUP_ERROR 270227

#define AMDATAMANAPPCONTROLLER_STARTUP_ERROR_ISFIRSTTIME 270228
#define AMDATAMANAPPCONTROLLER_CANT_CREATE_EXPORT_FOLDER 270229

#define AMDATAMANAPPCONTROLLER_LOCAL_STORAGE_RUNNING_LOW 270230

/// This class takes the role of the main application controller for your particular version of the Acquaman program. It marshalls communication between separate widgets/objects, handles menus and menu actions, and all other cross-cutting issues that don't reside within a specific view or controller.  It creates and knows about all top-level GUI objects, and manages them within an AMMainWindow.
/// This is the bare bones version of the GUI framework because it has no acquisition code inside and therefore forms the basis of a take home Dataman program for users.  It contains the ability to scan through the database, create experiments, and view scans using the scan editor.
/*! The AMMainWindow class is a reusable GUI framework class that should not contain application-specific code.  Instead, you should subclass this class for your specific version of Acquaman.

  \todo Add an API for managing the scan editor windows, opening a new scan editor, checking out which editors are open, which scans they have, etc.

*/

class AMDatamanAppController : public QObject
{
	Q_OBJECT
public:
	/// This constructor contains the list of database upgrades that need to be done at the dataman level, you must modify this list in your AppController. Call AMDatamanAppController::startup() to create all of the application windows, widgets, and data objects that are needed on program startup.
	explicit AMDatamanAppController(QObject *parent = 0);

	/// This destructor is empty.  You should call shutdown() to delete everything and clean up.
	virtual ~AMDatamanAppController() {}

	// 1. Lifecycle control
	//////////////////////////////
public slots:
	/// create and setup all of the application windows, widgets, communication connections, and data objects that are needed on program startup. Returns true on success.  If reimplementing, must call the base-class startup() as the first thing it does, or ensure all base-class functionality is covered:
	/*!
	  0) startupBeforeAnything(): Can be overloaded to add anything prior to startup
	  1) startupLoadSettings(): Load AMSettings and AMUserSettings.
	  2) startupLoadPlugins(): Load Plugins (using the plugin folder paths in AMSettings)
	  3) startupIsFirstTime(): Check if this is the first time a user runs Acquaman (ie: no user database found)
	  3.1) startupOnFirstTime(): If no database, prompt for basic new user information and create+open one
	  3.2) startupOnEveryTime(): If there is a database, open it and check if substantial upgrades are required
	  4) startupRegisterDatabases(): Register the user database and database classes with the AMDbObject system
	  5.1) startupPopulateNewDatabase(): If this is a new database, give a chance to initialize its contents (ex: storing the AMUser, creating facilities, etc.)
	  5.2) startupLoadFromExistingDatabase(): If this is an existing database, give a chance to retrieve information from it into memory (ex: AMUser() object)
	  6) startupRegisterExporters(): Register exporters (\todo Move to plugin system one day?)
	  7) startupBeforeUserInterface(): Can be overloaded to add anything prior to creating the user interface.
	  8) startupCreateUserInterface(): Create main window and setup connections. Can overload and call base implementation first if you want to add anything.
	  9) startupAfterUserInterface(): Can be overloaded to add anything after creating the user interface.
	  10) startupInstallActions(): Install QActions in the menu bar
	  11) startupAfterEverything():

	  All of these components are in separate virtual functions, so that they may be overriden individually.
	  */
	virtual bool startup();
	virtual bool startupBeforeAnything() { return true; }
	virtual bool startupLoadSettings();
	virtual bool startupLoadPlugins();
	virtual bool startupIsFirstTime();
	virtual bool startupOnFirstTime(); ///< Run on first time only
	virtual bool startupOnEveryTime(); ///< Run on every time except the first time
	virtual bool startupCreateDatabases(); ///< Run every time to create the databases (reimplement to create additional databases). This is always called before startupDatabaseUpgrades().
	bool startupDatabaseUpgrades(); ///< Run every time except the first time, to see if non-trivial database upgrades are necessary. This SHOULD NOT BE SUBCLASSED, if you want other upgrades completed, add them to the databaseUpgrades_.
	virtual bool startupBackupDataDirectory();
	virtual bool startupCheckExportDirectory();
	virtual bool startupRegisterDatabases();
	virtual bool startupPopulateNewDatabase(); ///< Run on first time only
	virtual bool startupLoadFromExistingDatabase(); ///< Run on every time except the first time
	virtual bool startupRegisterExporters();
	virtual bool startupBeforeUserInterface()  { return true; }
	virtual bool startupCreateUserInterface();
	virtual bool startupAfterUserInterface();
	virtual bool startupInstallActions();
	virtual bool startupAfterEverything();


	/// destroy all of the windows, widgets, and data objects created by applicationStartup(). Only call this if startup() has ran successfully.  If reimplementing, must call the base-class shutdown() as the last thing it does.
	virtual void shutdown();

	/// This saves all open and modified scans.
	void saveAll();
	/// This checks to see if any open scans have been modified.
	bool anyOpenScansModified() const;

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
	/// Create and add a new scan editor.  Returns the new editor.  Determines whether new new editor should use AMScanView or AM2DScanView based on \param use2DScanView.
	virtual AMGenericScanEditor *createNewScanEditor(bool use2DScanView = false);

	/// If a scan with this \c id and \c database are currently open, returns the editor that has it open. Otherwise returns 0.
	AMGenericScanEditor* isScanOpenForEditing(int id, AMDatabase* db);

	// Convenience functions for database upgrades.
	/// Returns the number of database upgrades.
	int databaseUpgradeCount() const { return databaseUpgrades_.count(); }
	/// Returns the database upgrade at \param index.
	AMDbUpgrade *databaseUpgradeAt(int index) { return databaseUpgrades_.at(index); }
	/// Insert a database upgrade anywhere within the list.  If databaseUpgradeCount() is provided for an index then the upgrade is appended to the end of the list.
	void addDatabaseUpgrade(int index, AMDbUpgrade *upgrade) { databaseUpgrades_.insert(index, upgrade); }
	/// Append a database upgrade to the end of the list.
	void appendDatabaseUpgrade(AMDbUpgrade *upgrade) { databaseUpgrades_.insert(databaseUpgradeCount(), upgrade); }
	/// Prepend a database upgrade to the end of the list.
	void prependDatabaseUpgrade(AMDbUpgrade *upgrade) { databaseUpgrades_.insert(0, upgrade); }
	/// Removes a database upgrade from the list at index \param index.  Returns the database upgrade.
	AMDbUpgrade *removeDatabaseUpgrade(int index) { return databaseUpgrades_.takeAt(index); }

signals:
	/// Notifier that a new generic scan editor has been created. Passes the reference to the new editor.
	void scanEditorCreated(AMGenericScanEditor *);

	/// Notifies that the AMDatamanAppController startup routine is finished
	void datamanStartupFinished();

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
	/// Open a scan specified by a database URL and returns the scan. The scans are checked to make sure that they're not already open, and that they're not still scanning somewhere else. Returns 0 if an error occurred.
	AMScan *dropScanURL(const QUrl& url);

	/// Opens scan configurations by a database URL, based on the view that is stored inside the scan.
	void onLaunchScanConfigurationsFromDb(const QList<QUrl> &urls);

	/// Calling this slot activates the Import Legacy Data wizard.
	void onActionImportLegacyFiles();
	/// Calling this slot activates the Import Acquaman Bundle wizard, to import another acquamn database.
	void onActionImportAcquamanDatabase();

	/// Calling this slot activates the Settings View
	void onActionSettings();

	/// Calling this slot activates the Issue Submission View
	void onActionIssueSubmission();

	/// this slot is called when the "add something" button is pushed. For now, it just creates a new experiment. This could be expanded to a drop-down menu that offers creating a new experiment, a new scan, a new run...
	void onAddButtonClicked();

	/// This slot is called when a menu action requests to export the current Scan Editor's graphics to a file.
	void onActionExportGraphics();
	/// This slot is called when a menu action requests to export the current Scan Editor's graphics to a file.
	void onActionPrintGraphics();

	/// Helps force quit Acquaman by setting a flag to override the check in the event filter for QEvent::Close
	void forceQuitAcquaman();

	/// Opens another database in read-only mode so that you can compare data across databases.
	void onOpenOtherDatabaseClicked();

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

	/// This is called when the user clicks any of the available "close" buttons in the main window's sidebar. For now, this could involve closing a scan editor window, or deleting an experiment.
	virtual void onWindowPaneCloseButtonClicked(const QModelIndex& index);

	/// This is called when the user right clicks any of the items in the main window's sidebar.
	virtual void onWindowPaneRightClicked(const QModelIndex &index, const QPoint &globalPosition);

	/// This is called when the issue submission view is finished and no longer needed
	virtual void onIssueSubmissionViewFinished();

	/// This is called by a signal (chosen with the resetFinishedSignal function) to run when the startup is actually finished. Can be reimplemented in subclasses, but you show call this function in it at some point.
	virtual void onStartupFinished();

	/// This is called when we detect that a new experiment has finished being created. We call it to select that experiment and start editing its name.
	/*! \c index is the model index of the experiment item in the main window's AMWindowPaneModel. */
	virtual void onNewExperimentAdded(const QModelIndex& index);

	/// Slot which shows the application's about page
	void onShowAboutPage();

	/// Handles opening up the AMScanEditorsCloseView
	void openScanEditorsCloseView(AMGenericScanEditor *editor = 0);
	/// Handles request from the AMScanEditorsCloseView to close a list of editors
	void onScanEditorsCloseViewCloseRequested(QList<AMGenericScanEditor*> editorsToClose);
	/// Handles cleaning up the AMScanEditorsCloseView when its closed
	void onScanEditorsCloseViewClosed();

protected:
	/// Whether the application is currently using local storage
	bool usingLocalStorage() const;

	/// Returns whether or not this application intends to use local storage as the default
	bool defaultUseLocalStorage() const;

	/// Sets whether or not this application intends to use local storage as the default
	void setDefaultUseLocalStorage(bool defaultUseLocalStorage);

	/// Helper function to go through all the scan editors and see if we can close all of them.
	bool canCloseScanEditors() const;

	/// Filters the close-event from the main window, to catch anything we need to check before closing the window.
	virtual bool eventFilter(QObject *, QEvent *);

	/// This can be called to reset the signal that will cause the startupFinished slot to run. Pass the object and in something with the signal macro, such as SIGNAL(mySignal(bool itsArgument))
	bool resetFinishedSignal(QObject *sender, const char *signal);


	/// Call this in startupBeforeAnything() if you want to let users choose the AMUserSettings::userDataFolder, before the app starts looking for and loading the database.
	/*! It opens a dialog to choose the folder, and saves that folder back to AMUserSettings::userDataFolder in the acquaman ini file.  If the user cancels the dialog, no changes are made; the existing AMUserSettings::userDataFolder is left alone.

	  \param presentAsParentFolder If you want users to select the actual AMUserSettings::userDataFolder, this should remain false. [The userDataFolder is the folder that contains the user's database file (typically userData.db), and their raw data (typically organized in folders by date).]  Some beamlines have chosen to encapsulate this inside another folder. If you want the user to select the <i>encapsulating</i> folder in the dialog, set \c presentAsParentFolder to true.  A "userData" folder will be created inside the encapsulating folder if it doesn't exist already, and set as the actual AMUserSettings::userDataFolder.
*/
	void getUserDataFolderFromDialog(bool presentAsParentFolder = false);

	/// Method that allows the app controller and all subclasses to have their own specific bottom panel.  This method MUST ensure that the bottomPanel_ member is valid.
	virtual void addBottomPanel();

	/// Method that handles the database upgrades the first when a database has been created.  \param upgrades is the list of upgrades that need to be done.
	bool onFirstTimeDatabaseUpgrade(QList<AMDbUpgrade *> upgrades);
	/// Method that handles the database upgrades for every other time the database is loaded.  \param upgrades is the list of upgrades that need to be done.
	bool onEveryTimeDatabaseUpgrade(QList<AMDbUpgrade *> upgrades);

	/// create scan configuration view from a given database URL.
	AMScanConfigurationView* createScanConfigurationViewFromDb(const QUrl &url);
	/// Opens a single scan configuration from a given database URL.
	virtual void launchScanConfigurationFromDb(const QUrl &url);

protected:
	/// Helper method that returns the editor associated with a scan for the scanEditorsScanMapping list.  Returns 0 if not found.
	AMGenericScanEditor *editorFromScan(AMScan *scan) const;
	/// Helper method that returns the scan associated with an editor for the scanEditorsScanMapping list.  Returns 0 if not found.
	AMScan *scanFromEditor(AMGenericScanEditor *editor) const;

	/// The QObject timer event which handles refreshing the storage info.
	void timerEvent(QTimerEvent *);

	/// Helper function which updates the progress bar with the current usage, if local storage is used.
	void updateStorageProgressBar();

	/// UI structure components
	AMMainWindow* mw_;

	/// Menus
	QMenuBar* menuBar_;
	QMenu *fileMenu_;
	QMenu *viewMenu_;
	QMenu *helpMenu_;
	QProgressBar* internalStorageRemainingBar_;
	/// The action that triggers saving the current AMScanView image.
	QAction* exportGraphicsAction_;
	QAction* printGraphicsAction_;


	/// Top-level panes in the main window
	AMBottomPanel *bottomPanel_;
	AMScanDataView* dataView_;
	AMRunExperimentInsert* runExperimentInsert_;

	/// The startup splash screen for loading
	AMDatamanStartupSplashScreen *splashScreen_;

	/// Persistent view for AMSettings
	AMSettingsMasterView *settingsMasterView_;

	/// Persistent view for AMGithubIssueSubmissionView
	AMGithubIssueSubmissionView *issueSubmissionView_;
	/// Storage for additional issue types and assignees in subclass app controllers
	AMOrderedSet<QString, QString> additionalIssueTypesAndAssignees_;

	/// The parent item for all runs and experiments we'll place in the window pane model
	QStandardItem* runsParentItem_, *experimentsParentItem_;

	/// The parent item of all scan editors we'll place in the window pane model.
	QStandardItem* scanEditorsParentItem_;

	/// Application state:
	bool isStarting_, isShuttingDown_;
	/// This will be set to true if this is the first time a user has run Acquaman
	bool isFirstTimeRun_;

	/// Holds the list of extra databases that have been opened since the application has started.
	QList<AMDatabase *> otherOpenDatabases_;
	/// Holds the list of database upgrades to do in order (holds these as QMetaObjects so they can be new'd at the correct time)
	QList<AMDbUpgrade*> databaseUpgrades_;
	/// Holds a list of additional databases to look in for export options (the "user" database will always be searched")
	QStringList additionalExporterOptionsDatabases_;

	/// Holds a boolean that may be set to warn about poor choices for database directory
	bool isBadDatabaseDirectory_;

	/// List that organizes active scans with editors for updating the ScanEditorModelItem's.  Although the list currently will only be populated AMAppController, the clean up is very general and is done in the onWindowPaneCloseClicked() method.
	QList<QPair<AMScan *, AMGenericScanEditor *> > scanEditorScanMapping_;

	/// Flag for overriding check on eventfilter for QEvent::Close
	bool overrideCloseCheck_;

	/// Flag for letting individual appControllers default the local storage flag for first time users
	bool defaultUseLocalStorage_;

	/// Flag for if the call to startupIsFirstTime() detects an error
	bool firstTimeError_;

	/// Window to handle closing of multiple scan editors
	AMScanEditorsCloseView *scanEditorCloseView_;

	/// The storage info for the volume used to store the data.
	AMStorageInfo storageInfo_;

	/// The timer id of the object. Used to keep a reference to this objects timer
	/// in the event queue, should it need to be stopped.
	int timerIntervalID_;

	/// Used to stop error mons being spammed each time we check the storage usage and find it running out. Iterated each time the storage is checked and it over 85%. A message is displayed every 30 iterations (30 minutes)
	int storageWarningCount_;

	/// Icon label used to warn users the local storage space is running low.
	QLabel* storageWarningLabel_;

private:
	/// Holds the QObject whose signal is currently being used to connect to the onStartupFinished slot
	QObject *finishedSender_;
};

#endif // AMDATAMANAPPCONTROLLER_H
