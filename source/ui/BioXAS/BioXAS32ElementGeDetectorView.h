#ifndef BIOXAS32ELEMENTGEDETECTORVIEW_H
#define BIOXAS32ELEMENTGEDETECTORVIEW_H

#include "ui/beamline/AMXRFBaseDetectorView.h"

#include "beamline/BioXAS/BioXAS32ElementGeDetector.h"

class BioXAS32ElementGeDetectorView : public AMXRFBaseDetectorView
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

protected:
	/// Returns a pointer to the detector as a BioXAS32ElementGeDetector rather than just an AMXRFDetector.
	BioXAS32ElementGeDetector *bioXAS32ElementGeDetector() { return qobject_cast<BioXAS32ElementGeDetector *>(detector_); }

	/// The status message label.
	QLabel *statusMessageLabel_;
	/// The initialization status
	QLabel *initializationLabel_;
	/// The acquisition status.
	QLabel *acquisitionStatus_;
};

#endif // BIOXAS32ELEMENTGEDETECTORVIEW_H
