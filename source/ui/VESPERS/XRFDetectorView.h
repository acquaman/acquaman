#ifndef XRFDETECTORVIEW_H
#define XRFDETECTORVIEW_H

#include "ui/AMDetectorView.h"
#include "beamline/VESPERS/XRFDetector.h"
#include "util/VESPERS/XRFElement.h"
#include "ui/VESPERS/ROIPlotMarker.h"

#include "MPlot/MPlot.h"
#include "MPlot/MPlotWidget.h"
#include "MPlot/MPlotSeries.h"

#include <QLabel>
#include <QDoubleSpinBox>
#include <QPushButton>

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

	/// Returns the minimum energy used for limiting what is drawn on the view.
	double minimumEnergy() const { return minimumEnergy_; }
	/// Returns the maximum energy used for limiting what is drawn on the view.
	double maximumEnergy() const { return maximumEnergy_; }

public slots:
	/// Places coloured markers on the plot to show where the acceptable emission lines are in the range of the detector.
	void showEmissionLines(XRFElement *el);
	/// Changes the colors of the ROIMarkers for the latest element selected, if any.
	void highlightMarkers(XRFElement *el);
	/// Slot handling what happens when a region of interest is added.
	void addRegionOfInterestMarker(AMROIInfo info);
	/// Slot handling what happens when a region of interest is removed.
	void removeRegionOfInterestMarker(AMROIInfo info);
	/// Slot removing all the element markers.
	void removeAllRegionsOfInterestMarkers();
	/// Slot that sorts all the regions of interst.
	void sortRegionsOfInterest();
	/// Handles resizing the ROIPlotMarkers to a new width.
	void roiWidthUpdate(AMROI *roi);
	/// Sets the minimum energy used by this view.
	void setMinimumEnergy(double energy) { minimumEnergy_ = energy; }
	/// Sets the maximum energy used by this view.
	void setMaximumEnergy(double energy) { maximumEnergy_ = energy; }

protected slots:
	/// Once the detector emits the connected signal, this will connect all the appropriate slots.  It also disconnects them if the detector goes offline.
	void onDetecterConnected(bool connected);

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
	/// Handles when the log scale button is clicked.  It takes a log of the data if true.
	void onLogEnabled(bool logged);
	/// Enables/Disables whether the raw spectra are displayed or the corrected sum.
	void onWaterfallToggled(bool isWaterfall);
	/// Changes the amount of waterfall separation between the plots.
	void onWaterfallSeparationChanged(double val);
	/// Handles if the detector ROIs have changed from an external source.  This listens to the externalRegionsOfInterestChanged signal and changes all the markers to reflect the new list.
	void onExternalRegionsOfInterestChanged();
	/// Handles changing the indicator light when status changes.
	void onStatusChanged() { detector_->status() == 1 ? status_->setPixmap(QIcon(":/ON.png").pixmap(20)) : status_->setPixmap(QIcon(":/OFF.png").pixmap(20)); }

	/// Hack to save the spectra.  For four element it will print out the four raw data and the corrected sum.
	void saveSpectra();

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

	/// The indicator light.
	QLabel *status_;
	/// The elapsed time label.
	QLabel *elapsedTime_;
	/// The dead time label.
	QLabel *deadTime_;
	/// The spectrum update rate combo box.
	QComboBox *updateRate_;
	/// The waterfall spin box.
	QDoubleSpinBox *waterfallSeparation_;
	/// The log button.
	QPushButton *logButton_;
	/// The raw spectra button.
	QPushButton *waterfallButton_;

	/// The button group used for the dead time tool buttons.
	QButtonGroup *deadTimeGroup_;

	/// This is the plot widget that holds the plot used for viewing the spectra.
	MPlotWidget *view_;
	/// This is the plot itself.
	MPlot *plot_;
	/// This holds the raw data sources.
	QList<MPlotSeriesBasic *> rawDataSeries_;
	/// This holds the corrected sum.
	MPlotSeriesBasic *corrSum_;
	/// Holds whether the plot is in a waterfall plot mode or not.
	bool isWaterfall_;

	/// This holds the minimum energy that should be displayed by this view.  This is important for deciding what lines should be painted or not.
	double minimumEnergy_;
	/// This holds the maximum energy that should be displayed by this view.  This is important for deciding what lines should be painted or not.
	double maximumEnergy_;

	/// Holds the list of current markers.
	QList<ROIPlotMarker *> markers_;
	/// This holds the plot markers for showing emission lines.
	QList<MPlotPoint *> lines_;
};

#endif // XRFDETECTORVIEW_H
