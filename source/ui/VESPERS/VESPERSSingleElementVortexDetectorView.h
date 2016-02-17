#ifndef VESPERSSINGLEELEMENTVORTEXDETECTORVIEW_H
#define VESPERSSINGLEELEMENTVORTEXDETECTORVIEW_H

#include "ui/beamline/AMXRFDetailedDetectorView.h"
#include "beamline/VESPERS/VESPERSSingleElementVortexDetector.h"

/// Subclass that allows for setting the maximum energy and peaking time to the single element detector.
class VESPERSSingleElementVortexDetectorView : public AMXRFDetailedDetectorView
{
	Q_OBJECT

public:
	/// Constructor.
	VESPERSSingleElementVortexDetectorView(VESPERSSingleElementVortexDetector *detector, QWidget *parent = 0);
	/// Destructor.
	virtual ~VESPERSSingleElementVortexDetectorView(){}

	/// Re-implementing to add the peaking time spin box.
	virtual void buildDetectorView();

protected slots:
	/// Handles setting the peaking time.
	void onPeakingTimeChanged();

protected:
	/// Implementation method for setMaximumEnergy.  The view handles all the visuals, but if there is something specific that needs to be passed on to the detector or viewing subclass, you can implement that here.
	virtual void setMaximumEnergyImplementation(double energy);

	/// Pointer to the actual VESPERSSingleElementVortexDetector for setting the peaking time and maximum energy.
	VESPERSSingleElementVortexDetector *singleElementVortexDetector_;

	/// Spin box for the peaking time.
	QDoubleSpinBox *peakingTimeSpinBox_;
};

#endif // VESPERSSINGLEELEMENTVORTEXDETECTORVIEW_H
