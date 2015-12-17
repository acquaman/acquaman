#ifndef BIOXASSSRLMONOCHROMATORMASK_H
#define BIOXASSSRLMONOCHROMATORMASK_H

#include "beamline/BioXAS/BioXASBeamlineComponent.h"

class BioXASSSRLMonochromatorMask : public BioXASBeamlineComponent
{
    Q_OBJECT

public:
	/// Enumeration of different state values.
	enum State { Open = 0, Closed = 1 };

	/// Constructor.
	explicit BioXASSSRLMonochromatorMask(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSSRLMonochromatorMask();

	/// Returns the upper blade control.
	AMControl* upperBlade() const { return upperBlade_; }
	/// Returns the lower blade control.
	AMControl* lowerBlade() const { return lowerBlade_; }
	/// Returns the state control.
	AMControl* state() const { return state_; }

signals:
	/// Notifier that the upper slit blade control has changed.
	void upperBladeChanged(AMControl *newControl);
	/// Notifier that the lower slit blade control has changed.
	void lowerBladeChanged(AMControl *newControl);
	/// Notifier that the state control has changed.
	void stateChanged(AMControl *newControl);

public slots:
	/// Sets the upper blade control.
	void setUpperBlade(AMControl *newControl);
	/// Sets the lower blade control.
	void setLowerBlade(AMControl *newControl);
	/// Sets the state control.
	void setState(AMControl *newControl);

protected:
	/// The upper blade control.
	AMControl *upperBlade_;
	/// The lower blade control.
	AMControl *lowerBlade_;
	/// The state control.
	AMControl *state_;
};

#endif // BIOXASSSRLMONOCHROMATORMASK_H
