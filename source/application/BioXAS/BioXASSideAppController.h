#ifndef BIOXASSIDEAPPCONTROLLER_H
#define BIOXASSIDEAPPCONTROLLER_H

#include "application/AMAppController.h"

class CLSSIS3820ScalerView;
class BioXASSidePersistentView;
class BioXASSideXASScanConfiguration;
class BioXASSideXASScanConfigurationView;
class AMScanConfigurationViewHolder3;

class BioXASSideAppController : public AMAppController
{
	Q_OBJECT

public:
	/// This constructor should be empty.  Call BioXASSideAppController::start() to create the object.
	explicit BioXASSideAppController(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSideAppController() {}

	/// create and setup all of the application windows, widgets, communication connections, and data objects that are needed on program startup. Returns true on success.  If reimplementing, must call the base-class startup() as the first thing it does.
	virtual bool startup();

	/// destroy all of the windows, widgets, and data objects created by applicationStartup(). Only call this if startup() has ran successfully.  If reimplementing, must call the base-class shutdown() as the last thing it does.
	virtual void shutdown();

protected slots:
    void onScalerConnected();
    void onBeamlineConnected();

protected:
	/// Implementation method that individual applications can flesh out if extra setup is required when a scan action is started.  This is not pure virtual because there is no requirement to do anything to scan actions.
	virtual void onCurrentScanActionStartedImplementation(AMScanAction *action);
	/// Implementation method that individual applications can flesh out if extra cleanup is required when a scan action finishes.  This is not pure virtual because there is no requirement to do anything to scan actions.
	virtual void onCurrentScanActionFinishedImplementation(AMScanAction *action);

	// Things to do on startup.
	/// Registers all of the necessary classes that are VESPERS specific.
	void registerClasses();
	/// Sets up all of the exporter options for the various scan types.
	void setupExporterOptions();
	/// Sets up the user interface by specifying the extra pieces that will be added to the main window.
	void setupUserInterface();
	/// Sets up all of the connections.
	void makeConnections();    

protected:
    /// View for the BioXAS Side scaler.
    CLSSIS3820ScalerView *scalerView_;

    BioXASSidePersistentView *persistentPanel_;

    BioXASSideXASScanConfiguration *configuration_;

    BioXASSideXASScanConfigurationView *configurationView_;

    AMScanConfigurationViewHolder3 *configurationViewHolder_;


};

#endif // BIOXASSIDEAPPCONTROLLER_H
