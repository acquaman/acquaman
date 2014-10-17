/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#ifndef STRIPTOOLSERIES2_H
#define STRIPTOOLSERIES2_H

#include "MPlot/MPlotSeries.h"
#include <QDebug>

class StripToolSeries2 : public QObject
{
	Q_OBJECT

public:
	StripToolSeries2(QObject *parent = 0);

signals:
	void dataRangeUpdated(MPlotAxisRange* newRange);
	void displayRangeUpdated(const MPlotAxisRange* newRange);

public:
	MPlotSeriesBasic* series() const;
	MPlotAxisRange* displayedRange();
	MPlotAxisRange* dataRange();

	qreal dataMin() const;
	qreal dataMax() const;

	bool customMinDefined() const;
	bool customMaxDefined() const;
	double customMin() const;
	double customMax() const;

	double displayedMin() const;
	double displayedMax() const;

	bool waterfallApplied() const;
	qreal waterfallMin() const;
	qreal waterfallMax() const;

public slots:
	void setData(MPlotVectorSeriesData* newData);

	void setCustomMin(qreal min);
	void setCustomMax(qreal max);
	void eraseCustomMin();
	void eraseCustomMax();

	void setWaterfallMin(qreal min);
	void setWaterfallMax(qreal max);
	void eraseWaterfallLimits();

	void enableYNormalization(bool normOn, qreal ymin_axis, qreal ymax_axis);
	void enableWaterfall(bool waterfallOn, int itemPosition, int itemCount);

protected:
	void setWaterfallLimits(qreal min, qreal max);
	void applyWaterfall(int itemPosition, int itemCount);

/*
protected slots:
	void onDataUpdate();
*/

private:
	void buildComponents();
	void makeConnections();
	void defaultSettings();


private:
	bool customMinDefined_;
	bool customMaxDefined_;

	double customMin_;
	double customMax_;

	bool waterfallApplied_;

	double waterfallMin_;
	double waterfallMax_;

	MPlotSeriesBasic* series_;
};

#endif // STRIPTOOLSERIES2_H
