#ifndef BIOXASSIDESHUTTERS_H
#define BIOXASSIDESHUTTERS_H

#include "beamline/BioXAS/BioXASShutters.h"

class BioXASSideShutters : public BioXASShutters
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSideShutters(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSideShutters();

	/// Returns true if each of the subcontrols is connected.
	virtual bool isConnected() const;

	/// Returns the Side endstation safety shutter.
	virtual AMControl* safetyShutterSide() const { return safetyShutterSide_; }
	/// Returns the Main endstation safety shutter.
	virtual AMControl* safetyShutterMain() const { return safetyShutterMain_; }
	/// Returns the Imaging endstation safety shutter.
	virtual AMControl* safetyShutterImaging() const { return safetyShutterImaging_; }

protected:
	/// The Side endstation safety shutter.
	CLSBiStateControl *safetyShutterSide_;
	/// The Main endstation safety shutter.
	AMReadOnlyPVControl *safetyShutterMain_;
	/// The Imaging endstation safety shutter.
	AMReadOnlyPVControl *safetyShutterImaging_;
};

#endif // BIOXASSIDESHUTTERS_H
