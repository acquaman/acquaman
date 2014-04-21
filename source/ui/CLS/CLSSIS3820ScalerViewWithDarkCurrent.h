#ifndef CLSSIS3820SCALERVIEWWITHDARKCURRENT_H
#define CLSSIS3820SCALERVIEWWITHDARKCURRENT_H

#include "source/beamline/CLS/CLSSIS3820Scaler.h"
#include "CLSSIS3820ScalerView.h"

#include <QLabel>
#include <QGroupBox>

class CLSSIS3820ScalerViewWithDarkCurrent : public CLSSIS3820ScalerView
{
    Q_OBJECT

public:

    /// Enum indicates what view modes are available to display dark current information: Hide keeps darkCurrentContent_ hidden, Show displays darkCurrentContent_ widget.
    enum DarkCurrentViewMode {
        Hide = 0,
        Show = 1
    };

    explicit CLSSIS3820ScalerViewWithDarkCurrent(CLSSIS3820Scaler *scaler, QWidget *parent = 0);
    virtual ~CLSSIS3820ScalerViewWithDarkCurrent();

signals:
    void viewModeChanged(CLSSIS3820ScalerViewWithDarkCurrent::DarkCurrentViewMode);

public:
    /// Returns the current dark current view mode.
    CLSSIS3820ScalerViewWithDarkCurrent::DarkCurrentViewMode darkCurrentViewMode();
    /// Sets the dark current view mode--whether or not dark current information is displayed.
    void setDarkCurrentViewMode(DarkCurrentViewMode newMode);

protected slots:
    /// Handles updating the displayed dark current value as updates are reported by the scaler.
    void onNewScalerDarkCurrentValue(double newMeasurement);
    /// Handles updating the status of the displayed dark current value, as updates are reported by the scaler.
    void onNewScalerDarkCurrentStatus(CLSSIS3820Scaler::DarkCurrentCorrectionState newStatus);

private:
    /// Enum value indicating whether or not to display dark current information.
    DarkCurrentViewMode viewMode_;

    /// The content that is shown or hidden, depending on the dark current view mode.
    QGroupBox *darkCurrentContent_;

    /// Label that displays the latest dark current information. The status of this information (whether or not the measurement is stale) is indicated by label color.
    QLabel *darkCurrentValue_;

};

#endif // CLSSIS3820SCALERVIEWWITHDARKCURRENT_H
