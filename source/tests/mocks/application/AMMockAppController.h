#ifndef AMMOCKAPPCONTROLLER_H
#define AMMOCKAPPCONTROLLER_H

#include "application/AMAppController.h"

#include "dataman/AMRun.h"

class AMGenericStepScanConfiguration;
class AMGenericStepScanConfigurationView;
class AMScanConfigurationViewHolder3;


/*!
  * Central ui and controller class for the AMMock application.
  */
class AMMockAppController : public AMAppController
{
	Q_OBJECT

public:
	/// Creates an instance of an AMMockAppController. startup() must be called
	/// before the QApplication is executed.
	explicit AMMockAppController(QObject *parent = 0);
	/// Fress the resources associated with the app controller.
	virtual ~AMMockAppController() {}

	/// create and setup all of the application windows, widgets, communication
	/// connections, and data objects that are needed on program startup. Returns
	/// true on success.
	virtual bool startup();

	/// destroy all of the windows, widgets, and data objects created by applicationStartup().
	/// Only call this if startup() has ran successfully.  If reimplementing, must
	/// call the base-class shutdown() as the last thing it does.
	virtual void shutdown();

protected slots:

protected:
	virtual AMFacility facility() const { return clsFacility_; }

	// Things to do on startup.
	/// Registers all of the necessary mock classes.
	void registerClasses();
	/// Check and set up the database for the first time run.
	void setupOnFirstRun();
	/// Sets up all of the exporter options for the various scan types.
	void setupExporterOptions();
	/// Sets up the user interface by specifying the extra pieces that will be
	/// added to the main window.
	void setupUserInterface();
	/// Sets up all of the connections.
	void makeConnections();

protected:
	/// the definition of the current facility
	AMFacility clsFacility_;

	AMGenericStepScanConfiguration* genericStepScanConfiguration_;
	AMGenericStepScanConfigurationView* genericStepScanView_;
	AMScanConfigurationViewHolder3* genericScanViewHolder_;

};

#endif // AMMOCKAPPCONTROLLER_H
