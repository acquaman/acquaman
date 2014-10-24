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


#include "CLSSIS3820ScalerChannelViewWithDarkCurrent.h"

#include <QDebug>

CLSSIS3820ScalerChannelViewWithDarkCurrent::CLSSIS3820ScalerChannelViewWithDarkCurrent(CLSSIS3820ScalerChannel *channel, QWidget *parent) :
    CLSSIS3820ScalerChannelView(channel, parent)
{
    if (channel_->detector()->canDoDarkCurrentCorrection()) {

        // Create and initialize UI elements.

        darkCurrentValue_ = new QLabel();
        setDisplayedDarkCurrentValue(channel_->detector()->darkCurrentMeasurementValue());

        darkCurrentCorrected_ = new QLabel();
        setDisplayedDarkCurrentCorrected(0);

        // Create and set layouts.

        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->addWidget(darkCurrentValue_);
        layout->addWidget(darkCurrentCorrected_);
        channelLayout_->addLayout(layout);

        // Make connections.

        connect( channel_->detector(), SIGNAL(newDarkCurrentMeasurementValueReady(double)), this, SLOT(updateDarkCurrentValue()) );
        connect( channel_->detector(), SIGNAL(requiresNewDarkCurrentMeasurement(bool)), this, SLOT(updateDarkCurrentStatus()) );
        connect( channel_->detector(), SIGNAL(newValuesAvailable()), this, SLOT(updateDarkCurrentCorrected()) );
    }
}

CLSSIS3820ScalerChannelViewWithDarkCurrent::~CLSSIS3820ScalerChannelViewWithDarkCurrent()
{

}

void CLSSIS3820ScalerChannelViewWithDarkCurrent::updateView()
{
    updateDarkCurrentValue();
    updateDarkCurrentStatus();
    updateDarkCurrentCorrected();
}

void CLSSIS3820ScalerChannelViewWithDarkCurrent::updateDarkCurrentValue()
{
    setDisplayedDarkCurrentValue(channel_->detector()->darkCurrentMeasurementValue());
}

void CLSSIS3820ScalerChannelViewWithDarkCurrent::updateDarkCurrentStatus()
{
    if (channel_->detector()->requiresNewDarkCurrentMeasurement())
        darkCurrentValue_->setStyleSheet("color: red;");

    else
        darkCurrentValue_->setStyleSheet("color: blue;");
}

void CLSSIS3820ScalerChannelViewWithDarkCurrent::updateDarkCurrentCorrected()
{
    // grab the latest measurement and dwell time from the detector.

    double *measurement = 0;
    channel_->detector()->data(measurement);

    double dwellTime = channel_->detector()->acquisitionTime();

    // the detector's dark current is normalized by its acquisition time already (see AMDetector::setAsDarkCurrentMeasurementValue()).

    double darkCurrent = channel_->detector()->darkCurrentMeasurementValue();

    double correctedMeasurement = (*measurement / dwellTime) - darkCurrent;
    setDisplayedDarkCurrentCorrected(correctedMeasurement);
}

void CLSSIS3820ScalerChannelViewWithDarkCurrent::setDisplayedDarkCurrentValue(double displayValue)
{
    darkCurrentValue_->setText(QString("Dark current value : %1").arg(displayValue, 0, 'f', 2));
}

void CLSSIS3820ScalerChannelViewWithDarkCurrent::setDisplayedDarkCurrentCorrected(double displayValue)
{
    darkCurrentCorrected_->setText(QString("Corrected measurement : %1").arg(displayValue, 0, 'f', 2));
}




