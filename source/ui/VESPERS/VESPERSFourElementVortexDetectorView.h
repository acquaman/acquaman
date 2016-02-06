#ifndef VESPERSFOURELEMENTVORTEXDETECTORVIEW_H
#define VESPERSFOURELEMENTVORTEXDETECTORVIEW_H

#include "ui/beamline/AMXRFDetailedDetectorView.h"
#include "beamline/VESPERS/VESPERSFourElementVortexDetector.h"

/// Subclass that allows for setting the maximum energy and peaking time to the four element detector.
class VESPERSFourElementVortexDetectorView : public AMXRFDetailedDetectorView
{
	Q_OBJECT

public:
	/// Constructor.
	VESPERSFourElementVortexDetectorView(VESPERSFourElementVortexDetector *detector, QWidget *parent = 0);
	/// Destructor.
	virtual ~VESPERSFourElementVortexDetectorView(){}

	/// Re-implementing to add the peaking time spin box.
	virtual void buildDetectorView();

protected slots:
	/// Handles setting the peaking time.
	void onPeakingTimeChanged();

protected:
	/// Implementation method for setMaximumEnergy.  The view handles all the visuals, but if there is something specific that needs to be passed on to the detector or viewing subclass, you can implement that here.
	virtual void setMaximumEnergyImplementation(double energy);

	/// Pointer to the actual VESPERSSingleElementVortexDetector for setting the peaking time and maximum energy.
	VESPERSFourElementVortexDetector *fourElementVortexDetector_;

	/// Spin box for the peaking time.
	QDoubleSpinBox *peakingTimeSpinBox_;
};

#endif // VESPERSFOURELEMENTVORTEXDETECTORVIEW_H
