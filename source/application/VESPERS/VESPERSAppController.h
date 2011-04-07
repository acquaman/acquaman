#ifndef VESPERSAPPCONTROLLER_H
#define VESPERSAPPCONTROLLER_H

#include "application/AMAppController.h"

class VESPERSBeamlineView;
class AMFreeRunScanConfigurationViewHolder;
class VESPERSXRFScanConfigurationView;

class VESPERSAppController : public AMAppController {
	Q_OBJECT

public:
	/// This constructor should be empty. Call VESPERSAppController::startup() to create all of the application windows, widgets, and data objects that are needed on program startup.
	explicit VESPERSAppController(QObject* parent = 0);

	/// Destructor
	virtual ~VESPERSAppController() {}

	/// create and setup all of the application windows, widgets, communication connections, and data objects that are needed on program startup. Returns true on success.  If reimplementing, must call the base-class startup() as the first thing it does.
	virtual bool startup();

	/// destroy all of the windows, widgets, and data objects created by applicationStartup(). Only call this if startup() has ran successfully.  If reimplementing, must call the base-class shutdown() as the last thing it does.
	virtual void shutdown();


protected slots:

protected:
	/// This contains the general endstation view.
	VESPERSBeamlineView *vespersView_;
	/// This is a holder for the VESPERSXRFScanConfigurationView for the single element detector.
	AMFreeRunScanConfigurationViewHolder *xrf1EConfigHolder_;
	/// This is a holder for the VESPERSXRFScanConfigurationView for the four element detector.
	AMFreeRunScanConfigurationViewHolder *xrf4EConfigHolder_;
	/// This is the XRF scan configuration.
	VESPERSXRFScanConfigurationView *xrf1EConfigView_;
	/// This is the XRF scan configuration.
	VESPERSXRFScanConfigurationView *xrf4EConfigView_;
};

#endif // VESPERSAPPCONTROLLER_H
