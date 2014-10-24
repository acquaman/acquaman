#ifndef BIOXASSIDESCALERVIEW_H
#define BIOXASSIDESCALERVIEW_H

#include <QWidget>

#include "beamline/BioXAS/BioXASSideBeamline.h"
#include "ui/CLS/CLSSIS3820ScalerView.h"

class BioXASSideScalerView : public QWidget
{
    Q_OBJECT

public:
    /// Constructor.
    explicit BioXASSideScalerView(QWidget *parent = 0);
    /// Destructor.
    virtual ~BioXASSideScalerView();

signals:

public slots:

protected slots:
    /// Updates all amplifier views to match the most recent change.
    void onAmplifierViewModeChanged(AMCurrentAmplifierView::ViewMode newMode);
    /// Updates all output views to match the most recent change.
    void onOutputViewModeChanged(CLSSIS3820ScalerChannelView::OutputViewMode newMode);

protected:
    /// The list of all scaler channel views.
    QList<CLSSIS3820ScalerChannelView*> channelViews_;
};

#endif // BIOXASSIDESCALERVIEW_H
