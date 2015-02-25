#ifndef CSRMAINWINDOW_H
#define CSRMAINWINDOW_H

#include <QWidget>

#include <QLabel>

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

protected:
	/// Sets up the plot.  Can add the plot widget to the layout after calling this function.  Can be reimplemented for customizing the plot as desired.  This is called inside of AMXRFBaseDetectorView::buildDetectorView().  If completely re-implementing buildDetectorView, you need to make sure to call this method inside your own buildDetectorView().
	void setupPlot();

	/// The data model.
	CSRDataModel *model_;

	/// This is the plot widget that holds the plot used for viewing the spectra.
	MPlotWidget *plotView_;
	/// This is the plot itself.
	MPlot *plot_;
};

#endif // CSRMAINWINDOW_H
