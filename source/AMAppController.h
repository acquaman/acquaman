#ifndef AMAPPCONTROLLER_H
#define AMAPPCONTROLLER_H

#include <QObject>
#include <QUrl>
#include <QList>
#include <QModelIndex>

class AMMainWindow;
class ConnectionSettings;
class SGMSampleTransferView;
class AMSamplePositionView;
class SamplePositions;
class GratingResolution;
class AbsorptionScanController;
class AMXASScanConfigurationHolder;
class AMScanConfigurationView;
class SGMXASScanConfigurationViewer;
class EmissionScanController;
class AMWorkflowManagerView;
class BottomBar;
class AMDataView;
class AMRunExperimentInsert;


class AMGenericScanEditor;


class QMenuBar;
class QMenu;
class QStandardItem;

/// This class takes the role of the main application controller for your particular version of the Acquaman program. It marshalls communication between separate widgets/objects, handles menus and menu actions, and all other cross-cutting issues that don't reside within a specific view or controller.  It creates and knows about all top-level GUI objects, and manages them within an AMMainWindow.
/*! The AMMainWindow class is a reusable GUI framework class that should not contain application-specific code.  Instead, you should modify this class for your specific version of Acquaman.

  \todo Add an API for managing the scan editor windows, opening a new scan editor, checking out which editors are open, which scans they have, etc.

*/
class AMAppController : public QObject
{
Q_OBJECT
public:
	/// This constructor is empty. Call AMAppController::startup() to create all of the application windows, widgets, and data objects that are needed on program startup.
	explicit AMAppController(QObject *parent = 0);
	/// This destructor automatically calls shutdown() if required. (ie: if startup() has run successfully, and shutdown() hasn't been called yet.)
	virtual ~AMAppController();

	/// create and setup all of the application windows, widgets, communication connections, and data objects that are needed on program startup. Returns true on success
	bool startup();

	/// destroy all of the windows, widgets, and data objects created by applicationStartup(). Only call this if startup() has ran successfully.
	void shutdown();

	/// Returns true when the application is starting up (ie: true prior to startup() finishing successfully, false afterwards)
	bool isStarting() { return isStarting_; }
	/// Returns true when the application is in the process of shutting down (ie: as soon as shutdown() has been called)
	bool isShuttingDown() { return isShuttingDown_; }
	/// Returns true when the application is running normally (ie: after startup() finishes succesfully, and before shutdown() is called)
	bool isRunning() { return isStarting_ == false && isShuttingDown_ == false; }


signals:

public slots:

	// These slots are used to tell the app to activate a particular window/widget
	///////////////////////////////

	/// Bring the Workflow view to the front level
	void goToWorkflow();

	///////////////////////////////////

	/// Calling this slot activates the Import Data wizard.
	void onActionImport();

	/// this slot is called when the "add something" button is pushed. For now, it just creates a new experiment. This could be expanded to a drop-down menu that offers creating a new experiment, a new scan, a new run...
	void onAddButtonClicked();


protected slots:

	/// This slot catches changes in the current widget of the AMMainWindow. \c pane is the new current widget.  Insert any widget-specific responses that you need here.
	/*! \note This only applies to panes that are currently docked within the main window.  If a pane has been undocked, no notification will be received when it becomes raised or activated by the user.
	  */
	void onCurrentPaneChanged(QWidget* pane);

	/// This slot is called when a set of items in a data view are activated (for example, by double clicking). We use it to open this items in a new scan editor.
	/*! the \c itemUrls are in the same format as the uri-list mime type used for drag-and-drop: "amd://databaseConnectionName/tableName/id"
	  */
	void onDataViewItemsActivated(const QList<QUrl>& itemUrls);

	/// This slot is called when an 'alias' item is clicked in the sidebar of the main window.  Alias items are links that contain additional information that needs to be delivered to the widget.
	void onMainWindowAliasItemActivated(QWidget* target, const QString& key, const QVariant& value);

	/// This is called when we detect that a new experiment has finished being created. We call it to select that experiment and start editing its name.
	/*! \c index is the model index of the experiment item in the main window's AMWindowPaneModel. */
	void onNewExperimentAdded(const QModelIndex& index);

	/// This is called when the user clicks any of the available "close" buttons in the main window's sidebar. For now, this could involve closing a scan editor window, or deleting an experiment.
	void onWindowPaneCloseButtonClicked(const QModelIndex& index);


	/// \todo comment this
	void onCurrentScanControllerCreated();
	void onCurrentScanControllerDestroyed();
	void onCurrentScanControllerReinitialized(bool removeScan);

protected:
	// UI structure components
	AMMainWindow* mw_;

	// Menus
	QMenuBar* menuBar_;
	QMenu* fileMenu_;

	// Top-level panes in the main window
	SGMSampleTransferView* sampleTransferView_;
	AMSamplePositionView* samplePositionView_;
	AbsorptionScanController* absorptionScanController_;
	AMScanConfigurationView* scanConfigurationView_;
	SGMXASScanConfigurationViewer* xasScanConfigurationViewer_;
	AMXASScanConfigurationHolder* scanConfigurationHolder_;
	AMWorkflowManagerView* workflowManagerView_;
	BottomBar* bottomBar_;
	AMDataView* dataView_;
	AMRunExperimentInsert* runExperimentInsert_;


	/// The parent item of all scan editors we'll placed in the window pane model.
	QStandardItem* scanEditorsParentItem_;

	/// A list of the scan editors we have open. \todo this is currently unused... consider deprecating...
	QList<QWidget*> scanEditors_;

	/// The scan editor belonging to the active scan controller. (0 if not initialized)
	AMGenericScanEditor* scanControllerActiveEditor_;

	// Application state:
	bool isStarting_, isShuttingDown_;
};

#endif // AMAPPCONTROLLER_H
