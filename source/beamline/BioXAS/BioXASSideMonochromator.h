#ifndef BIOXASSIDEMONOCHROMATOR_H
#define BIOXASSIDEMONOCHROMATOR_H

#include <QObject>

#include "beamline/BioXAS/BioXASSSRLMonochromator.h"

class BioXASSideMonochromator : public BioXASSSRLMonochromator
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSideMonochromator(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSideMonochromator();

public slots:
//	/// Reimplemented for testing.
//	virtual bool stop() const;
};

#endif // BIOXASSIDEMONOCHROMATOR_H
