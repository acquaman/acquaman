#ifndef XRFVIEWER_H
#define XRFVIEWER_H

#include <QWidget>

#include "MPlot/MPlot.h"
#include "MPlot/MPlotWidget.h"
#include "MPlot/MPlotSeries.h"

#include <QLabel>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QTextEdit>

class XRFViewer : public QWidget
{
	Q_OBJECT
public:
	/// Builds the viewer.  The general layout, control buttons and information is built here.
	explicit XRFViewer(QWidget *parent = 0);

signals:

public slots:

protected slots:
	/// Loads a spectra file in and puts everything in its place.
	void loadFile();
	/// Handles enabling/disabling of elements based on which button is clicked in the dead time button group.
	void elementClicked(int elementId);
	/// Handles when the log scale button is clicked.  It takes a log of the data if true.
	void onLogEnabled(bool logged);
	/// Handles what happens when a spectra group button specified by \code id is clicked.
	void onSpectraGroupClicked(int id);
	/// Changes the amount of waterfall separation between the plots.
	void onWaterfallSeparationChanged(double val) { plot_->setAxisScaleWaterfall(MPlot::Left, val*getMaximumHeight(plot_->item(0))); }

protected:
	/// Sets up the plot.  Can add the plot widget to the layout after calling this function.
	void setupPlot();
	/// Gets the maximum height from the first data source.  Used for scaling the height of the ROI markers.
	double getMaximumHeight(MPlotItem *data);
	/// Get a color for the color of a line on the plot.
	QColor getColor(int index);

	/// The elapsed time label.
	QLabel *elapsedTime_;
	/// The dead time label.
	QLabel *deadTime_;
	/// The region of interest text edit.
	QTextEdit *roiList_;

	/// The waterfall spin box.
	QDoubleSpinBox *waterfallSeparation_;
	/// The log button.
	QPushButton *logButton_;

	/// The button group used for the dead time tool buttons.
	QButtonGroup *deadTimeGroup_;
	/// The layout that holds the dead time buttons.
	QHBoxLayout *deadTimeLayout_;

	/// This is the plot widget that holds the plot used for viewing the spectra.
	MPlotWidget *view_;
	/// This is the plot itself.
	MPlot *plot_;
	/// This holds the raw data sources.
	QList<MPlotSeriesBasic *> rawDataSeries_;
	/// This holds the corrected data sources.
	QList<MPlotSeriesBasic *> corrDataSeries_;
	/// This holds the corrected sum.
	MPlotSeriesBasic *corrSum_;
};

#include <QString>
#include <QList>

#include "MPlot/MPlotSeriesData.h"

/*! This waveform series uses multiple inheritance.  It uses MPlotAbstractSeriesData
	for combining the actual data to an MPlot object.
*/
class SpectrumData : public MPlotAbstractSeriesData
{

public:
	/// Constructor for the series.  It requires a QList of data..
	SpectrumData(const QList<double> &data)
		: MPlotAbstractSeriesData()
	{
		data_ = data;
		scale_ = 1;
	}

	/// Returns the x value for the given index.
	virtual double x(unsigned index) const { return scale_*index; }
	/// Returns the y value for the given index.
	virtual double y(unsigned index) const { return data_.at(index); }

	/// Return the number of elements.
	virtual int count() const { return data_.size(); }


	/// Return the bounds of the data (the rectangle containing the max/min x- and y-values).
	virtual QRectF boundingRect() const
	{
		double minY = searchMinY();
		double maxY = searchMaxY();
		double xMax = data_.size()*scale_;

		// Need to ensure that the bounding rect has a non-zero height.
		if (maxY == 0)
			maxY = 1;

		return QRectF(0, minY, xMax, maxY-minY);
	}

	/// Return the maximum value from the y axis.
	double maxY(){ return searchMaxY(); }

	/// Return the minimum value from the y axis.
	double minY(){ return searchMinY(); }

	/// Return the maximum value from the x axis.
	double maxX(){ return data_.size()*scale_; }

	/// Return the minimum value from the x axis.
	double minX(){ return 0; }

	/// Sets the scale for the x-axis.
	void setScale(double scale) { scale_ = scale; emitDataChanged(); }
	/// Returns the scale for the x-axis.
	double scale() const { return scale_; }

private:
	/// The data.
	QList<double> data_;

	/// The x-axis scaler.
	double scale_;

	/// Searches for the maximum y value in the waveform.
	double searchMinY() const
	{
		if(count() == 0)
				return -1;

			double min = y(0);

			for(int i=1; i<count(); i++) {
				if(y(i) < min)
					min = y(i);
			}

			return min;
	}
	/// Searchs for the minimum y value in the waveform.
	double searchMaxY() const
	{
		if(count() == 0)
				return -1;

			double max = y(0);

			for(int i=1; i<count(); i++) {
				if(y(i) > max)
					max = y(i);
			}

			return max;
	}

};

#endif // XRFVIEWER_H
