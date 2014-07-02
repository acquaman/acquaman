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


#ifndef AM3DDATASOURCEVIEW_H
#define AM3DDATASOURCEVIEW_H

#include <QWidget>
#include "util/AMDeferredFunctionCall.h"

class AMScan;
namespace Qwt3D {
	class SurfacePlot;
}

class AM3dDataSourceView : public QWidget
{
	Q_OBJECT
public:

	enum PlotStyle { WireFrameStyle = 1, WireFrameWithHiddenLinesStyle, ColorStyle, WireFrameWithColorStyle, PointsStyle };
	enum FloorStyle { NoFloor, ContourFloor, ColoredFloor };

 	virtual ~AM3dDataSourceView();
	explicit AM3dDataSourceView(const AMScan* scan = 0, int dataSourceIndex = 0, QWidget *parent = 0);

	bool setScanAndDataSource(const AMScan* scan, int dataSourceIndex);



signals:

public slots:
	void updatePlotFromDataSource();
	void setPlotStyle(int plotStyle);
	void setFloorStyle(int floorStyle);
	void setLogScaleEnabled(bool logScaleOn = true, double logMinValue = 1.0);

protected:
	Qwt3D::SurfacePlot* surfacePlot_;
	const AMScan* scan_;
	int dataSourceIndex_;

	bool logScaleEnabled_;
	double logMin_;

	template <class T>
	T** new2dArray(int sizeX, int sizeY) const {
		T** rv = new T*[sizeX];
		rv[0] = new T[sizeX*sizeY];
		for(int i=1; i<sizeX; i++)
			rv[i] = rv[0] + i*sizeY;
		return rv;
	}

	template <class T>
	void delete2dArray(T** array) const {
		delete [] array[0];
		delete [] array;
	}

	AMDeferredFunctionCall updatePlotScheduler_;

};

#endif // AM3DDATASOURCEVIEW_H
