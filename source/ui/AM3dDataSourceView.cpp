#include "AM3dDataSourceView.h"

#include "qwt3d_surfaceplot.h"
#include <QBoxLayout>
#include <QStringBuilder>

#include "dataman/AMScan.h"
#include "dataman/AMDataSource.h"

AM3dDataSourceView::AM3dDataSourceView(const AMScan* scan, int dataSourceIndex, QWidget *parent) :
	QWidget(parent)
{
	QVBoxLayout* vl = new QVBoxLayout();

	surfacePlot_ = new Qwt3D::SurfacePlot();
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

	bool uniformAxisScales = ds->axisInfoAt(0).isUniform && ds->axisInfoAt(1).isUniform;

	if(uniformAxisScales) {
		qDebug() << "Uniform axis scales. Using simplest (uniform) grid filling";
		int sizeX = ds->size(0);
		int sizeY = ds->size(1);
		double** zValues = new2dArray<double>(sizeX, sizeY);

		for(int i=0; i<sizeX; i++)
			for(int j=0; j<sizeY; j++)
				zValues[i][j] = ds->value(AMnDIndex(i,j));

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
		int sizeX = ds->size(0);
		int sizeY = ds->size(1);

		Qwt3D::Triple** gridValues = new2dArray<Qwt3D::Triple>(sizeX, sizeY);
		for(int i=0; i<sizeX; i++) {
			double xAxisValue = ds->axisValue(0,i);
			for(int j=0; j<sizeY; j++) {
				gridValues[i][j] = Qwt3D::Triple(xAxisValue, ds->axisValue(1,j), ds->value(AMnDIndex(i,j)));
			}
		}

		surfacePlot_->loadFromData(gridValues, sizeX, sizeY);
		delete2dArray(gridValues);
	}

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



