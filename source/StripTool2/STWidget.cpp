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


#include "STWidget.h"

#include <QDebug>

STWidget::STWidget(QWidget *parent) : QWidget(parent)
{
    ringCurrent_ = new STVariable("PCT1402-01:mA:fbk", this);
    connect( ringCurrent_, SIGNAL(connected(bool)), this, SLOT(onRingCurrentConnected(bool)) );

    plotWidget_ = new STPlotWidget();
    plotWidget_->setPlotName("Storage ring current");
    plotWidget_->showPlotName(true);
    connect( plotWidget_, SIGNAL(showPlotEditor(bool)), this, SLOT(showPlotEditor(bool)) );

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(plotWidget_);

    setLayout(layout);
    setWindowTitle("Storage Ring Current");
}

STWidget::~STWidget()
{

}

void STWidget::onRingCurrentConnected(bool isConnected)
{
    if (isConnected) {
        // add series to plot.
        plotWidget_->plot()->addItem(ringCurrent_->series());
    }
}

void STWidget::showPlotEditor(bool show)
{
    if (show) {
        QDialog editor;
    }
}
