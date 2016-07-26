#ifndef BIOXASWIGGLERGAPSTATUS_H
#define BIOXASWIGGLERGAPSTATUS_H

#include "beamline/AMSingleEnumeratedControl.h"
#include "beamline/AMControl.h"

#define BIOXASWIGGLERGAPSTATUS_GAP_CLOSED_MIN 0.0
#define BIOXASWIGGLERGAPSTATUS_GAP_CLOSED_MAX 11.7
#define BIOXASWIGGLERGAPSTATUS_GAP_OPEN_MIN 11.8
#define BIOXASWIGGLERGAPSTATUS_GAP_OPEN_MAX 300.0

class BioXASWigglerGapStatus : public AMSingleEnumeratedControl
{
	Q_OBJECT

public:
	/// Enumeration of the possible status values.
	enum Value { Closed = 0, Open = 1 };

	/// Constructor.
	BioXASWigglerGapStatus(const QString &name, const QString &baseName, QObject *parent);
	/// Destructor.
	virtual ~BioXASWigglerGapStatus();

	/// Returns true if this control can move right now. False otherwise.
	virtual bool canMove() const { return false; }
	/// Returns true if this control can stop a move right now. False otherwise.
	virtual bool canStop() const { return false; }

	/// Returns true if this control is connected, false otherwise.
	virtual bool isConnected() const;

	/// Returns the gap control.
	AMControl* gap() const { return gap_; }

signals:
	/// Notifier that the gap control has changed.
	void gapChanged(AMControl *newControl);

public slots:
	/// Sets the gap control.
	void setGap(AMControl *newControl);

protected:
	/// The gap control.
	AMControl *gap_;
};

#endif // BIOXASWIGGLERGAPSTATUS_H
