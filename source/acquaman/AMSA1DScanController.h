#ifndef AMSA1DSCANCONTROLLER_H
#define AMSA1DSCANCONTROLLER_H

#include "acquaman/AMScanController.h"

class AMControl;
class AMSADetector;
class AMRegionScanConfiguration;
class AMInternalControlMoveAction;

class AMSA1DScanController : public AMScanController
{
    Q_OBJECT
public:
	AMSA1DScanController(AMControl* control, QList<AMSADetector*> detectors, AMRegionScanConfiguration* scanConfig, QObject* parent = 0);
	virtual ~AMSA1DScanController();

	bool setControl(AMControl* control);
	bool setDetectors(const QList<AMSADetector*>& detectors);



signals:

public slots:

protected:
	// Re-implemented functions
	/////////////////////////////

	/// Implement to initialize the beamline and prepare the scan to run.  (ie: transition from Constructed to Initialized). Returns whether or not initialization can occur, not if it is initialized. If you return true, scan object should be a valid object at this point.  After initialization is complete, call setInitialized().
	virtual bool initializeImplementation();
	/// Implement to start a scan (ie: transition from Initialized to Running).  Returns whether or not the scan can be started, not that the scan has started. After the scan is running, call setStarted().
	virtual bool startImplementation();

//	/// Implement to define whether or not your scan controller is capable of pausing (defaults to not able to pause). If you canPause (return true), then you need to implement a pauseImplementation and a resumeImplementation.
//	virtual bool canPause() const { return false; }
//	/// Implement to pause a running scan (ie: transition from Running to Paused).  After the scan is paused, call setPaused(). (Note: If it's not possible to pause this type of scan (ie: canPause() is false), this will never be called.)
//	virtual void pauseImplementation() {}
//	/// Implement to resume a paused scan (ie: transition from Paused to Running).  If pauseImplementation() returns false, you don't need to do anything here. After the scan is resumed, call setResumed().
//	virtual void resumeImplementation() {}

	/// Implement to cancel a scan. This could be called from any state.  After the scan is cancelled, call setCancelled().
	virtual void cancelImplementation();


	// New functions:
	////////////////////////////

	/// The scan controller creates measurements for each detector in the data store. Re-implement this if you want to control which raw data sources are added to the scan. The base class creates one raw data source for each detector.
	virtual void createRawDataSources();
	/// Re-implement this if you want to add analysis blocks to the scan after it's created. The base class does not create any.
	virtual void createAnalysisBlocks() {}


	/// Start moving to the next measurement position
	void doNextPosition();
	/// Once in position, trigger the next acquisition (from each detector).
	void doNextAcquisition();
	/// Returns the control position for the currentRegion_ and currentStepInRegion_
	double currentPosition() const;

protected slots:
	void onMoveActionSucceeded();
	void onMoveActionFailed();
	void onMoveActionStateChangedWhileCancelling();

	void onDetectorInitialized(bool succeeded = true);
	void onDetectorAcquisitionFinished(bool succeeded = true);
	void onAllAcquisitionFinished();


protected:
	AMControl* control_;
	QList<AMSADetector*> detectors_;
	AMRegionScanConfiguration* regionScanConfig_;


	AMInternalControlMoveAction* moveAction_;

	int currentRegion_;
	int currentStepInRegion_;

	bool initializingDetectors_;
	bool triggeringDetectors_;

	bool detectorInitializationFailed_;
	bool detectorAcquisitionFailed_;

};

#endif // AMSA1DSCANCONTROLLER_H
