#ifndef XRFDETECTORVIEW_H
#define XRFDETECTORVIEW_H

#include "ui/AMDetectorView.h"
#include "beamline/VESPERS/XRFDetector.h"
#include "MPlot/MPlot.h"
#include "MPlot/MPlotWidget.h"

#include <QLabel>
#include <QDoubleSpinBox>

class XRFBriefDetectorView : public AMBriefDetectorView
{
	/// This class builds a very brief view for the XRF detector.  Its primary function is to show that the detector is working and if you want to use it, enables you to build a more detailed view.

	Q_OBJECT
public:

	/// Default constructor.  Can build a view with a null pointer (ie: not a useful view) if necessary.
	Q_INVOKABLE explicit XRFBriefDetectorView(XRFDetector *detector = 0, bool configureOnly = false, QWidget *parent = 0);

	/// Returns a pointer to the detector being viewed.
	AMDetector *detector() { return detector_; }

signals:

	/// This passes on the signal that the detailed button was clicked.
	void detailedViewClicked();

public slots:

protected slots:
	/// Handles the update from the dead time control.
	void onDeadTimeUpdate();

protected:

	/*! Sets up the view based with the given detector.
	 We are trusting createDetectorView to pass in the correct type of detector, sub classes should trust AMDetector is actually their type. */
	bool setDetector(AMDetector *detector, bool configureOnly);

	/// The pointer to the detector.
	XRFDetector *detector_;

	/// The dead time label.
	QLabel *deadTime_;

};

class XRFDetailedDetectorView : public AMDetailedDetectorView
{
	/// This class builds the detailed view for viewing XRF detector data.  It should be able to read in an XRFDetector object and build a view based on the data contained within it.

	Q_OBJECT
public:

	/// Default contructor.  Can build a view with a null pointer (ie: not a useful view) if necessary.
	Q_INVOKABLE explicit XRFDetailedDetectorView(XRFDetector *detector = 0, bool configureOnly = false, QWidget *parent = 0);

	/// Returns a pointer to the detector being viewed.
	AMDetector *detector() { return detector_; }

signals:

protected slots:
	/// Handles the update from the dead time control.
	void onDeadTimeUpdate();
	/// Handles the update from the elapsed time control.
	void onElapsedTimeUpdate(double time);
	/// Handles enabling/disabling of elements based on which button is clicked in the dead time button group.
	void elementClicked(int elementId);
	/// Takes the current plot and applies a log transformation to the y-axis. If true then apply the log, if false undo it.
	void applyLog(bool apply);
	/// Takes the current plot and applies a channel transformation on the x-axis (channel # <-> energy).  If true then apply the conversion from energy to channel #, if false undo it.
	void applyChannel(bool apply);
	/// Takes the current plot and shows all the raw data instead of the corrected sum.  Only valid for detectors with more than one element.  If true create the waterfall plot, if false undo it.
	void applyWaterfall(bool apply);

protected:

	/*! Sets up the view based with the given detector.
	 We are trusting createDetectorView to pass in the correct type of detector, sub classes should trust AMDetector is actually their type. */
	bool setDetector(AMDetector *detector, bool configureOnly);

	/// Sets up the controls for the view.  Things like starting, stopping, etc.  Returns the layout to be added to the master layout.
	QLayout *setupControls();
	/// Sets up the plot view.  Returns the layout to be added to the master layout.
	QLayout *setupPlot();

	/// The pointer to the detector.
	XRFDetector *detector_;

	/// The integration time line edit.
	QDoubleSpinBox *integrationTime_;
	/// The elapsed time label.
	QLabel *elapsedTime_;
	/// The dead time label.
	QLabel *deadTime_;
	/// The waterfall separation spin box.
	QDoubleSpinBox *waterfall_;

	/// The button group used for the dead time tool buttons.
	QButtonGroup *deadTimeGroup_;

	/// This is the plot for the view.
	MPlot *plot_;
	/// This is the plot viewing widget.
	MPlotWidget *plotWindow_;
};

#endif // XRFDETECTORVIEW_H
