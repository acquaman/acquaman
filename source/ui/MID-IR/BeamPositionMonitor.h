/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#ifndef BEAMPOSITIONMONITOR_H
#define BEAMPOSITIONMONITOR_H

#include <QWidget>

#include "MPlot/MPlot.h"
#include "MPlot/MPlotWidget.h"
#include "MPlot/MPlotSeries.h"
#include "MPlot/MPlotMarker.h"

#include "dataman/datasource/AMXYScatterPVDataSource.h"


class BeamPositionMonitor : public QWidget
{
	Q_OBJECT
public:
	/// Constructor.  Currently takes no arguments.  However, it could be imagined that it takes the X, Y, and Intensity PVs as arguments to make it more general and applicable in other programs.
 	virtual ~BeamPositionMonitor();
	explicit BeamPositionMonitor(QWidget *parent = 0);
	/// Example of constructor making this class more accessible to other programs.
	explicit BeamPositionMonitor(QString name, AMProcessVariable *x, AMProcessVariable *y, QWidget *parent = 0);
	//explicit BeamPositionMonitor(QString name, AMProcessVariable *x, AMProcessVariable *y, AMProcessVariable *intensity, QWidget *parent = 0);

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
