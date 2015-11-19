#ifndef SGMVATVALVE_H
#define SGMVATVALVE_H

#include "beamline/SGM/SGMControl.h"
#include "beamline/AMPVControl.h"

class SGMVATValve : public SGMControl
{
	Q_OBJECT

public:
	/// Constructor.
	explicit SGMVATValve(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~SGMVATValve();

	/// Returns the position.
	AMSinglePVControl* position() const { return position_; }
	/// Returns the speed.
	AMSinglePVControl* speed() const { return speed_; }

protected:
	/// The VAT valve position.
	AMSinglePVControl* position_;
	/// The VAT valve speed.
	AMSinglePVControl* speed_;
};

#endif // SGMVATVALVE_H
