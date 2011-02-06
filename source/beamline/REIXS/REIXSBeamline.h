#ifndef REIXSBEAMLINE_H
#define REIXSBEAMLINE_H

#include "beamline/AMControl.h"

class REIXSBeamline : public AMControl
{
	Q_OBJECT
public:
	explicit REIXSBeamline(QObject *parent = 0);

signals:

public slots:

};

#endif // REIXSBEAMLINE_H
