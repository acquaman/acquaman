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

    /// Enum indicates what view modes are available to display dark current information: Hide keeps darkCurrentContent_ hidden, Show displays darkCurrentContent_ widget.
    enum DarkCurrentViewMode {
        Hide = 0,
        Show = 1
    };

    explicit CLSSIS3820ScalerChannelViewWithDarkCurrent(CLSSIS3820ScalerChannel *channel, QWidget *parent = 0);
    virtual ~CLSSIS3820ScalerChannelViewWithDarkCurrent();

signals:
    /// Emitted when darkCurrentViewMode_ is set to a new value.
    void darkCurrentViewModeChanged(CLSSIS3820ScalerChannelViewWithDarkCurrent::DarkCurrentViewMode newMode);

public:
    /// Returns the current dark current view mode.
    CLSSIS3820ScalerChannelViewWithDarkCurrent::DarkCurrentViewMode darkCurrentViewMode();
    /// Sets the dark current view mode--whether or not dark current information is displayed.
    void setDarkCurrentViewMode(DarkCurrentViewMode newMode);

protected slots:
    /// Handles updating the displayed dark current value as updates are made available.
    void onNewDarkCurrentMeasurementValue(double newValue);
    /// Handles updating the status of the displayed dark current value, as updates are made available.
    void onNewDarkCurrentMeasurementState(bool measurementUpToDate);

private:
    /// Enum value indicating whether or not to display dark current information.
    DarkCurrentViewMode darkCurrentViewMode_;

    /// The content that is shown or hidden, depending on the dark current view mode.
    QGroupBox *darkCurrentContent_;

    /// Label displaying the latest dark current value measurement information. The status of this information (whether or not it's stale) is indicated by label color.
    QLabel *darkCurrentValue_;

};

#endif // CLSSIS3820SCALERCHANNELVIEWWITHDARKCURRENT_H
