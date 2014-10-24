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


#ifndef CLSSIS3820SCALERCHANNELVIEWWITHDARKCURRENT_H
#define CLSSIS3820SCALERCHANNELVIEWWITHDARKCURRENT_H

#include "CLSSIS3820ScalerView.h"
#include "source/beamline/CLS/CLSSIS3820Scaler.h"

#include <QGroupBox>

class CLSSIS3820Scaler;

class CLSSIS3820ScalerChannelViewWithDarkCurrent : public CLSSIS3820ScalerChannelView
{
    Q_OBJECT

public:
    /// Constructor.
    explicit CLSSIS3820ScalerChannelViewWithDarkCurrent(CLSSIS3820ScalerChannel *channel, QWidget *parent = 0);
    /// Destructor.
    virtual ~CLSSIS3820ScalerChannelViewWithDarkCurrent();

signals:

public slots:
    /// Updates all UI elements.
    void updateView();

protected slots:
    /// Handles updating the displayed dark current value.
    void updateDarkCurrentValue();
    /// Handles updating the status of the displayed dark current value.
    void updateDarkCurrentStatus();
    /// Handles updating the displayed corrected measurement.
    void updateDarkCurrentCorrected();

protected:
    /// Sets the text for the dark current value label.
    void setDisplayedDarkCurrentValue(double displayValue);
    /// Sets the text for the corrected measurement label.
    void setDisplayedDarkCurrentCorrected(double displayValue);

private:
    /// Label displaying the latest dark current value measurement information. The status of this information (whether or not it's stale) is indicated by label color.
    QLabel *darkCurrentValue_;
    /// Label displaying the latest dark current corrected measurement information.
    QLabel *darkCurrentCorrected_;

};

#endif // CLSSIS3820SCALERCHANNELVIEWWITHDARKCURRENT_H
