#ifndef CLSBEAMLINE_H
#define CLSBEAMLINE_H

#include "beamline/AMBeamline.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"
#include "beamline/CLS/CLSJJSlits.h"

class CLSBeamline : public AMBeamline
{
    Q_OBJECT

public:
	/// Singleton accessor.
	static CLSBeamline *clsBeamline()
	{
		if (!instance_) {
			instance_ = new CLSBeamline("CLS Beamline");
			instance_->initializeBeamlineSupport();
		}

		return static_cast<CLSBeamline*>(instance_);
	}

	/// Destructor.
	virtual ~CLSBeamline();

	/// Returns the beamline's scaler.
	virtual CLSSIS3820Scaler* scaler() const { return 0; }
	/// Returns the beamline's JJ slits.
	virtual CLSJJSlits* jjSlits() const { return 0; }

protected:
	/// Protected constructor, for singleton pattern.
	CLSBeamline(const QString &controlName);

};

#endif // CLSBEAMLINE_H
