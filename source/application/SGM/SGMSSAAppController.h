#ifndef SGMAPPCONTROLLER_H
#define SGMAPPCONTROLLER_H

#include "application/AMAppController.h"

class SGMSampleTransferView;
class AMSampleManagementWidget;
class SGMSampleManipulatorView;
class SGMSidebar;


class SGMSSAAppController : public AMAppController {
	Q_OBJECT

public:
	/// This constructor should be empty. Call SGMAppController::startup() to create all of the application windows, widgets, and data objects that are needed on program startup.
	explicit SGMSSAAppController(QObject* parent = 0);

	/// Destructor
	virtual ~SGMSSAAppController() {}

	/// create and setup all of the application windows, widgets, communication connections, and data objects that are needed on program startup. Returns true on success.  If reimplementing, must call the base-class startup() as the first thing it does.
	virtual bool startup();

	/// destroy all of the windows, widgets, and data objects created by applicationStartup(). Only call this if startup() has ran successfully.  If reimplementing, must call the base-class shutdown() as the last thing it does.
	virtual void shutdown();


protected slots:
	/// This slot catches changes in the current widget of the AMMainWindow. \c pane is the new current widget.  Re-implement to catch any widget-specific responses that you need here.
	/*! \note This only applies to panes that are currently docked within the main window.  If a pane has been undocked, no notification will be received when it becomes raised or activated by the user.
	  */
	virtual void onCurrentPaneChanged(QWidget* pane);

	void onSGMBeamlineConnected();

protected:

	SGMSampleTransferView* sampleTransferView_;
	//AMSampleManagementWidget* samplePositionView_;
	SGMSampleManipulatorView* sampleManipulatorView_;
	SGMSidebar* sgmSidebar_;
};

#endif // SGMAPPCONTROLLER_H
