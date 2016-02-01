#ifndef BIOXASMAINMONOCHROMATOR_H
#define BIOXASMAINMONOCHROMATOR_H

#include "beamline/BioXAS/BioXASSSRLMonochromator.h"

class BioXASMainMonochromator : public BioXASSSRLMonochromator
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASMainMonochromator(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASMainMonochromator();

	/// Returns the control monitoring the difference between the encoder and steps feedback (degrees);
	AMReadOnlyPVControl* encoderStepsDiffControl() const { return encoderStepsDiff_; }

protected:
	/// Control that reports changes between the encoder feedback and the steps feedback. Value is in degrees.
	AMReadOnlyPVControl *encoderStepsDiff_;
};


#endif // BIOXASMAINMONOCHROMATOR_H
