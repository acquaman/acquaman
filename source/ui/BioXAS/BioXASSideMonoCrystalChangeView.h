#ifndef BIOXASSIDEMONOCRYSTALCHANGEVIEW_H
#define BIOXASSIDEMONOCRYSTALCHANGEVIEW_H

#include <QWidget>
#include <QLayout>

#include "beamline/BioXAS/BioXASSideMonochromator.h"

class BioXASSideMonoCrystalChangeView : public QWidget
{
    Q_OBJECT

public:
    explicit BioXASSideMonoCrystalChangeView(BioXASSideMonochromator *toView, QWidget *parent = 0);
    virtual ~BioXASSideMonoCrystalChangeView();

    /// Returns the monochromator being viewed.
    BioXASSideMonochromator* mono() const;

signals:
    /// Notifier that the mono being viewed has changed.
    void monoChanged(BioXASSideMonochromator *newMono);

public slots:
    /// Sets the monochromator to be viewed.
    void setMono(BioXASSideMonochromator *toView);

protected slots:
    /// Handles monochromator connection changes.
    void onMonoConnectedChanged();

protected:
    BioXASSideMonochromator *mono_;


};

#endif // BIOXASSIDEMONOCRYSTALCHANGEVIEW_H
