#ifndef SGMTURBOPUMP_H
#define SGMTURBOPUMP_H

#include "beamline/AMControl.h"
#include "beamline/AMPVControl.h"

class SGMTurboPump : public AMControl
{
	Q_OBJECT

public:
	/// Constructor.
	explicit SGMTurboPump(const QString &name, const QString &baseName, QObject *parent = 0);
	/// Destructor.
	virtual ~SGMTurboPump();

	/// Returns the current connected state.
	virtual bool isConnected() const;

	/// Returns the running state control.
	AMSinglePVControl* running() const { return running_; }

protected slots:
	/// Sets the connected state.
	void setConnected(bool isConnected);
	/// Updates the connected state.
	void updateConnected();

protected:
	/// The current connected state.
	bool connected_;
	/// The running state control.
	AMSinglePVControl *running_;
};

#endif // SGMTURBOPUMP_H
