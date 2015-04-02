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

#define CLSSIS3820SCALERCHANNELVIEWWITHDARKCURRENT_ACTION_FAILED 2348970

class CLSSIS3820Scaler;

class CLSSIS3820ScalerChannelViewWithDarkCurrent : public CLSSIS3820ScalerChannelView
{
    Q_OBJECT

public:
    /// Enum indicates what view modes are available to display dark current information: Hide keeps darkCurrentContent_ hidden, Show displays darkCurrentContent_ widget.
    enum DarkCurrentViewMode { Hide = 0, Show };

    /// Constructor.
    explicit CLSSIS3820ScalerChannelViewWithDarkCurrent(CLSSIS3820ScalerChannel *channel, QWidget *parent = 0);
    /// Destructor.
    virtual ~CLSSIS3820ScalerChannelViewWithDarkCurrent();
    /// Returns the current dark current view mode.
    CLSSIS3820ScalerChannelViewWithDarkCurrent::DarkCurrentViewMode darkCurrentViewMode() const { return darkCurrentViewMode_; }

signals:
    /// Emitted when the dark current view mode has been changed.
    void darkCurrentViewModeChanged(CLSSIS3820ScalerChannelViewWithDarkCurrent::DarkCurrentViewMode newMode);

public slots:
    /// Shows the dark current view.
    void show();
    /// Hides the dark current view.
    void hide();
    /// Reimplements CLSSIS3820ScalerChannelView::setOutputViewMode, extends functionality to include showing the dark current info if the mode is Counts, hiding it for Voltage.
    virtual void setOutputViewMode(CLSSIS3820ScalerChannelView::OutputViewMode mode);
    /// Sets the dark current value label with the given value.
    void setDarkCurrentValue(double newValue);
    /// Sets the dark current corrected measurement label with the given value.
    void setDarkCurrentCorrected(double newValue);
    /// Sets the valid-status of the current dark current value and corrected measurement. 'Stale' values are red, valid values are blue. Black values are the default, they haven't been set yet.
    void setDarkCurrentState(bool valid);

protected slots:
    /// Handles updating the display once a new dark current measurement has been made.
    void onDetectorDarkCurrentValueChanged(double newValue);
    /// Reimplements CLSSIS3820ScalerChannelView::onReadingChanged(), extends functionality to include updating the displayed dark current corrected measurement.
    virtual void onReadingChanged();

protected:
    /// Returns the latest measurement value, normalized by the acquisition time. Returns zero if no previous measurement made, or if the acquisition time is invalid.
    double lastMeasurement();

private slots:
    /// Sets the dark current view mode.
    void setDarkCurrentViewMode(DarkCurrentViewMode newMode);

private:
    /// Enum value indicating whether or not to display dark current information.
    DarkCurrentViewMode darkCurrentViewMode_;

    /// Label displaying the latest dark current value measurement information. The status of this information (whether or not it's stale) is indicated by label color.
    QLabel *darkCurrentValueLabel_;
    /// Label displaying the latest dark current corrected measurement information.
    QLabel *darkCurrentCorrectedLabel_;

};

#endif // CLSSIS3820SCALERCHANNELVIEWWITHDARKCURRENT_H
