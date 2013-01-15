/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef XRFDETECTORVIEW_H
#define XRFDETECTORVIEW_H

#include "ui/beamline/AMDetectorView.h"
#include "beamline/VESPERS/XRFDetector.h"
#include "util/VESPERS/XRFElement.h"
#include "MPlot/MPlotMarkerTransparentVerticalRectangle.h"

#include "MPlot/MPlot.h"
#include "MPlot/MPlotWidget.h"
#include "MPlot/MPlotSeries.h"
#include "MPlot/MPlotPoint.h"

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
	AMOldDetector *detector() { return detector_; }

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
	bool setDetector(AMOldDetector *detector, bool configureOnly);

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
	AMOldDetector *detector() { return detector_; }
	/// Returns a pointer to the plot.
	MPlot *plot() const { return plot_; }

	/// Returns the minimum energy used for limiting what is drawn on the view.
	double minimumEnergy() const { return minimumEnergy_; }
	/// Returns the maximum energy used for limiting what is drawn on the view.
	double maximumEnergy() const { return maximumEnergy_; }
	/// Returns whether or not the ROI markers will be highlighted when the current element is changed.
	bool roiMarkersHightlighted() const { return highlightMarkers_; }
	/// Returns whether or not the possibly viewable emission lines are visible.
	bool emissionLinesVisible() const { return showEmissionLines_; }
	/// Returns whether or not pile up peaks for the current element are visible.
	bool pileUpPeaksVisible() const { return showPileUpPeaks_; }
	/// Returns whether or not combination pile up peaks are visible.
	bool combinationPileUpPeaksVisible() const { return showCombinationPileUpPeaks_; }
	/// Returns the currently viewable element.
	XRFElement *currentElement() const { return currentElement_; }
	/// Returns the element that is used in combination pile up peak visualization.
	XRFElement *secondaryElement() const { return secondaryElement_; }

public slots:
	/// Slot handling what happens when a region of interest is added.
	void addRegionOfInterestMarker(AMROIInfo info);
	/// Slot handling what happens when a region of interest is removed.
	void removeRegionOfInterestMarker(AMROIInfo info);
	/// Slot removing all the element markers.
	void removeAllRegionsOfInterestMarkers();
	/// Slot that sorts all the regions of interst.
	void sortRegionsOfInterest();
	/// Handles resizing the MPlotMarkerTransparentVerticalRectangles to a new width.
	void roiWidthUpdate(AMROI *roi);
	/// Sets the minimum energy used by this view.
	void setMinimumEnergy(double energy) { minimumEnergy_ = energy; }
	/// Sets the maximum energy used by this view.
	void setMaximumEnergy(double energy) { maximumEnergy_ = energy; }
	/// Sets whether or not the ROI markers are highlighted.
	void setROIMarkersHighlighted(bool highlight) { highlightMarkers_ = highlight; highlightMarkers(); }
	/// Sets whether or not possible emission lines are visible.
	void setEmissionLinesVisible(bool showLines) { showEmissionLines_ = showLines; showEmissionLines(); }
	/// Sets whether or not pile up peaks are visible.
	void setPileUpPeaksVisible(bool showPeaks)
	{
		showPileUpPeaks_ = showPeaks;
		showPileUpPeaks();

		if (!showPeaks)
			setCombinationPileUpPeaksVisible(false);
	}
	/// Sets whether or not combination pile up peaks are visible.
	void setCombinationPileUpPeaksVisible(bool showPeaks) { showCombinationPileUpPeaks_ = showPeaks; showCombinationPileUpPeaks(); }
	/// Sets the current element that is to be viewed.
	void setCurrentElement(XRFElement *el);
	/// Sets the secondary element that is to be viewed.
	void setSecondaryElement(XRFElement *el);

protected slots:
	/// Places coloured markers on the plot to show where the acceptable emission lines are in the range of the detector.  Uses the current element.
	void showEmissionLines();
	/// Places coloured markers on the plot to show where the pile up peaks are in the range of the detector.  Uses the current element.
	void showPileUpPeaks();
	/// Places coloured markers on the plot to show where the combination pile up peaks are in the range of the detector.  Uses the current element and the secondary element.
	void showCombinationPileUpPeaks();
	/// Changes the colors of the ROIMarkers for the latest element selected, if any.  Uses the current element.
	void highlightMarkers();

	/// Handles the update from the dead time control.
	void onDeadTimeUpdate();
	/// Handles the update from the elapsed time control.
	void onElapsedTimeUpdate(double time);
	/// Handles changes in the dead time and passes on the dead time to the dead time buttons.
	void onDeadTimeChanged();
	/// Handles enabling/disabling of elements based on which button is clicked in the dead time button group.
	void elementClicked(int elementId);
	/// Handles changes from the spectrum update rate combo box.
	void onComboBoxUpdate(int index);
	/// Handles setting the combo box is the status is changed by another program.
	void onUpdateRateChanged(int index)
	{
		updateRate_->blockSignals(true);
		updateRate_->setCurrentIndex(index);
		updateRate_->blockSignals(false);
	}
	/// Handles when the log scale button is clicked.  It takes a log of the data if true.
	void onLogEnabled(bool logged);
	/// Enables/Disables whether the raw spectra are displayed or the corrected sum.
	void onWaterfallToggled(bool isWaterfall);
	/// Changes the amount of waterfall separation between the plots.
	void onWaterfallSeparationChanged(double val);
	/// Handles if the detector ROIs have changed from an external source.  This listens to the externalRegionsOfInterestChanged signal and changes all the markers to reflect the new list.
	void onExternalRegionsOfInterestChanged();
	/// Handles changing the indicator light when status changes.
	void onStatusChanged(bool status) { status == true ? status_->setPixmap(QIcon(":/ON.png").pixmap(20)) : status_->setPixmap(QIcon(":/OFF.png").pixmap(20)); }
	/// Handles resetting everything in the view after the detector reconnects.
	void onConnectionChanged(bool isConnected);

	/// Hack to save the spectra.  For four element it will print out the four raw data and the corrected sum.
	void saveSpectra();

protected:

	/*! Sets up the view based with the given detector.
	 We are trusting createDetectorView to pass in the correct type of detector, sub classes should trust AMDetector is actually their type. */
	bool setDetector(AMOldDetector *detector, bool configureOnly);

	/// Sets up the plot.  Can add the plot widget to the layout after calling this function.
	void setupPlot();

	/// Gets the maximum height from the first data source.  Used for scaling the height of the ROI markers.
	double getMaximumHeight(MPlotItem *data);

	/// Returns whether the given \param energy is within energy range of the detector.
	bool withinEnergyRange(double energy) { return (energy <= maximumEnergy_ && energy >= minimumEnergy_) ? true : false; }

	/// Helper function that builds a pile up line based on two indices for emission lines.  Does nothing if either index is invalid.
	/*!
	  \param el1 is the element for the first pile up component.
	  \param line1 is the first line used for the pile up calculation.
	  \param el2 is the element for the second pile up component.
	  \param line 2 is the second line used for the pile up calculation.
	  */
	void addPileUpMarker(XRFElement *el1, int line1, XRFElement *el2, int line2);

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
	QList<MPlotMarkerTransparentVerticalRectangle *> markers_;
	/// This holds the plot markers for showing emission lines.
	QList<MPlotPoint *> lines_;
	/// This holds the plot markers for showing pile up peaks.
	QList<MPlotPoint *> pileUpLines_;
	/// This holds the plot markers for showing combination pile up peaks.
	QList<MPlotPoint *> combinationPileUpLines_;

	/// Current element selected.
	XRFElement *currentElement_;
	/// Secondary element used for combination pile up peak visualization.
	XRFElement *secondaryElement_;

	/// Bool holds whether or not regions of interest are highlighted when the currently viewed element is changed.
	bool highlightMarkers_;
	/// Bool holds whether or not the possible emission lines are drawn on the plot.
	bool showEmissionLines_;
	/// Bool determining if pile up peaks are visible or not.
	bool showPileUpPeaks_;
	/// Bool determining if combination pile up peaks are visible or not.
	bool showCombinationPileUpPeaks_;
};

#endif // XRFDETECTORVIEW_H
