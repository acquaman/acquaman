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


#include "StripToolPlot.h"

StripToolPlot::StripToolPlot(QWidget *parent) : MPlotWidget(parent)
{
    selector_ = 0;
    plot_ = 0;

    buildComponents();
    makeConnections();
    defaultSettings();

    qDebug() << "StripToolPlot object created.";
}



StripToolPlot::~StripToolPlot()
{
}



bool StripToolPlot::contains(MPlotItem *series)
{
    return plot_->plotItems().contains(series);
}



MPlotItem* StripToolPlot::selectedItem() const {
    return selector_->selectedItem();
}



bool StripToolPlot::addPlotItem(MPlotItem *newSeries)
{    
    bool success = false;

    if (newSeries == 0) {
        qDebug() << "StripToolPlot :: cannot add a series with an empty pointer! No changes made.";
        success = false;

    } else if (contains(newSeries)) {
        qDebug() << "StripToolPlot :: cannot add duplicate series to plot. No changes made.";
        success = false;

    } else {
        int oldItemNum = plot_->plotItems().size();
        plot()->addItem(newSeries);
        int newItemNum = plot_->plotItems().size();

        if (contains(newSeries) && newItemNum == oldItemNum + 1) {
            qDebug() << "StripToolPlot :: added new series to plot successfully.";
            success = true;

        } else {
            qDebug() << "StripToolPlot :: adding new series to plot failed.";
            success = false;
        }
    }

    return success;
}



bool StripToolPlot::removePlotItem(MPlotItem *toRemove)
{
    if (toRemove == 0) {
        qDebug() << "StripToolPlot :: cannot remove a series with an empty pointer! No changes made.";
        return false;
    }

    if (!contains(toRemove)) {
        qDebug() << "StripToolPlot :: cannot remove a series that the plot doesn't contain! No changes made.";
        return false;
    }

    int oldItemNum = plot_->plotItems().size();
    plot()->removeItem(toRemove);
    int newItemNum = plot_->plotItems().size();

    if (!contains(toRemove) && newItemNum == oldItemNum - 1) {
        return true;

    } else {
        qDebug() << "StripToolPlot :: removing series failed.";
        return false;
    }

}



void StripToolPlot::setSelectedItem(MPlotItem *newSelection)
{
    if (contains(newSelection)) {
        selector_->setSelection(newSelection);
        qDebug() << "StripToolPlot::setSelectedItem(...) : selection changed.";
    }
}



void StripToolPlot::setLeftAxisName(const QString &newName) {
    plot_->axisLeft()->setAxisName(newName);
    plot_->axisLeft()->showAxisName(true);
}



void StripToolPlot::setLeftAxisRange(const MPlotAxisRange *newAxisRange) {
    qreal rangeMin = newAxisRange->min();
    qreal rangeMax = newAxisRange->max();

    plot_->axisScaleLeft()->setDataRange(newAxisRange->normalized(), true);

    int itemCount = plot_->plotItems().size();

    for (int i = 0; i < itemCount; i++) {
        StripToolSeries* series = qgraphicsitem_cast<StripToolSeries*>(plot_->plotItems().at(i));
        series->enableWaterfall(waterfallOn_, i, itemCount);
        series->enableYNormalization(true, rangeMin, rangeMax);
    }
}



void StripToolPlot::setBottomAxisName(const QString &newName) {
    plot_->axisBottom()->setAxisName(newName);
}



void StripToolPlot::setWaterfall(bool waterfallOn)
{
    waterfallOn_ = waterfallOn;

    if (waterfallOn) {
        qDebug() << "StripToolPlot :: waterfall feature is turning on:";
    } else {
        qDebug() << "StripToolPlot :: waterfall feature is turning off:";
    }

    int count = plot_->plotItems().size();

    for (int i = 0; i < count; i++) {
        StripToolSeries* series = qgraphicsitem_cast<StripToolSeries*>(plot_->plotItems().at(i));
        series->enableWaterfall(waterfallOn_, i, count);
    }
}



void StripToolPlot::onSelectionChange()
{
    MPlotItem *newSelection = selector_->selectedItem();

    if (newSelection == 0) {
        plot_->axisLeft()->showTickLabels(false);
        plot_->axisLeft()->showAxisName(false);
        emit selectionChanged(0);

    } else {
        plot_->axisLeft()->showTickLabels(true);
        plot_->axisLeft()->showAxisName(true);
        emit selectionChanged(newSelection);
    }
}



void StripToolPlot::buildComponents()
{
    MPlotDragZoomerTool *dragZoomer = new MPlotDragZoomerTool();
    selector_ = new StripToolSelector();
    plot_ = new MPlot();

    plot_->addTool(dragZoomer);
    plot_->addTool(selector_);

    setPlot(plot_);
}



void StripToolPlot::makeConnections()
{
    connect( selector_, SIGNAL(itemSelected(MPlotItem*)), this, SLOT(onSelectionChange()) );
    connect( selector_, SIGNAL(deselected()), this, SLOT(onSelectionChange()) );
}



void StripToolPlot::defaultSettings()
{
    waterfallOn_ = false;

    plot_->axisBottom()->setAxisName("Time");
    plot_->axisBottom()->showAxisName(true);
    plot_->axisBottom()->showTickLabels(true);

    plot_->axisLeft()->showAxisName(false);
    plot_->axisLeft()->showTickLabels(false);

    plot_->enableAxisNormalization(MPlot::Left, false);
    plot_->axisScaleLeft()->setAutoScaleEnabled(false);
}


