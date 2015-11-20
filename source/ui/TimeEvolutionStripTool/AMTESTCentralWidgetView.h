#ifndef AMTESTCENTRALWIDGETVIEW_H
#define AMTESTCENTRALWIDGETVIEW_H

#include <QWidget>

#include "MPlot/MPlot.h"
#include "MPlot/MPlotWidget.h"

/// This is the central widget for the time evolution strip tool.
class AMTESTCentralWidgetView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMTESTCentralWidgetView(QWidget *parent = 0);
	/// Destructor.
	virtual ~AMTESTCentralWidgetView();

signals:

public slots:

protected:
	/// Creates a plot for viewing the sources.
	void setupPlot();

	/// Holds the plot widget.
	MPlotWidget *plotWidget_;
	/// Holds the plot.
	MPlot *plot_;
};

#endif // AMTESTCENTRALWIDGETVIEW_H
