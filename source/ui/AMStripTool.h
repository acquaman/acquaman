#ifndef AMSTRIPTOOL_H
#define AMSTRIPTOOL_H

#include <QWidget>
#include <QLayout>

#include "MPlot/MPlot.h"
#include "MPlot/MPlotWidget.h"

#include "beamline/AMControl.h"

class AMDataSource;
class AMDataSourceSeriesData;

class AMStripTool : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMStripTool(QWidget *parent = 0);
	/// Destructor.
	virtual ~AMStripTool();

	/// Returns the plot.
	MPlot* plot() const { return plot_; }

signals:

public slots:
	/// Adds a control to the plot.
	void addControl(AMControl *control_);

protected:
	/// Creates and returns a series suitable for plotting pv updates with accompanying timestamps for the given control.
	MPlotItem* createTimedDataPlotItem(AMControl *control);


protected:
	/// The plot proper.
	MPlot *plot_;
	/// The plot widget.
	MPlotWidget *plotWidget_;

};

#endif // AMSTRIPTOOL_H
