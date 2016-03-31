#ifndef AMCONTINUOUSSCANACTIONCONTROLLER_H
#define AMCONTINUOUSSCANACTIONCONTROLLER_H

#include "acquaman/AMScanActionController.h"
#include "acquaman/AMContinuousScanConfiguration.h"
#include "dataman/AMUser.h"
#include "acquaman/AMAgnosticDataAPI.h"

#include "acquaman/AMScanActionControllerBasicFileWriter.h"

#include <QThread>
#include <QTimer>

class AMScanActionControllerScanAssembler;
class AMListAction3;

Q_DECLARE_METATYPE(AMScanActionControllerBasicFileWriter::FileWriterError)

#define AMCONTINUOUSSCANACTIONCONTROLLER_COULD_NOT_ADD_DETECTOR			285000
#define AMCONTINUOUSSCANACTIONCONTROLLER_FILE_ALREADY_EXISTS			285001
#define AMCONTINUOUSSCANACTIONCONTROLLER_COULD_NOT_OPEN_FILE			285002
#define AMCONTINUOUSSCANACTIONCONTROLLER_UNKNOWN_FILE_ERROR				285003
#define AMCONTINUOUSSCANACTIONCONTROLLER_FAILE_TO_WRITE_FILE			285004

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
	/// Emitted when we want to write the AMDS client data request to file
	void requestWriteToFile(const AMDSClientDataRequestMap &clientDataRequestMap);

public slots:

protected slots:
	/// Handles dealing with file writer errors.
	void onFileWriterError(AMScanActionControllerBasicFileWriter::FileWriterError error);

	/// Helper slot that tells AMCDFDataStore to flush it's contents to disk.  This prevents it from corrupting itself.
	void flushCDFDataStoreToDisk();
	/// Slot that disables the connection to onScanningActionsSucceeded() upon startup.
	void onScanningActionsStarted();

protected:
	/// Implementation to ensure that the data acquisition event is caught and handled.
	virtual bool event(QEvent *e);
	/// Creates the scan assembler that builds all the actions used to run the scan.
	virtual void createScanAssembler();
	/// Creates the axis order for higher dimensional scans.  The default order is the axis order, but if a different order is desired, then you should reimplement this in subclasses.
	/*!
		Be sure to use scan_->rawData() for access to the scan axes and not the configuration, as they haven't been properly configured by the time this has been called.
	  */
	virtual void createAxisOrderMap();

	/// Helper function to place all of the code into the datastore.  Subclasses should re-implement because continuous data has a lot of implementation specific details.
	virtual void onAxisFinished();
	/// Helper method that places data into the appropriate places after a data available message.  Passes the message.  Subclasses should re-implement.
	virtual void fillDataMaps(AMAgnosticDataAPIDataAvailableMessage *message);	
	/// Helper function to do necessary cleanup on client data requests
	virtual bool cleanupClientDataRequests();

protected:
	/// The assembler that takes in the region scan configuration and turns it into a tree of scanning actions.
	AMScanActionControllerScanAssembler *scanAssembler_;
	/// The pointer to the region scan configuration to provide the subclass information.
	AMContinuousScanConfiguration *continuousConfiguration_;

	/// A timer used when using AMCDFDataStore.  After a timeout it flushes the contents to disk.
	QTimer flushToDiskTimer_;

	/// Map of bufferNames to client data requests
	QMap<QString, AMDSClientDataRequest*> clientDataRequestMap_;
	/// The insertion index for placing data
	AMnDIndex insertionIndex_;
	/// The axis order mapping.  The mapping is <scan axis name, desired index>.  As with any list, the index should be between 0 and scanRank()-1.
	QMap<QString, int> axisOrderMap_;
	/// Holds the list of AMDS buffer names we expect to have
	QList<QString> requiredBufferNames_;
	/// Holds the list of time scales for polling detectors
	QList<int> timeScales_;
	/// Flag for if this scan increases or decreases (based on actual feedbacks)
	bool isUpScan_;


};

#endif // AMCONTINUOUSSCANACTIONCONTROLLER_H
