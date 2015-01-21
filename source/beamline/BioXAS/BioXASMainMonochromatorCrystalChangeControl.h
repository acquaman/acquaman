#ifndef BIOXASMAINMONOCHROMATORCRYSTALCHANGECONTROL_H
#define BIOXASMAINMONOCHROMATORCRYSTALCHANGECONTROL_H

#include <QObject>

class BioXASMainMonochromatorCrystalChangeControl : public QObject
{
    Q_OBJECT

public:
    /// Constructor.
    explicit BioXASMainMonochromatorCrystalChangeControl(QObject *parent = 0);
    /// Destructor.
    virtual ~BioXASMainMonochromatorCrystalChangeControl();

signals:

public slots:

};

#endif // BIOXASMAINMONOCHROMATORCRYSTALCHANGECONTROL_H
