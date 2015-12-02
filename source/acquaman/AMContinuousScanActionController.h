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
class AMDSClientDataRequest;

class CLSAMDSScalerChannelDetector;
class CLSAmptekSDD123DetectorNew;

Q_DECLARE_METATYPE(AMScanActionControllerBasicFileWriter::FileWriterError)

#define AMCONTINUOUSSCANACTIONCONTROLLER_COULD_NOT_ADD_DETECTOR 285000
#define AMCONTINUOUSSCANACTIONCONTROLLER_REQUIRED_DATA_MISSING  285001
#define AMCONTINUOUSSCANACTIONCONTROLLER_INITIAL_ENCODER_POSITION_MISSING  285002
#define AMCONTINUOUSSCANACTIONCONTROLLER_BAD_SCALER_DATAHOLDER_TYPE  285003
#define AMCONTINUOUSSCANACTIONCONTROLLER_SCALER_CHANNEL_MISMATCH  285004

#define AMCONTINUOUSSCANACTIONCONTROLLER_FILE_ALREADY_EXISTS 285006
#define AMCONTINUOUSSCANACTIONCONTROLLER_COULD_NOT_OPEN_FILE 285007
#define AMCONTINUOUSSCANACTIONCONTROLLER_UNKNOWN_FILE_ERROR 285008
#define AMCONTINUOUSSCANACTIONCONTROLLER_FAILE_TO_WRITE_FILE 285009

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

	/// Helper function responsible principally responsible for gathering the requiredBufferNames_ and timesScales_. Returns false if an error occurs.
	virtual bool generateAnalysisMetaInfo();

	/// Helper function responsible for generating the scalerChannelIndexMap. Returns false if an error occurs.
	virtual bool generateScalerMaps();

	/// Helper function responsible for finding available motion start indices
	virtual bool findStartMotionIndices();

	/// Helper function responsible for generating the axis feedback values
	virtual bool generateAxisFeedbackValues();

	/// Helper function responsible for generating all of the interpolation vectors and parameters
	virtual bool generateInterpolatedParameters();
	/// Helper function responsible for generating all of the interpolated data and placing it in the vectors for the scaler channels
	virtual bool generateInterpolatedScalerVectors();
	/// Helper function responsible for generating all of the interpolated data and placing it in the vectors for the ampteks
	virtual bool generateInterpolatedAmptekVectors();

	/// Helper function to place the interpolated data in the data store
	virtual bool placeInterpolatedDataInDataStore();

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
	/// Map of other meta data collected at run time by request
	QMap<QString, double> metaDataMap_;
	/// The axis order mapping.  The mapping is <scan axis name, desired index>.  As with any list, the index should be between 0 and scanRank()-1.
	QMap<QString, int> axisOrderMap_;

	/// The insertion index for placing data
	AMnDIndex insertionIndex_;

	/// Holds the list of AMDS buffer names we expect to have
	QList<QString> requiredBufferNames_;
	/// Holds the list of time scales for polling detectors
	QList<int> timeScales_;
	/// The maximum value in the previous list, for quick lookup
	int largestBaseTimeScale_;
	/// Holds a list of scaler channel detectors is any were found
	QList<CLSAMDSScalerChannelDetector*> scalerChannelDetectors_;
	/// Holds a list of amptek detectors is any were found
	QList<CLSAmptekSDD123DetectorNew*> amptekDetectors_;

	/// Holds the total amount of scaler data received
	int scalerTotalCount_;
	/// Holds the mapping from scaler channel index to scaler channel detector name
	QMap<int, QString> scalerChannelIndexMap_;

	/// Calculated expected duration from the base time scale used
	int expectedDurationScaledToBaseTimeScale_;
	/// A map of detectors (by name) to the start motion index each is using
	QMap<QString, int> detectorStartMotionIndexMap_;

	/// A map of the scaler channel detectors (by name) to each rebased vector
	QMap<QString, QVector<qint32> > scalerChannelRebaseVectors_;
	/// One of the scaler vectors that might have our movement data
	QVector<qint32> encoderUpVector_;
	/// The other scaler vector that might have our movement data
	QVector<qint32> encoderDownVector_;
	/// Convenience location to quickly call for the start index of scaler motion
	int scalerInitiateMovementIndex_;

	/// Vector of the actual recorded axis values
	QVector<double> axisFeedbackValues_;

	/// The interpolation resolution step
	double resolutionStep_;
	/// The size of the interpolated vectors, calculated from data available and parameters
	double interpolatedSize_;
	/// Flag for if this scan increases or decreases (based on actual feedbacks)
	bool isUpScan_;
	/// The vector of interpolated axis values
	QVector<double> interpolatedAxis_;
	/// The vector of interpolated midpoints (for binning interpolation)
	QVector<double> interpolatedMidpoints_;
	/// The vector that maps the midpoints above to fractional indices
	QVector<double> interpolatedMidpointsMappingIndices_;

	/// The actual interpolated data for the scaler channels
	QMap<QString, QVector<double> > interpolatedScalerChannelVectors_;
	/// The actual interpolated data for the ampteks
	QMap<QString, QVector<double> > interpolatedAmptekVectors_;
};

#endif // AMCONTINUOUSSCANACTIONCONTROLLER_H
