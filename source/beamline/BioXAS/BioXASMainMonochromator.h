#ifndef BIOXASMAINMONOCHROMATOR_H
#define BIOXASMAINMONOCHROMATOR_H

#include "beamline/BioXAS/BioXASSSRLMonochromator.h"

#define BIOXASMAINMONOCHROMATOR_MASK_UPPER_BLADE_CW_DESTINATION -5
#define BIOXASMAINMONOCHROMATOR_MASK_LOWER_BLADE_CW_DESTINATION 5
#define BIOXASMAINMONOCHROMATOR_PADDLE_CCW_DESTINATION -1
#define BIOXASMAINMONOCHROMATOR_BRAGG_CW_DESTINATION -1000
#define BIOXASMAINMONOCHROMATOR_BRAGG_CCW_DESTINATION 1000
#define BIOXASMAINMONOCHROMATOR_CRYSTAL_CHANGE_CW_DESTINATION 15000
#define BIOXASMAINMONOCHROMATOR_CRYSTAL_CHANGE_CCW_DESTINATION -15000

class BioXASMainMonochromator : public BioXASSSRLMonochromator
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASMainMonochromator(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASMainMonochromator();

	/// Returns the control monitoring the difference between the encoder and steps feedback (degrees);
	AMReadOnlyPVControl* encoderStepsDiffControl() const { return encoderStepsDiff_; }

protected:
	/// Control that reports changes between the encoder feedback and the steps feedback. Value is in degrees.
	AMReadOnlyPVControl *encoderStepsDiff_;
};


#endif // BIOXASMAINMONOCHROMATOR_H
