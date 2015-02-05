#ifndef BIOXAS32ELEMENTGEDETECTORVIEW_H
#define BIOXAS32ELEMENTGEDETECTORVIEW_H

#include "ui/beamline/AMXRFDetailedDetectorView.h"

#include "beamline/BioXAS/BioXAS32ElementGeDetector.h"

class BioXAS32ElementGeDetectorView : public AMXRFDetailedDetectorView
{
	Q_OBJECT

public:
	/// Constructor.
	BioXAS32ElementGeDetectorView(BioXAS32ElementGeDetector *detector, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXAS32ElementGeDetectorView(){}

	/// Re-implementing but still going to use the base class buildDetectorView since this view is merely adding to it.
	virtual void buildDetectorView();

protected slots:
	void initializationStatusUpdate();
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
	BioXAS32ElementGeDetector *bioXAS32ElementGeDetector() { return qobject_cast<BioXAS32ElementGeDetector *>(detector_); }

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

#endif // BIOXAS32ELEMENTGEDETECTORVIEW_H
