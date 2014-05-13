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


#include "AM3dDataSourceView.h"

#include "../contrib/qwtplot3d/include/qwt3d_surfaceplot.h"
#include <QBoxLayout>
#include <QStringBuilder>

#include "dataman/AMScan.h"
#include "dataman/datasource/AMDataSource.h"
#include "util/AMDataSourcePlotSettings.h"

#include <QGLColormap>

 AM3dDataSourceView::~AM3dDataSourceView(){}
AM3dDataSourceView::AM3dDataSourceView(const AMScan* scan, int dataSourceIndex, QWidget *parent) :
	QWidget(parent)
{
	QVBoxLayout* vl = new QVBoxLayout();

	surfacePlot_ = new Qwt3D::SurfacePlot();
	surfacePlot_->setPlotStyle(Qwt3D::FILLEDMESH);
	surfacePlot_->setShading(Qwt3D::GOURAUD);

	surfacePlot_->enableLighting();
	surfacePlot_->illuminate(0);
	surfacePlot_->setLightShift(23,23,23);
	surfacePlot_->setLightRotation(90,0,0);

	surfacePlot_->setRotation(30,0,15);
	surfacePlot_->setZoom(0.90);


	QGLColormap colors;
	colors.setEntry(0, qRgb(0, 0, 131));
	colors.setEntry(1, qRgb(0, 0, 255));
	colors.setEntry(2, qRgb(0, 255, 255));
	colors.setEntry(3, qRgb(255, 255, 0));
	colors.setEntry(4, qRgb(255, 0, 0));
	colors.setEntry(5, qRgb(128, 0, 0));

	surfacePlot_->setColormap(colors);
	surfacePlot_->setResolution(4);
	vl->addWidget(surfacePlot_);

	setLayout(vl);

	if(scan && scan->dataSourceCount() > dataSourceIndex && scan->dataSourceAt(dataSourceIndex)->rank() == 2) {
		scan_ = scan;
		dataSourceIndex_ = dataSourceIndex;
		updatePlotScheduler_.schedule();
	}
	else {
		scan_ = 0;
		dataSourceIndex_ = 0;
	}

	connect(&updatePlotScheduler_, SIGNAL(executed()), this, SLOT(updatePlotFromDataSource()));
}

bool AM3dDataSourceView::setScanAndDataSource(const AMScan *scan, int dataSourceIndex)
{
	if(!scan || dataSourceIndex >= scan->dataSourceCount() || scan->dataSourceAt(dataSourceIndex)->rank() != 2)
		return false;	// silly error, or wrong dimensionality

	scan_ = scan;
	dataSourceIndex_ = dataSourceIndex;

	updatePlotScheduler_.schedule();	// will call updatePlotFromDataSource() when back in the event loop.
	return true;
}

void AM3dDataSourceView::updatePlotFromDataSource()
{
	// Know from previously that the rank of this data source is 2.

	// Or the scan is 0, which means we have no data to show.
	if(scan_ == 0) {
		surfacePlot_->loadFromData(0,0,0);
		return;
	}


	AMDataSource* ds = scan_->dataSourceAt(dataSourceIndex_);

	double minZ = 0, maxZ = 1;
	int sizeX = ds->size(0);
	int sizeY = ds->size(1);

	bool uniformAxisScales = ds->axisInfoAt(0).isUniform && ds->axisInfoAt(1).isUniform;

	if(uniformAxisScales) {
		qDebug() << "Uniform axis scales. Using simplest (uniform) grid filling";
		double** zValues = new2dArray<double>(sizeX, sizeY);

		if(logScaleEnabled_) {
			if(sizeX > 0 && sizeY > 0)
				minZ = maxZ = log(qMax(logMin_,(double)ds->value(AMnDIndex(0,0))));

			for(int i=0; i<sizeX; i++)
				for(int j=0; j<sizeY; j++) {
					double zValue = log(qMax(logMin_,(double)ds->value(AMnDIndex(i,j))));
					minZ = qMin(minZ, zValue);
					maxZ = qMax(maxZ, zValue);
					zValues[i][j] = zValue;
				}
		}
		else {
			if(sizeX > 0 && sizeY > 0)
				minZ = maxZ = ds->value(AMnDIndex(0,0));

			for(int i=0; i<sizeX; i++)
				for(int j=0; j<sizeY; j++) {
					double zValue = ds->value(AMnDIndex(i,j));
					minZ = qMin(minZ, zValue);
					maxZ = qMax(maxZ, zValue);
					zValues[i][j] = zValue;
				}
		}

		surfacePlot_->loadFromData(zValues,
								   sizeX,
								   sizeY,
								   ds->axisValue(0,0),
								   ds->axisValue(0,sizeX-1),
								   ds->axisValue(1,0),
								   ds->axisValue(1,sizeY-1));

		// called from loadFromData(): surfacePlot_->updateData();
		// not necessary?: surfacePlot_->updateGL();

		delete2dArray(zValues);
	}
	else {	// non-uniform axis scale grid
		qDebug() << "Non-uniform axis scale. Using irregular grid";

		Qwt3D::Triple** gridValues = new2dArray<Qwt3D::Triple>(sizeX, sizeY);

		if(logScaleEnabled_) {
			if(sizeX >0 && sizeY > 0)
				minZ = maxZ = log(qMax(logMin_,(double)ds->value(AMnDIndex(0,0))));

			for(int i=0; i<sizeX; i++) {
				double xAxisValue = ds->axisValue(0,i);
				for(int j=0; j<sizeY; j++) {
					double zValue = log(qMax(logMin_,(double)ds->value(AMnDIndex(i,j))));
					minZ = qMin(minZ, zValue);
					maxZ = qMax(maxZ, zValue);
					gridValues[i][j] = Qwt3D::Triple(xAxisValue, ds->axisValue(1,j), zValue);
				}
			}
		}
		else {
			if(sizeX >0 && sizeY > 0)
				minZ = maxZ = ds->value(AMnDIndex(0,0));

			for(int i=0; i<sizeX; i++) {
				double xAxisValue = ds->axisValue(0,i);
				for(int j=0; j<sizeY; j++) {
					double zValue = ds->value(AMnDIndex(i,j));
					minZ = qMin(minZ, zValue);
					maxZ = qMax(maxZ, zValue);
					gridValues[i][j] = Qwt3D::Triple(xAxisValue, ds->axisValue(1,j), zValue);
				}
			}
		}

		surfacePlot_->loadFromData(gridValues, sizeX, sizeY);
		delete2dArray(gridValues);
	}

	// doesnt realy do anyting: surfacePlot_->coordinates()->setAutoScale(true);

	double zRange = maxZ - minZ;
	double xRange = (double)ds->axisValue(0, sizeX-1) - (double)ds->axisValue(0,0);
	double yRange = (double)ds->axisValue(1, sizeY-1) - (double)ds->axisValue(1,0);

	surfacePlot_->setScale(100*zRange/xRange, 100*zRange/yRange, 100);

	double drad = (surfacePlot_->hull().maxVertex-surfacePlot_->hull().minVertex).length();
	drad *= 30/20.;
	surfacePlot_->setLightShift(drad,drad,drad);
	surfacePlot_->setLightRotation(90,0,0);

	AMAxisInfo xInfo = ds->axisInfoAt(0);
	AMAxisInfo yInfo = ds->axisInfoAt(1);

	QString xLabel = xInfo.description % " (" % xInfo.units % ")";
	QString yLabel = yInfo.description % " (" % yInfo.units % ")";
	QString zLabel = ds->description();

	surfacePlot_->coordinates()->axes[Qwt3D::X1].setLabelString(xLabel);
	surfacePlot_->coordinates()->axes[Qwt3D::X2].setLabelString(xLabel);
	surfacePlot_->coordinates()->axes[Qwt3D::X3].setLabelString(xLabel);
	surfacePlot_->coordinates()->axes[Qwt3D::X4].setLabelString(xLabel);

	surfacePlot_->coordinates()->axes[Qwt3D::Y1].setLabelString(yLabel);
	surfacePlot_->coordinates()->axes[Qwt3D::Y2].setLabelString(yLabel);
	surfacePlot_->coordinates()->axes[Qwt3D::Y3].setLabelString(yLabel);
	surfacePlot_->coordinates()->axes[Qwt3D::Y4].setLabelString(yLabel);

	surfacePlot_->coordinates()->axes[Qwt3D::Z1].setLabelString(zLabel);
	surfacePlot_->coordinates()->axes[Qwt3D::Z2].setLabelString(zLabel);
	surfacePlot_->coordinates()->axes[Qwt3D::Z3].setLabelString(zLabel);
	surfacePlot_->coordinates()->axes[Qwt3D::Z4].setLabelString(zLabel);
}

void AM3dDataSourceView::setPlotStyle(int plotStyle)
{
	if(plotStyle == surfacePlot_->plotStyle())
		return;

	if(plotStyle < Qwt3D::NOPLOT || plotStyle > Qwt3D::POINTS)
		return;

	surfacePlot_->setPlotStyle((Qwt3D::PLOTSTYLE) plotStyle);
	surfacePlot_->updateData();
	surfacePlot_->updateGL();
}

void AM3dDataSourceView::setFloorStyle(int floorStyle)
{
	if(floorStyle == surfacePlot_->floorStyle())
		return;

	if(floorStyle < Qwt3D::NOFLOOR || floorStyle > Qwt3D::FLOORDATA)
		return;

	surfacePlot_->setFloorStyle((Qwt3D::FLOORSTYLE) floorStyle);
	surfacePlot_->updateData();
	surfacePlot_->updateGL();
}

void AM3dDataSourceView::setLogScaleEnabled(bool logScaleOn, double logMinValue)
{
	if(logScaleEnabled_ == logScaleOn && logMin_ == logMinValue)
		return;

	logScaleEnabled_ = logScaleOn;
	logMin_ = logMinValue;
	updatePlotScheduler_.schedule();
}



