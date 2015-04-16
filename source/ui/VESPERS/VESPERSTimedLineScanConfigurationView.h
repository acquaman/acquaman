#ifndef VESPERSTIMEDLINESCANCONFIGURATIONVIEW_H
#define VESPERSTIMEDLINESCANCONFIGURATIONVIEW_H

#include "ui/VESPERS/VESPERSSpatialLineScanConfigurationView.h"
#include "acquaman/VESPERS/VESPERSTimedLineScanConfiguration.h"

class VESPERSTimedLineScanConfigurationView : public VESPERSSpatialLineScanConfigurationView
{
	Q_OBJECT

public:
	/// Constructor.
	explicit VESPERSTimedLineScanConfigurationView(VESPERSTimedLineScanConfiguration *configuration, QWidget *parent = 0);
	/// Destructor.
	virtual ~VESPERSTimedLineScanConfigurationView();

protected slots:
	/// Helper slot that manages setting the time per acquisition.
	void onTimePerAcquisitionChanged();
	/// Helper slot that manages setting the number of iterations.
	void onIterationsChanged();

protected:
	/// Returns the pointer to the timed scan configuration when it's needed explicitly.
	VESPERSTimedLineScanConfiguration *timedLineConfiguration() const;

	/// The time per acquisition box.
	QDoubleSpinBox *timePerAcquisitionBox_;
	/// The number of iterations box.
	QSpinBox *iterationsBox_;
};

#endif // VESPERSTIMEDLINESCANCONFIGURATIONVIEW_H
