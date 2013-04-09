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


#ifndef VESPERSXRFVIEWER_H
#define VESPERSXRFVIEWER_H

#include <QWidget>

#include "MPlot/MPlot.h"
#include "MPlot/MPlotWidget.h"
#include "MPlot/MPlotSeries.h"
#include "util/AMSelectablePeriodicTable.h"
#include "ui/util/AMSelectablePeriodicTableView.h"

#include <QLabel>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QTextEdit>

class VESPERSXRFViewer : public QWidget
{
	Q_OBJECT
public:
	/// Builds the viewer.  The general layout, control buttons and information is built here.
	explicit VESPERSXRFViewer(QWidget *parent = 0);

signals:

public slots:

protected slots:
	/// Loads a spectra file in and puts everything in its place.
	void loadFile();
	/// Handles when the log scale button is clicked.  It takes a log of the data if true.
	void onLogEnabled(bool logged);
	/// Handles what happens when a spectra group button specified by \code id is clicked.
	void onSpectraGroupClicked(int id);
	/// Changes the amount of waterfall separation between the plots.
	void onWaterfallSeparationChanged(double val) { plot_->setAxisScaleWaterfall(MPlot::Left, val*getMaximumHeight(plot_->item(0))); }
	/// Helper slot that adds lines to the plot based on elements being selected from the table.
	void onElementSelected(int atomicNumber);
	/// Helper slot that removes lines from the plot based on elements being deselected fromm the table.
	void onElementDeselected(int atomicNumber);
	/// Sets the plot range used for placing markers inside the plot.
	void setPlotRange(double low, double high);
	/// Sets the minimim for the plot range used for placing markers inside the plot.
	void setMinimum(double min);
	/// Sets the maximum for the plot range used for placing markers inside the plot.
	void setMaximum(double max);

protected:
	/// Enum for the different file types.  None is if it can't determine it.
	enum FileType { None, SpectrumSnapshot, VespersXRF, AcquamanXRF };

	/// Checks the data file and returns a FileType.
	FileType checkDataFile(QString filename);
	/// Loads the data file based on Ru's Spectrum snapshot program and fills in the view with the information it can.
	void loadSpectrumSnapshotFile(QString filename);
	/// Loads old VespersXRF data file and fills in the information it can.
	void loadVespersXRFFile(QString filename);
	/// Loads up the current data file format and places all the appropriate information in its associated spots.
	void loadAcquamanXRFFile(QString filename);

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

	/// The button to show the raw spectra in a waterfall plot.
	QPushButton *rawSpectraButton_;
	/// The button to show the corrected spectra in a waterfall plot.
	QPushButton *correctedSpectraButton_;
	/// The button to show the corrected sum.
	QPushButton *correctedSumButton_;

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

	/// The periodic table model that holds all of the selected elements.
	AMSelectablePeriodicTable *table_;
	/// The view that looks at the selectable periodic table model.
	AMSelectablePeriodicTableView *tableView_;
	/// Pair that holds the plot range that should be considered.
	QPair<double, double> range_;
	/// Spin box holding the minimum value for the range.
	QDoubleSpinBox *minimum_;
	/// Spin box holding the maximum value for the range.
	QDoubleSpinBox *maximum_;
};

#include <QString>
#include <QList>

#include "MPlot/MPlotSeriesData.h"

/*! This waveform series uses multiple inheritance.  It uses MPlotAbstractSeriesData
	for combining the actual data to an MPlot object.
*/
class VESPERSSpectrumData : public MPlotAbstractSeriesData
{

public:
	/// Constructor for the series.  It requires a QList of data..
	VESPERSSpectrumData(const QList<double> &data)
		: MPlotAbstractSeriesData()
	{
		data_ = data;
		scale_ = 1;
	}

	/// Returns the x value for the given index.
	virtual double x(unsigned index) const { return scale_*index; }
	virtual void xValues(unsigned indexStart, unsigned indexEnd, qreal *outputValues) const { for(unsigned i=indexStart; i<=indexEnd; ++i) *(outputValues++) = scale_*i; }
	/// Returns the y value for the given index.
	virtual double y(unsigned index) const { return data_.at(index); }
	virtual void yValues(unsigned indexStart, unsigned indexEnd, qreal *outputValues) const { for(unsigned i=indexStart; i<=indexEnd; ++i) *(outputValues++) = data_.at(i); }

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
