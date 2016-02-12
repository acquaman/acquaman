#ifndef VESPERS13ELEMENTGEDETECTORVIEW_H
#define VESPERS13ELEMENTGEDETECTORVIEW_H

#include "ui/beamline/AMXRFDetailedDetectorView.h"

#include "beamline/VESPERS/VESPERS13ElementGeDetector.h"

/// Subclass that allows for setting the peaking time to the 13 element detector.
class VESPERS13ElementGeDetectorView : public AMXRFDetailedDetectorView
{
	Q_OBJECT

public:
	/// Constructor.
	VESPERS13ElementGeDetectorView(VESPERS13ElementGeDetector *detector, QWidget *parent = 0);
	/// Destructor.
	virtual ~VESPERS13ElementGeDetectorView(){}

	/// Re-implementing to add the peaking time spin box.
	virtual void buildDetectorView();

protected slots:
	/// Handles setting the peaking time.
	void onPeakingTimeChanged();

protected:
	/// Implementation method for setMaximumEnergy.  The view handles all the visuals, but if there is something specific that needs to be passed on to the detector or viewing subclass, you can implement that here.
	virtual void setMaximumEnergyImplementation(double energy);

	/// Pointer to the actual VESPERSSingleElementVortexDetector for setting the peaking time and maximum energy.
	VESPERS13ElementGeDetector *ge13ElementDetector_;

	/// Spin box for the peaking time.
	QDoubleSpinBox *peakingTimeSpinBox_;
};

#endif // VESPERS13ELEMENTGEDETECTORVIEW_H
