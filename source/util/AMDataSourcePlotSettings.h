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


#ifndef AMDATASOURCEPLOTSETTINGS_H
#define AMDATASOURCEPLOTSETTINGS_H

#include <QPen>
#include <QColor>

#include "MPlot/MPlotColorMap.h"

/// This class holds visualization information about AMDataSources; all the plot settings that are associated with a particular plot/layout, rather than with the AMDataSource itself.
class AMDataSourcePlotSettings {
public:
	/// Default Constructor
 	virtual ~AMDataSourcePlotSettings(){}
	AMDataSourcePlotSettings(bool Visible = true, double Priority = 1, const QPen& LinePen = QPen(nextColor()))
		: priority(Priority),
		  visible(Visible),
		  linePen(LinePen),
		  colorMap(MPlotColorMap::Jet)
	{

		areaFilled = false;

		colorMap.setContrast(2.1);
		colorMap.setBrightness(0.08);
		colorMap.setGamma(0.8);
	}



	/// Priority level for this data source (used for ordering... lower numbers appear first.)
	double priority;
	/// Whether this data source is shown/enabled in non-exclusive views. This option is available to users; they can toggle it on or off. It is initialized from AMDataSource::visibleInPlots()
	bool visible;

	// 1D plot settings:
	/// Pen used for this data source (dots, dashes, etc.), as well as width and color
	QPen linePen;
	/// True if the area below the plot should be filled \note These don't work yet, since MPlot doesn't yet support filled plots
	bool areaFilled;
	/// The brush of the fill, if used (ie: areaFilled is true) \note These don't work yet, since MPlot doesn't yet support filled plots
	QBrush fillBrush;

	// 2D plot settings:

	/// Resultant colormap used for multi-dimensional data
	MPlotColorMap colorMap;


	/// Globally-accessible function to get the "next" data source color to use.
	static QColor nextColor() {
		static int i = 0;

		switch(i++ % 11) {
		case 0: return QColor(255, 0, 128);
		case 1: return QColor(0, 128, 255);
		case 2: return QColor(128, 255, 0);
		case 3: return QColor(255, 128, 0);
		case 4: return QColor(128, 0, 255);
		case 5: return QColor(0, 0, 255);
		case 6: return QColor(0, 128, 0);
			// Yellow is hard to see: case 7: return QColor(255, 255, 0);
		case 7: return QColor(255, 0, 0);
		case 8: return QColor(0, 64, 128);
		case 9: return QColor(128, 64, 0);
		case 10: default: return QColor(128, 0, 64);
		}
	}

};

#endif // AMDATASOURCEPLOTSETTINGS_H
