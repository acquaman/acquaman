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
