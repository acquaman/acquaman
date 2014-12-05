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

class CLSSIS3820ScalerChannelViewWithDarkCurrent : public CLSSIS3820ScalerChannelView
{
    Q_OBJECT

public:
    /// Constructor.
    explicit CLSSIS3820ScalerChannelViewWithDarkCurrent(CLSSIS3820ScalerChannel *channel, QWidget *parent = 0);
    /// Destructor.
    virtual ~CLSSIS3820ScalerChannelViewWithDarkCurrent();
    /// Returns boolean indicating whether the dark current information is shown for this channel.
    bool darkCurrentShown() const { return darkCurrentShown_; }

public slots:
    /// Sets whether or not dark current information is displayed.
    void setDarkCurrentShown(bool shown);

protected slots:
    /// Handles updating the displayed dark current value as updates are made available.
    void onNewDarkCurrentMeasurementValue(double newValue);
    /// Handles updating the status of the displayed dark current value, as updates are made available.
    void onNewDarkCurrentMeasurementState(bool measurementUpToDate);
    /// Handles updating the displayed corrected measurement, once a measurement with the detector has been made.
    void onDetectorNewValuesAvailable();

protected:
    /// Sets the text for the dark current value label.
    void setDarkCurrentValueLabel(double displayValue);
    /// Sets the text for the corrected measurement label.
    void setCorrectedMeasurementLabel(double displayValue);

protected:
    /// Indicates whether or not the dark current information can be displayed at all.
    bool darkCurrentShowable_;
    /// Indicates whether or not dark current information is displayed.
    bool darkCurrentShown_;
    /// The content that is shown or hidden, depending on darkCurrentShown_.
    QWidget *darkCurrentContent_;
    /// Label displaying the latest dark current value measurement information. The status of this information (whether or not it's stale) is indicated by label color.
    QLabel *darkCurrentValueLabel_;
    /// Label displaying the latest dark current corrected measurement information.
    QLabel *correctedMeasurementLabel_;

};

#endif // CLSSIS3820SCALERCHANNELVIEWWITHDARKCURRENT_H
