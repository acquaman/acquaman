#ifndef VESPERSAPPCONTROLLER_H
#define VESPERSAPPCONTROLLER_H

#include "application/AMAppController.h"

class VESPERSEndstationView;
class XRFFreeRunView;
class VESPERSPersistentView;
class VESPERSXASScanConfigurationView;

class AMGenericScanEditor;

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
	/// Helper slot that builds a generic scan editor for the XAS scan.  \todo this seems like something that should be higher up in the framework.
	void onCurrentScanControllerStarted();

protected:
	/// A pointer to the current XAS scan editor. \todo this seems like something that should be higher up in the framework.
	AMGenericScanEditor *scanControllerActiveEditor_;
};

#endif // VESPERSAPPCONTROLLER_H
