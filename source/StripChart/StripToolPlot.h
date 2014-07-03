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


#ifndef STRIPTOOLPLOT_H
#define STRIPTOOLPLOT_H

#include <QtGui>

#include "MPlot/MPlot.h"
#include "MPlot/MPlotItem.h"
#include "MPlot/MPlotTools.h"
#include "MPlot/MPlotWidget.h"

#include "StripChart/StripToolModel.h"
#include "StripChart/StripToolSelector.h"

/// This class displays pv information as a plot. A series may be added or removed from the plot, and displays the units of a series when it's selected.

class StripToolPlot : public MPlotWidget
{
    Q_OBJECT

public:
    explicit StripToolPlot(QWidget *parent = 0);
    virtual ~StripToolPlot();
    
signals:
    void selectionChanged(MPlotItem *plotSelection);

public:
    bool contains(MPlotItem *series);
    MPlotItem* selectedItem() const;

public slots:
    bool addPlotItem(MPlotItem *newSeries);
    bool removePlotItem(MPlotItem *toRemove);
    void setSelectedItem(MPlotItem *newSelection);
    void setLeftAxisName(const QString &newName);
    void setLeftAxisRange(const MPlotAxisRange *newAxisRange);
    void setBottomAxisName(const QString &newName);
    void setWaterfall(bool waterfallOn);

protected slots:
    void onSelectionChange();

private:
    void buildComponents();
    void makeConnections();
    void defaultSettings();

private:
    MPlot *plot_;
    StripToolSelector *selector_;
    bool waterfallOn_;

};

#endif // STRIPTOOLPLOT_H
