#ifndef AMAPPCONTROLLER_H
#define AMAPPCONTROLLER_H

#include <QObject>
#include <QUrl>
#include <QList>

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

class QStandardItemModel;
//class AMGenericScanEditor;


class QMenuBar;
class QMenu;

/// This class takes the role of the main application controller for your particular version of the Acquaman program. It marshalls communication between separate widgets/objects, handles menus and menu actions, and all other cross-cutting issues that don't reside within a specific view or controller.  It creates and knows about all top-level GUI objects, and manages them within an AMMainWindow.
/*! The AMMainWindow class is a reusable GUI framework class that should not contain application-specific code.  Instead, you should modify this class for your specific version of Acquaman.

  \todo Add an API for managing the scan editor windows, opening a new scan editor, checking out which editors are open, which scans they have, etc.

*/
class AMAppController : public QObject
{
Q_OBJECT
public:
	/// Default construster
	explicit AMAppController(QObject *parent = 0);
	virtual ~AMAppController();

signals:

public slots:

	// These slots are used to tell the app to activate a particular window/widget

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
	ConnectionSettings* connectionSettings_;
	SGMSampleTransferView* sampleTransferView_;
	AMSamplePositionView* samplePositionView_;
	SamplePositions* samplePositions_;
	GratingResolution* gratingResolution_;
	AbsorptionScanController* absorptionScanController_;
	AMScanConfigurationView* scanConfigurationView_;
	SGMXASScanConfigurationViewer* xasScanConfigurationViewer_;
	AMXASScanConfigurationHolder* scanConfigurationHolder_;
	EmissionScanController* emissionScanController_;
	AMWorkflowManagerView* workflowManagerView_;
	BottomBar* bottomBar_;
	AMDataView* dataView_;
	AMRunExperimentInsert* runExperimentInsert_;

//	AMGenericScanEditor* scanEditor_;
	QStandardItemModel* scanEditors_;
};

#endif // AMAPPCONTROLLER_H
