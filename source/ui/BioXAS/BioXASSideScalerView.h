#ifndef BIOXASSIDESCALERVIEW_H
#define BIOXASSIDESCALERVIEW_H

#include <QWidget>
#include <QLayout>

#include "beamline/CLS/CLSSIS3820Scaler.h"

#include "ui/CLS/CLSSIS3820ScalerChannelViewWithDarkCurrent.h"

class BioXASSideScalerView : public QWidget
{
    Q_OBJECT

public:
    /// Constructor.
    explicit BioXASSideScalerView(CLSSIS3820Scaler *scaler, QWidget *parent = 0);
    /// Destructor.
    virtual ~BioXASSideScalerView();

protected slots:
    /// Updates all amplifier views to match the most recent change.
    void onAmplifierViewModeChanged(AMCurrentAmplifierView::ViewMode newMode);
    /// Updates all output views to match the most recent change.
    void onOutputViewModeChanged(CLSSIS3820ScalerChannelView::OutputViewMode newMode);

protected:
    /// The scaler being viewed.
    CLSSIS3820Scaler *scaler_;
    /// The list of all scaler channel views.
    QList<CLSSIS3820ScalerChannelView*> channelViews_;
};

#endif // BIOXASSIDESCALERVIEW_H
