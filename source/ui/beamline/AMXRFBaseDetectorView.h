#ifndef AMXRFBASEDETECTORVIEW_H
#define AMXRFBASEDETECTORVIEW_H

#include <QWidget>

#include "beamline/AMXRFDetector.h"

#include "MPlot/MPlot.h"
#include "MPlot/MPlotWidget.h"
#include "MPlot/MPlotSeries.h"
#include "MPlot/MPlotPoint.h"
#include "MPlot/MPlotMarkerTransparentVerticalRectangle.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QDoubleSpinBox>

/// This class provides a very basic view for any AMXRFDetector.  It provides very basic control and feedback with a spectrum viewer.
class AMXRFBaseDetectorView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.  Takes in an AMXRFDetector pointer.
	explicit AMXRFBaseDetectorView(AMXRFDetector *detector, QWidget *parent = 0);

signals:

public slots:

protected slots:
	/// Updates the status label when the detector acquisition state changes.
	void onAcquisitionStateChanged(AMDetector::AcqusitionState state);
	/// Sets the detector acquisition time.
	void onNewAcquisitionTimeSetpoint();
	/// Handles updates to the elapsed time.
	void onElapsedTimeChanged(double time);

protected:
	/// Builds the for the detector.  Reimplement this method when you want to customize the entire look and feel of the detector view.
	virtual void buildDetectorView();
	/// Sets up the plot.  Can add the plot widget to the layout after calling this function.  Can be reimplemented for customizing the plot as desired.
	virtual void setupPlot();

	/// The pointer to the detector.
	AMXRFDetector *detector_;

	/// This is the plot widget that holds the plot used for viewing the spectra.
	MPlotWidget *view_;
	/// This is the plot itself.
	MPlot *plot_;

	// The GUI elements for basic AMXRFDetectors.
	/// The acquire button.
	QPushButton *acquireButton_;
	/// The cancle button.
	QPushButton *cancelButton_;
	/// The acquire time spin box.
	QDoubleSpinBox *acquireTimeSpinBox_;
	/// The elapsed time label.
	QLabel *elapsedTimeLabel_;
	/// The status label.
	QLabel *statusLabel_;

	// The main layouts.
	/// The master layout that holds all of the other layouts in the base view.
	QVBoxLayout *masterLayout_;
	/// The left vertical layout.
	QVBoxLayout *leftLayout_;
	/// The right vertical layout.
	QVBoxLayout *rightLayout_;
	/// The top horizontal layout.
	QHBoxLayout *topLayout_;
	/// The bottom horizontal layout.
	QHBoxLayout *bottomLayout_;
	/// The horizontal layout that holds the plot.
	QHBoxLayout *plotLayout_;
};

#endif // AMXRFBASEDETECTORVIEW_H
