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
	/// Handles updating the label that says when the line scan exceeds the time per acquisition.
	void onTimesChanged();

protected:
	/// Returns the pointer to the timed scan configuration when it's needed explicitly.
	VESPERSTimedLineScanConfiguration *timedLineConfiguration() const;

	/// The time per acquisition box.
	QDoubleSpinBox *timePerAcquisitionBox_;
	/// The number of iterations box.
	QSpinBox *iterationsBox_;
	/// Help message for time per acquisition.
	QLabel *helpMessage_;
};

#endif // VESPERSTIMEDLINESCANCONFIGURATIONVIEW_H
