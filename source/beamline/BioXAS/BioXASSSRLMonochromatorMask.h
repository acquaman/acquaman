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

	/// Returns the upper slit blade control.
	AMControl* upperSlitBlade() const { return upperSlitBlade_; }
	/// Returns the lower slit blade control.
	AMControl* lowerSlitBlade() const { return lowerSlitBlade; }
	/// Returns the state control.
	AMControl* state() const { return state_; }

signals:
	/// Notifier that the upper slit blade control has changed.
	void upperSlitBladeChanged(AMControl *newControl);
	/// Notifier that the lower slit blade control has changed.
	void lowerSlitBladeChanged(AMControl *newControl);
	/// Notifier that the state control has changed.
	void stateChanged(AMControl *newControl);

public slots:
	/// Sets the upper slit blade control.
	void setUpperSlitBlade(AMControl *newControl);
	/// Sets the lower slit blade control.
	void setLowerSlitBlade(AMControl *newControl);
	/// Sets the state control.
	void setState(AMControl *newControl);

protected:
	/// The upper slit blade control.
	AMControl *upperSlitBlade_;
	/// The lower slit blade control.
	AMControl *lowerSlitBlade_;
	/// The state control.
	AMControl *state_;
};

#endif // BIOXASSSRLMONOCHROMATORMASK_H
