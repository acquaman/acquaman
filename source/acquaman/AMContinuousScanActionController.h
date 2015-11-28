#ifndef AMCONTINUOUSSCANACTIONCONTROLLER_H
#define AMCONTINUOUSSCANACTIONCONTROLLER_H

#include "acquaman/AMScanActionController.h"
#include "acquaman/AMContinuousScanConfiguration.h"
#include "dataman/AMUser.h"
#include "acquaman/AMAgnosticDataAPI.h"

#include <QThread>
#include <QTimer>

class AMScanActionControllerScanAssembler;
class AMListAction3;
class AMDSClientDataRequest;

class CLSAMDSScalerChannelDetector;
class CLSAmptekSDD123DetectorNew;

#define AMCONTINUOUSSCANACTIONCONTROLLER_COULD_NOT_ADD_DETECTOR 285000
#define AMCONTINUOUSSCANACTIONCONTROLLER_REQUIRED_DATA_MISSING  285001
#define AMCONTINUOUSSCANACTIONCONTROLLER_INITIAL_ENCODER_POSITION_MISSING  285002
#define AMCONTINUOUSSCANACTIONCONTROLLER_BAD_SCALER_DATAHOLDER_TYPE  285003
#define AMCONTINUOUSSCANACTIONCONTROLLER_SCALER_CHANNEL_MISMATCH  285004

/// This class is the base class for all continuous based scan controllers.
class AMContinuousScanActionController : public AMScanActionController
{
	Q_OBJECT

public:
	/// Constructor.  Takes an AMContinuousScanConfiguration.
	AMContinuousScanActionController(AMContinuousScanConfiguration *configuration, QObject *parent = 0);
	/// Destructor.
	virtual ~AMContinuousScanActionController();

	/// Method that builds all the general aspects, such as measurements and raw data sources, and the file writer capabilities for the scan controller.
	virtual void buildScanController();

signals:

public slots:

protected slots:
	/// Helper slot that tells AMCDFDataStore to flush it's contents to disk.  This prevents it from corrupting itself.
	void flushCDFDataStoreToDisk();
	/// Slot that disables the connection to onScanningActionsSucceeded() upon startup.
	void onScanningActionsStarted();

protected:
	/// Implementation to ensure that the data acquisition event is caught and handled.
	virtual bool event(QEvent *e);
	/// Creates the scan assembler that builds all the actions used to run the scan.
	virtual void createScanAssembler();

	/// Helper function to place all of the code into the datastore.  Subclasses should re-implement because continuous data has a lot of implementation specific details.
	virtual void onAxisFinished();
	/// Helper method that places data into the appropriate places after a data available message.  Passes the message.  Subclasses should re-implement.
	virtual void fillDataMaps(AMAgnosticDataAPIDataAvailableMessage *message);

	/// Helper function responsible principally responsible for gathering the requiredBufferNames_ and timesScales_. Returns false if an error occurs.
	virtual bool generateAnalysisMetaInfo();

protected:
	/// The assembler that takes in the region scan configuration and turns it into a tree of scanning actions.
	AMScanActionControllerScanAssembler *scanAssembler_;
	/// The pointer to the region scan configuration to provide the subclass information.
	AMContinuousScanConfiguration *continuousConfiguration_;

	/// A timer used when using AMCDFDataStore.  After a timeout it flushes the contents to disk.
	QTimer flushToDiskTimer_;

	/// Map of bufferNames to client data requests
	QMap<QString, AMDSClientDataRequest*> clientDataRequestMap_;
	/// Map of other meta data collected at run time by request
	QMap<QString, double> metaDataMap_;

	/// The insertion index for placing data
	AMnDIndex insertionIndex_;

	/// Holds the list of AMDS buffer names we expect to have
	QList<QString> requiredBufferNames_;
	/// Holds the list of time scales for polling detectors
	QList<int> timeScales_;
	/// Holds a list of scaler channel detectors is any were found
	QList<CLSAMDSScalerChannelDetector*> scalerChannelDetectors_;
	/// Holds a list of amptek detectors is any were found
	QList<CLSAmptekSDD123DetectorNew*> amptekDetectors_;
};

#endif // AMCONTINUOUSSCANACTIONCONTROLLER_H
