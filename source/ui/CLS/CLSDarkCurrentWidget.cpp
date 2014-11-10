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


#include "CLSDarkCurrentWidget.h"

CLSDarkCurrentWidget::CLSDarkCurrentWidget(double dwellSeconds, QWidget *parent) :
    QWidget(parent)
{
    dwellTime_ = 0;

    if (dwellSeconds > 0)
        dwellTime_ = dwellSeconds;

    QLabel *widgetTitle = new QLabel("Dark Current Correction");

    QLabel *dwellTimeLabel = new QLabel("Dwell time (s): ");

    dwellTimeEntry_ = new QDoubleSpinBox(this);
    dwellTimeEntry_->setMinimum(0);
    dwellTimeEntry_->setMaximum(100);
    dwellTimeEntry_->setValue(dwellTime_);

    darkCurrentButton_ = new QPushButton("Collect");
    connect( darkCurrentButton_, SIGNAL(clicked()), this, SLOT(onDarkCurrentButtonClicked()) );

    QGridLayout *layout = new QGridLayout();
    layout->addWidget(widgetTitle, 0, 0);
    layout->addWidget(dwellTimeLabel, 1, 0);
    layout->addWidget(dwellTimeEntry_, 1, 1);
    layout->addWidget(darkCurrentButton_, 2, 1);

    setLayout(layout);
}

CLSDarkCurrentWidget::~CLSDarkCurrentWidget()
{

}

void CLSDarkCurrentWidget::setDarkCurrentButtonEnabled(bool isEnabled)
{
    darkCurrentButton_->setEnabled(isEnabled);
}

void CLSDarkCurrentWidget::onDarkCurrentButtonClicked()
{
    double dwellEntered = dwellTimeEntry_->text().toDouble();

    if (dwellEntered > 0) {
        dwellTime_ = dwellEntered;
        emit collectDarkCurrentMeasurement(dwellTime_);
    }
}
