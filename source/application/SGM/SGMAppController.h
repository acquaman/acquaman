#ifndef SGMAPPCONTROLLER_H
#define SGMAPPCONTROLLER_H

#include "application/AMAppController.h"

class SGMSampleTransferView;
class AMSamplePositionView;
class AMXASScanConfigurationHolder;
class AMFastScanConfigurationHolder;
class AMScanConfigurationView;
class SGMXASScanConfigurationViewer;
class SGMFastScanConfigurationViewer;


class SGMAppController : public AMAppController {
	Q_OBJECT

public:
	/// This constructor should be empty. Call SGMAppController::startup() to create all of the application windows, widgets, and data objects that are needed on program startup.
	explicit SGMAppController(QObject* parent = 0);

	/// Destructor
	virtual ~SGMAppController() {}

	/// create and setup all of the application windows, widgets, communication connections, and data objects that are needed on program startup. Returns true on success.  If reimplementing, must call the base-class startup() as the first thing it does.
	virtual bool startup();

	/// destroy all of the windows, widgets, and data objects created by applicationStartup(). Only call this if startup() has ran successfully.  If reimplementing, must call the base-class shutdown() as the last thing it does.
	virtual void shutdown();


protected slots:
	/// This slot catches changes in the current widget of the AMMainWindow. \c pane is the new current widget.  Re-implement to catch any widget-specific responses that you need here.
	/*! \note This only applies to panes that are currently docked within the main window.  If a pane has been undocked, no notification will be received when it becomes raised or activated by the user.
	  */
	virtual void onCurrentPaneChanged(QWidget* pane);

	/// \todo comment this, David...
	void onCurrentScanControllerCreated();
	void onCurrentScanControllerDestroyed();
	void onCurrentScanControllerReinitialized(bool removeScan);


	/// This is called when the user clicks any of the available "close" buttons in the main window's sidebar. For now, this could involve closing a scan editor window, or deleting an experiment.
	/*! \todo This was REIMPLEMENTED from AMAppController to check if we're trying to close the current scan editor (ie: the one that holds a scan that is currently acquiring). Find a cleaner system to do this, in general. Build into somewhere else?*/
	virtual void onWindowPaneCloseButtonClicked(const QModelIndex& index);

protected:

	SGMSampleTransferView* sampleTransferView_;
	AMSamplePositionView* samplePositionView_;
	AMScanConfigurationView* scanConfigurationView_;
	SGMXASScanConfigurationViewer* xasScanConfigurationViewer_;
	SGMFastScanConfigurationViewer* fastScanConfigurationViewer_;
	AMXASScanConfigurationHolder* scanConfigurationHolder_;
	AMFastScanConfigurationHolder* fastScanConfigurationHolder_;



	/// The scan editor belonging to the active scan controller. (0 if not initialized)
	AMGenericScanEditor* scanControllerActiveEditor_;

};

#endif // SGMAPPCONTROLLER_H
