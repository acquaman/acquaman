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
	/// Constructor.  Currently takes no arguments.  However, it could be imagined that it takes the X, Y, and Intensity PVs as arguments to make it more general and applicable in other programs.
	explicit BeamPositionMonitor(QWidget *parent = 0);
	/// Example of constructor making this class more accessible to other programs.
	explicit BeamPositionMonitor(QString name, AMProcessVariable *x, AMProcessVariable *y, AMProcessVariable *intensity, QWidget *parent = 0);

signals:

public slots:

protected slots:
	/// Handles changes in the intensity.
	void onIntensityChanged(double val);

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
