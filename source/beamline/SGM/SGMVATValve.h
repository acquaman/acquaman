#ifndef SGMVATVALVE_H
#define SGMVATVALVE_H

#include "beamline/AMPVControl.h"

class SGMVATValve : public AMControl
{
	Q_OBJECT

public:
	/// Constructor.
	explicit SGMVATValve(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~SGMVATValve();

	/// Returns true if this control is connected, false otherwise.
	virtual bool isConnected() const;

	/// Returns the position.
	AMSinglePVControl* position() const { return position_; }
	/// Returns the speed.
	AMSinglePVControl* speed() const { return speed_; }

protected slots:
	/// Sets the current connected state.
	void setConnected(bool isConnected);
	/// Updates the current connected state.
	void updateConnected();

protected:
	/// The current connected state.
	bool connected_;

	/// The VAT valve position.
	AMSinglePVControl* position_;
	/// The VAT valve speed.
	AMSinglePVControl* speed_;
};

#endif // SGMVATVALVE_H
