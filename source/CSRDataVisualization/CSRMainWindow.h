#ifndef CSRMAINWINDOW_H
#define CSRMAINWINDOW_H

#include <QWidget>

#include <QLabel>
#include <QSpinBox>

#include "CSRDataVisualization/CSRDataModel.h"

#include "MPlot/MPlot.h"
#include "MPlot/MPlotWidget.h"
#include "MPlot/MPlotSeries.h"

class CSRMainWindow : public QWidget
{
	Q_OBJECT
public:
	/// Constructor.
	explicit CSRMainWindow(CSRDataModel *model, QWidget *parent = 0);

signals:

public slots:

protected slots:
	/// Updates the minimum display value.
	void onRangeMinimumChanged();
	/// Updates the maximum display value.
	void onRangeMaximumChanged();

protected:
	/// Sets up the plot.  Can add the plot widget to the layout after calling this function.  Can be reimplemented for customizing the plot as desired.  This is called inside of AMXRFBaseDetectorView::buildDetectorView().  If completely re-implementing buildDetectorView, you need to make sure to call this method inside your own buildDetectorView().
	void setupPlot();

	/// The data model.
	CSRDataModel *model_;

	/// Spin box for defining the minium display value.
	QSpinBox *rangeMinimum_;
	/// Spin box for defining the maximum display value.
	QSpinBox *rangeMaximum_;
	/// This is the plot widget that holds the plot used for viewing the spectra.
	MPlotWidget *plotView_;
	/// This is the plot itself.
	MPlot *plot_;
	/// The series data that lets us visualize the points.
	MPlotVectorSeriesData *seriesData_;
	/// The list of point markers.
	QList<MPlotItem *> items_;
};

#endif // CSRMAINWINDOW_H
