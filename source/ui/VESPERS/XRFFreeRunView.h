#ifndef XRFFREERUNVIEW_H
#define XRFFREERUNVIEW_H

#include <QWidget>
#include <QToolButton>

#include "ui/VESPERS/XRFDetectorView.h"
#include "acquaman/AMScanController.h"
#include "ui/VESPERS/CustomizeRegionsOfInterest.h"
#include "ui/VESPERS/VESPERSXRFScanConfigurationView.h"
#include "acquaman/VESPERS/XRFFreeRun.h"

class XRFFreeRunView : public QWidget
{
	Q_OBJECT
public:
	/// Constructor.
	explicit XRFFreeRunView(XRFFreeRun *xrfFreeRun, QWidget *parent = 0);

signals:
	/// Passes along the configuration view's start scan signal.
	void startScan();

protected slots:
	/// Handles what happens when the start button is clicked.
	void onStartClicked() { detector_->setTime(integrationTime_->value()); start_->setDisabled(true); emit startScan(); }
	/// Handles what happens when the stop button is clicked.
	void onStopClicked();
	/// Changes the disabled state of the start button based on the status of the detector.
	void onStatusChanged() { detector_->status() == true ? start_->setDisabled(true) : start_->setDisabled(false); }

protected:

	/// Pointer to the XRFPeriodicTable inside configuration.
	XRFPeriodicTable *xrfTable_;
	/// The detector view.
	XRFDetailedDetectorView *view_;
	/// The pointer to the detector.
	XRFDetector *detector_;
	/// The pointer to the customize view.
	CustomizeRegionsOfInterest *customize_;
	/// The pointer to the xrf free run model.
	XRFFreeRun *xrfFreeRun_;

	/// The start button.
	QToolButton *start_;
};

#endif // XRFFREERUNVIEW_H
