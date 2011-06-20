#ifndef VESPERSDATAMANAPPCONTROLLER_H
#define VESPERSDATAMANAPPCONTROLLER_H

#include "application/AMDatamanAppController.h"

class VESPERSDatamanAppController : public AMDatamanAppController
{
	Q_OBJECT

public:
	/// This constructor should be empty. Call VESPERSAppController::startup() to create all of the application windows, widgets, and data objects that are needed on program startup.
	explicit VESPERSDatamanAppController(QObject *parent = 0);

	/// Destructor
	virtual ~VESPERSDatamanAppController() {}

	/// create and setup all of the application windows, widgets, communication connections, and data objects that are needed on program startup. Returns true on success.  If reimplementing, must call the base-class startup() as the first thing it does.
	virtual bool startup();

	/// destroy all of the windows, widgets, and data objects created by applicationStartup(). Only call this if startup() has ran successfully.  If reimplementing, must call the base-class shutdown() as the last thing it does.
	virtual void shutdown() {}
};

#endif // VESPERSDATAMANAPPCONTROLLER_H
