#ifndef BEAMPOSITIONMONITOR_H
#define BEAMPOSITIONMONITOR_H

#include <QWidget>

#include "MPlot/MPlot.h"
#include "MPlot/MPlotWidget.h"
#include "MPlot/MPlotSeries.h"
#include "MPlot/MPlotMarker.h"

#include "dataman/AMXYScatterPVDataSource.h"


class BeamPositionMonitor : public QWidget
{
	Q_OBJECT
public:
	explicit BeamPositionMonitor(QWidget *parent = 0);

signals:

public slots:

protected slots:
	/// Handles changes in the intensity.
	void onIntensityChanged(double val) { scatter_->setMarker(MPlotMarkerShape::CrossCircle, (int)val); }
protected:
	/// Sets up the plot.  Can add the plot widget to the layout after calling this function.
	void setupPlot();

	/// This is the plot widget that holds the plot used for viewing the spectra.
	MPlotWidget *view_;
	/// This is the plot itself.
	MPlot *plot_;

	/// MPlot series for the xy scatter plot.
	MPlotSeriesBasic *scatter_;

	/// The xy scatter plot data source.
	AMXYScatterPVDataSource *xy_;
	/// The intensity PV.
	AMProcessVariable *intensity_;
};

#endif // BEAMPOSITIONMONITOR_H
