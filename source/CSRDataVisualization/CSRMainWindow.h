#ifndef CSRMAINWINDOW_H
#define CSRMAINWINDOW_H

#include <QWidget>

#include <QLabel>
#include <QSpinBox>
#include <QComboBox>

#include "CSRDataVisualization/CSRDataModel.h"

#include "MPlot/MPlot.h"
#include "MPlot/MPlotWidget.h"
#include "MPlot/MPlotSeries.h"
#include "MPlot/MPlotPoint.h"

class CSRMainWindow : public QWidget
{
	Q_OBJECT
public:
	/// Constructor.
	explicit CSRMainWindow(CSRDataModel *model, QWidget *parent = 0);
	/// Destructor.
	~CSRMainWindow();

signals:

public slots:

protected slots:
	/// Updates the minimum display value.
	void onRangeMinimumChanged();
	/// Updates the maximum display value.
	void onRangeMaximumChanged();
	/// Updates the revolution.
	void onRevolutionChanged();
	/// Changes which plot is visualized.
	void onDataSelectionChanged(int index);

protected:
	/// Sets up the plot.  Can add the plot widget to the layout after calling this function.  Can be reimplemented for customizing the plot as desired.  This is called inside of AMXRFBaseDetectorView::buildDetectorView().  If completely re-implementing buildDetectorView, you need to make sure to call this method inside your own buildDetectorView().
	void setupDataPlot();
	/// Sets up the display plot.
	void setupDisplayPlot();
	/// Update plot items and data.
	void updatePlot();

	/// The data model.
	CSRDataModel *model_;

	/// Spin box for defining the minium display value.
	QSpinBox *rangeMinimum_;
	/// Spin box for defining the maximum display value.
	QSpinBox *rangeMaximum_;
	/// Spin box for looking at a full revolution.
	QSpinBox *revolution_;
	/// Combo box for choosing which plot to look at.
	QComboBox *dataSelectionBox_;
	/// This is the plot widget that holds the plot used for viewing the spectra.
	MPlotWidget *dataPlotView_;
	/// This is the plot itself.
	MPlot *dataPlot_;
	/// The series data that lets us visualize the points.
	MPlotVectorSeriesData *dataSeriesData_;
	/// The list of point markers.
	QList<MPlotPoint *> items_;
	/// List of peak markers.
	QList<MPlotPoint *> peakMarkers_;
	/// Maximum markers.
	QList<MPlotPoint *> maximaMarkers_;
	QList<MPlotSeriesBasic *> dataSeries_;

	/// This is the plot widget that holds the plot used for viewing the spectra.
	MPlotWidget *displayPlotView_;
	/// This is the plot itself.
	MPlot *displayPlot_;
	/// The series data that lets us visualize the points.
	MPlotVectorSeriesData *integralSeriesData_;
	/// The series data that lets us visualize the points.
	MPlotVectorSeriesData *mainToMainSeriesData_;
	/// The series data that lets us visualize the points.
	MPlotVectorSeriesData *mainPeakMaximaSeriesData_;
	/// The series data that lets us visualize the points.
	MPlotVectorSeriesData *wakefieldPeakMaximaSeriesData_;
};

#endif // CSRMAINWINDOW_H
