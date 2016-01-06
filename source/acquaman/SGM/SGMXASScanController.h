#ifndef SGMXASSCANCONTROLLER_H
#define SGMXASSCANCONTROLLER_H

#include "acquaman/AMGenericContinuousScanController.h"
#include "acquaman/AMAgnosticDataAPI.h"
#include "acquaman/SGM/SGMXASScanConfiguration.h"
#include "beamline/SGM/energy/SGMGratingSupport.h"

/// This class sets up, runs and manages an XAS continuous scan for the SGM beamline.
class SGMXASScanController : public AMGenericContinuousScanController
{
	Q_OBJECT

public:
	/// Constructor.  Takes an SGMXASScanConfiguration.
	SGMXASScanController(SGMXASScanConfiguration *configuration, QObject *parent = 0);
	/// Destructor.
	virtual ~SGMXASScanController();

signals:

public slots:

protected:
	/// Helper function to place all of the code into the datastore.  Subclasses should re-implement because continuous data has a lot of implementation specific details.
	virtual void onAxisFinished();
	/// Helper method that places data into the appropriate places after a data available message.  Passes the message.  Subclasses should re-implement.
	virtual void fillDataMaps(AMAgnosticDataAPIDataAvailableMessage *message);

	/// Helper function responsible for generating the axis feedback values
	virtual bool generateAxisFeedbackValues();

	/// Reimplemented to provide actions that will setup the beamline for optimized operation of the XAS scan.
	AMAction3 *createInitializationActions();
	/// Reimplemented to put the beamline in a good state after a scan has been completed.
	AMAction3 *createCleanupActions();

protected:
	/// Specific SGM XAS scan configuration.
	SGMXASScanConfiguration *sgmXASConfiguration_;
};

#endif // SGMXASSCANCONTROLLER_H
