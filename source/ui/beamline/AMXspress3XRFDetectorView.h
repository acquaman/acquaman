#ifndef AMXSPRESS3XRFDETECTORVIEW_H
#define AMXSPRESS3XRFDETECTORVIEW_H

#include "ui/beamline/AMXRFDetailedDetectorView.h"
#include "beamline/AMXspress3XRFDetector.h"

class AMXspress3XRFDetectorView : public AMXRFDetailedDetectorView
{
	Q_OBJECT

public:
	/// Constructor.
	AMXspress3XRFDetectorView(AMXspress3XRFDetector *detector, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMXspress3XRFDetectorView(){}

	/// Re-implementing but still going to use the base class buildDetectorView since this view is merely adding to it.
	virtual void buildDetectorView();

protected slots:
	/// Updates the initialization status label.
	void initializationStatusUpdate();
	/// Updates the acquisition cleanup label.
	void acquisitionStatusUpdate();
	/// Updates the status label when the detector initialization state changes.  By default does nothing.
	virtual void onInitializationStateChanged(AMDetector::InitializationState state);
	/// Updates the status label when the detector acquisition state changes.
	virtual void onAcquisitionStateChanged(AMDetector::AcqusitionState state);

	/// Pushes the threshold to the detector.
	void setThreshold();
	/// Updates the threshold spin box.
	void updateThresholdSpinBox();
	/// Updates the number of frames we're at in the current acquisition.
	void onFramesChanged();
	/// Updates the frames per acquisition spin box.
	void updateFramesPerAcquisiton();
	/// Sets a new value for the number of frames to the detector.
	void onFramesPerAcquisitionSpinBoxChanged();

protected:
	/// Helper method that determines the state of the status label and ability to use the start/stop buttons.
	void updateStatusAndAcquisitionButtons();
	/// Returns a pointer to the detector as a BioXAS32ElementGeDetector rather than just an AMXRFDetector.
	AMXspress3XRFDetector *xspress3Detector() { return qobject_cast<AMXspress3XRFDetector *>(detector_); }

	/// The status message label.
	QLabel *statusMessageLabel_;
	/// The initialization status
	QLabel *initializationLabel_;
	/// The acquisition status.
	QLabel *acquisitionStatus_;
	/// The initialize button.
	QPushButton *initializeButton_;
	/// The frames label.
	QLabel *framesLabel_;
	/// The spin box that sets the number of frames per acquisition.
	QSpinBox *framesPerAcquisitionSpinBox_;

	/// Spin box that holds the threshold voltage.
	QSpinBox *thresholdSpinBox_;
	/// Button for disarming the detector.
	QPushButton *disarmButton_;
};

#endif // AMXSPRESS3XRFDETECTORVIEW_H
