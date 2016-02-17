#ifndef SXRMBFOURELEMENTVORTEXDETECTORVIEW_H
#define SXRMBFOURELEMENTVORTEXDETECTORVIEW_H

#include "ui/beamline/AMXRFDetailedDetectorView.h"
#include "beamline/SXRMB/SXRMBFourElementVortexDetector.h"

/// Sublcass that allows for setting the maximum energy and peaking time to the four element vortex detector.
class SXRMBFourElementVortexDetectorView : public AMXRFDetailedDetectorView
{
	Q_OBJECT

public:
	/// Constructor.
	SXRMBFourElementVortexDetectorView(SXRMBFourElementVortexDetector *detector, QWidget *parent = 0);
	/// Destructor.
	virtual ~SXRMBFourElementVortexDetectorView(){}

	/// Re-implementing to add the peaking time spin box.
	virtual void buildDetectorView();

protected slots:
	/// Handles setting the peaking time.
	void onPeakingTimeChanged();

protected:
	/// Implementation method for setMaximumEnergy.  The view handles all the visuals, but if there is something specific that needs to be passed on to the detector or viewing subclass, you can implement that here.
	virtual void setMaximumEnergyImplementation(double energy);

	/// Pointer to the actual VESPERSSingleElementVortexDetector for setting the peaking time and maximum energy.
	SXRMBFourElementVortexDetector *fourElementVortexDetector_;

	/// Spin box for the peaking time.
	QDoubleSpinBox *peakingTimeSpinBox_;
};

#endif // SXRMBFOURELEMENTVORTEXDETECTORVIEW_H
