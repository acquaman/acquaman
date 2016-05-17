#ifndef BIOXASSSRLMONOCHROMATORMASK_H
#define BIOXASSSRLMONOCHROMATORMASK_H

#include "beamline/BioXAS/BioXASBeamlineComponent.h"

class BioXASSSRLMonochromatorMaskState;

class BioXASSSRLMonochromatorMask : public BioXASBeamlineComponent
{
    Q_OBJECT

public:
	/// Enumeration of different state values.
	enum State { Open = 0, Closed = 1 };

	/// Constructor.
	explicit BioXASSSRLMonochromatorMask(const QString &deviceName, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSSRLMonochromatorMask();

	/// Returns true if the mask is connected, false otherwise.
	virtual bool isConnected() const;

	/// Returns the upper blade control.
	AMControl* upperBlade() const { return upperBlade_; }
	/// Returns the lower blade control.
	AMControl* lowerBlade() const { return lowerBlade_; }
	/// Returns the blades state control.
	AMControl* bladesState() const { return bladesState_; }
	/// Returns the mask state control.
	BioXASSSRLMonochromatorMaskState* state() const { return state_; }

signals:


public slots:


protected slots:
	/// Updates the state control with the upper blade, lower blade, and state PV controls.
	void updateState();

protected:

};

#endif // BIOXASSSRLMONOCHROMATORMASK_H
