#ifndef BIOXASWIGGLER_H
#define BIOXASWIGGLER_H

#include "beamline/AMConnectedControl.h"
#include "beamline/AMPVControl.h"
#include "beamline/BioXAS/BioXASWigglerGapStatus.h"

class BioXASWiggler : public AMConnectedControl
{
	Q_OBJECT

public:
	/// Constructor.
	BioXASWiggler(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASWiggler();

	/// Returns the gap control.
	AMPVControl* gap() const { return gap_; }
	/// Returns the gap status control.
	BioXASWigglerGapStatus* gapStatus() const { return gapStatus_; }

protected:
	/// The gap control.
	AMPVControl* gap_;
	/// The gap status control.
	BioXASWigglerGapStatus* gapStatus_;
};

#endif // BIOXASWIGGLER_H
