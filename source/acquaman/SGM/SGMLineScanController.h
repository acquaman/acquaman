#ifndef SGMLINESCANCONTROLLER_H
#define SGMLINESCANCONTROLLER_H

#include "acquaman/AMGenericContinuousScanController.h"
#include "acquaman/AMAgnosticDataAPI.h"
#include "acquaman/SGM/SGMLineScanConfiguration.h"

#define SGMLINESCANCONTROLLER_COULD_NOT_GENERATE_META_INFO 489100
#define SGMLINESCANCONTROLLER_COULD_NOT_GENERATE_SCALER_MAPS 489101

/// This class sets up, runs, and manages a line scan using the hexapod on the SGM beamline.
class SGMLineScanController : public AMGenericContinuousScanController
{
	Q_OBJECT

public:
	/// Constructor.
	SGMLineScanController(SGMLineScanConfiguration *configuration, QObject *parent = 0);
	/// Destructor.
	virtual ~SGMLineScanController();

signals:

public slots:

protected:
	/// Helper function to place all of the code into the datastore.  Subclasses should re-implement because continuous data has a lot of implementation specific details.
	virtual void onAxisFinished();
	/// Helper method that places data into the appropriate places after a data available message.  Passes the message.  Subclasses should re-implement.
	virtual void fillDataMaps(AMAgnosticDataAPIDataAvailableMessage *message);

	/// Reimplemented to provide actions that will setup the beamline for optimized operation of the XAS scan.
	AMAction3 *createInitializationActions();
	/// Reimplemented to put the beamline in a good state after a scan has been completed.
	AMAction3 *createCleanupActions();

	/// Specific SGM XAS scan configuration.
	SGMLineScanConfiguration *sgmLineConfiguration_;

	/// Map of data recorder values
	QMap<QString, QVector<double> > dataRecorderMap_;
};

#endif // SGMLINESCANCONTROLLER_H
