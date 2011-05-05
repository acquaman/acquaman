#ifndef XRFDETECTORVIEW_H
#define XRFDETECTORVIEW_H

#include "ui/AMDetectorView.h"
#include "beamline/VESPERS/XRFDetector.h"
#include "util/AMElement.h"

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
	/// Returns a pointer to the plot.
	MPlot *plot() const { return plot_; }

public slots:
	/// Places coloured markers on the plot to show where the acceptable emission lines are in the range of the detector.
	void showEmissionLines(AMElement *el);
	/// Changes the colors of the ROIMarkers for the latest element selected, if any.
	void highlightMarkers(AMElement *el);
	/// Slot handling what happens when a region of interest is added.
	void onAdditionOfRegionOfInterest(AMElement *el, QPair<QString, QString> line);
	/// Slot handling what happens when a region of interest is removed.
	void onRemovalOfRegionOfInterest(AMElement *el, QPair<QString, QString> line);
	/// Slot removing all the element markers.
	void removeAllRegionsOfInterest();
	/// Handles resizing the ROIPlotMarkers based on the maximum height of the data.
	void resizeRoiMarkers();
	/// Handles resizing the ROIPlotMarkers to a new width.
	//void roiWidthUpdate(AMROI *roi);

protected slots:
	/// Handles the update from the dead time control.
	void onDeadTimeUpdate();
	/// Handles the update from the elapsed time control.
	void onElapsedTimeUpdate(double time);
	/// Handles enabling/disabling of elements based on which button is clicked in the dead time button group.
	void elementClicked(int elementId);
	/// Handles changes from the spectrum update rate combo box.
	void onComboBoxUpdate(int index);
	/// Handles the changes from the update rate control.
	void onUpdateRateUpdate(double val);
	/// Enables/Disables whether the raw spectra are displayed or the corrected sum.
	void onWaterfallToggled(bool isWaterfall);
	/// Changes the amount of waterfall separation between the plots.
	void onWaterfallSeparationChanged(double val);

protected:

	/*! Sets up the view based with the given detector.
	 We are trusting createDetectorView to pass in the correct type of detector, sub classes should trust AMDetector is actually their type. */
	bool setDetector(AMDetector *detector, bool configureOnly);

	/// Sets up the plot.  Can add the plot widget to the layout after calling this function.
	void setupPlot();

	/// Gets the maximum height from the first data source.  Used for scaling the height of the ROI markers.
	double getMaximumHeight(MPlotItem *data);

	/// Get a color for the color of a line on the plot.
	QColor getColor(int index);
	/// Gets a color based on an emission line.
	QColor getColor(QString name);

	/// The pointer to the detector.
	XRFDetector *detector_;

	/// The elapsed time label.
	QLabel *elapsedTime_;
	/// The dead time label.
	QLabel *deadTime_;
	/// The spectrum update rate combo box.
	QComboBox *updateRate_;
	/// The waterfall spin box.
	QDoubleSpinBox *waterfallSeparation_;

	/// The button group used for the dead time tool buttons.
	QButtonGroup *deadTimeGroup_;

	/// This is the plot widget that holds the plot used for viewing the spectra.
	MPlotWidget *view_;
	/// This is the plot itself.
	MPlot *plot_;
	/// Holds the list of current markers.
	QList<MPlotItem *> markers_;
	/// This holds the plot markers for showing emission lines.
	QList<MPlotPoint *> *lines_;
};

#endif // XRFDETECTORVIEW_H
