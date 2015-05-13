#ifndef BIOXASMAINMONOCHROMATOR_H
#define BIOXASMAINMONOCHROMATOR_H

#include <QObject>

#include "beamline/BioXAS/BioXASSSRLMonochromator.h"

class BioXASMainMonochromator : public BioXASSSRLMonochromator
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASMainMonochromator(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASMainMonochromator();
};


#endif // BIOXASMAINMONOCHROMATOR_H
