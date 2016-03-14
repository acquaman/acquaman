#ifndef SGMCONTINUOUSSCANCONTROLLER_H
#define SGMCONTINUOUSSCANCONTROLLER_H

#include "acquaman/AMGenericContinuousScanController.h"

#define SGMCONTINUOUSSCANCONTROLLER_REQUIRED_DATA_MISSING			697301
#define SGMCONTINUOUSSCANCONTROLLER_BAD_SCALER_DATAHOLDER_TYPE		697302
#define SGMCONTINUOUSSCANCONTROLLER_SCALER_CHANNEL_MISMATCH			697303

class CLSAMDSScalerChannelDetector;
class CLSAmptekSDD123DetectorNew;
class SGMContinuousScanController : public AMGenericContinuousScanController
{
    Q_OBJECT
public:
    explicit SGMContinuousScanController(AMGenericContinuousScanConfiguration *configuration,
	                                     QObject *parent = 0);

	virtual ~SGMContinuousScanController() {}
signals:

public slots:
protected:
	/// Helper function responsible principally responsible for gathering the requiredBufferNames_ and timesScales_. Returns false if an error occurs.
	virtual bool generateAnalysisMetaInfo();

	/// Helper function responsible for finding available motion start indices
	virtual bool findStartMotionIndices();

	/// Helper function responsible for generating the axis feedback values
	virtual bool generateAxisFeedbackValues();

	/// Helper function responsible for generating the scalerChannelIndexMap. Returns false if an error occurs.
	virtual bool generateScalerMaps();

	/// Helper function responsible for generating all of the interpolation vectors and parameters
	virtual bool generateInterpolatedParameters();

	/// Helper function responsible for generating all of the interpolated data and placing it in the vectors for the scaler channels
	virtual bool generateInterpolatedScalerVectors();

	/// Helper function responsible for generating all of the interpolated data and placing it in the vectors for the ampteks
	virtual bool generateInterpolatedAmptekVectors();

	/// Helper function to place the interpolated data in the data store
	virtual bool placeInterpolatedDataInDataStore();

	/// Creates the scan assembler used for this scan
	virtual void createScanAssembler();

	/// Map of other meta data collected at run time by request
	QMap<QString, double> metaDataMap_;
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

#endif // SGMCONTINUOUSSCANCONTROLLER_H
