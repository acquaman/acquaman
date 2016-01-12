#ifndef BIOXASJJSLITS_H
#define BIOXASJJSLITS_H

#include "beamline/BioXAS/BioXASBeamlineComponent.h"

class AMControl;

class BioXASJJSlits : public BioXASBeamlineComponent
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASJJSlits(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASJJSlits();

	/// Returns the upper blade motor control.
	AMControl* upperBlade() const { return upperBlade_; }
	/// Returns the lower blade motor control.
	AMControl* lowerBlade() const { return lowerBlade_; }
	/// Returns the inboard blade motor control.
	AMControl* inboardBlade() const { return inboardBlade_; }
	/// Returns the outboard blade motor control.
	AMControl* outboardBlade() const { return outboardBlade_; }

signals:
	/// Notifier that the upper blade motor control has changed.
	void upperBladeChanged(AMControl *newControl);
	/// Notifier that the lower blade motor control has changed.
	void lowerBladeChanged(AMControl *newControl);
	/// Notifier that the inboard blade motor control has changed.
	void inboardBladeChanged(AMControl *newControl);
	/// Notifier that the outboard blade motor control has changed.
	void outboardBladeChanged(AMControl *newControl);

public slots:
	/// Sets the upper blade motor control.
	void setUpperBlade(AMControl *newControl);
	/// Sets the lower blade motor control.
	void setLowerBlade(AMControl *newControl);
	/// Sets the inboard blade motor control.
	void setInboardBlade(AMControl *newControl);
	/// Sets the outboard blade motor control.
	void setOutboardBlade(AMControl *newControl);

protected:
	/// Upper blade motor control.
	AMControl *upperBlade_;
	/// Lower blade motor control.
	AMControl *lowerBlade_;
	/// Inboard blade motor control.
	AMControl *inboardBlade_;
	/// Outboard blade motor control.
	AMControl *outboardBlade_;
};

#endif // BIOXASJJSLITS_H
