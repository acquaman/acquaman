#ifndef AMXRFBASEDETECTORVIEW_H
#define AMXRFBASEDETECTORVIEW_H

#include <QWidget>

#include "beamline/AMXRFDetector.h"

#include "MPlot/MPlot.h"
#include "MPlot/MPlotWidget.h"
#include "MPlot/MPlotSeries.h"
#include "MPlot/MPlotPoint.h"
#include "MPlot/MPlotMarkerTransparentVerticalRectangle.h"

/// This class provides a very basic view for any AMXRFDetector.  It provides very basic control and feedback with a spectrum viewer.
class AMXRFBaseDetectorView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.  Takes in an AMXRFDetector pointer.
	explicit AMXRFBaseDetectorView(AMXRFDetector *detector, QWidget *parent = 0);

signals:

public slots:

protected:
	/// Sets up the plot.  Can add the plot widget to the layout after calling this function.
	void setupPlot();

	/// The pointer to the detector.
	AMXRFDetector *detector_;

	/// This is the plot widget that holds the plot used for viewing the spectra.
	MPlotWidget *view_;
	/// This is the plot itself.
	MPlot *plot_;
};

#endif // AMXRFBASEDETECTORVIEW_H
