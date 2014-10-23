#ifndef BIOXASSIDESCALERVIEW_H
#define BIOXASSIDESCALERVIEW_H

#include <QWidget>

#include "beamline/BioXAS/BioXASSideBeamline.h"

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

};

#endif // BIOXASSIDESCALERVIEW_H
