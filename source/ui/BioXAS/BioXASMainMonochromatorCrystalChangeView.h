#ifndef BIOXASMAINMONOCRYSTALCHANGEVIEW_H
#define BIOXASMAINMONOCRYSTALCHANGEVIEW_H

#include <QWidget>
#include <QLabel>
#include <QLayout>
#include <QProgressBar>
#include <QMessageBox>

#include "beamline/BioXAS/BioXASMainMonochromator.h"

class BioXASMainMonochromatorCrystalChangeView : public QWidget
{
    Q_OBJECT

public:
    /// Constructor.
    explicit BioXASMainMonochromatorCrystalChangeView(BioXASMainMonochromator *mono, QWidget *parent);
    /// Destructor.
    virtual ~BioXASMainMonochromatorCrystalChangeView();

signals:
    /// Notifier that the monochromator being viewed has changed.
    void monoChanged(BioXASMainMonochromator *newMono);

public slots:
    /// Sets the monochromator being viewed.
    void setMono(BioXASMainMonochromator *newMono);

protected:
    /// The monochromator being viewed.
    BioXASMainMonochromator *mono_;
};



#endif // BIOXASMAINMONOCRYSTALCHANGEVIEW_H
