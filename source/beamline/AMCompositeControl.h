#ifndef AMCOMPOSITECONTROL_H
#define AMCOMPOSITECONTROL_H

#include "beamline/AMControl.h"

/// This subclass of AMControl monitors its childControls() and uses that to determine canMeasure(), canMove(), isConnected(), and isMoving().  Watches signals from the children to catch changes in connected() and movingChanged().
class AMCompositeControl : public AMControl
{
	Q_OBJECT
public:
	explicit AMCompositeControl(const QString& name, const QString& units = "n/a", QObject* parent = 0, const QString description = "");

	virtual void addChildControl(AMControl *control);

	/// returns true if any chidren are moving
	virtual bool isMoving() const {
		bool isMoving = false;
		foreach(AMControl* c, children_) {
			isMoving |= c->isMoving();
		}
		return isMoving;
	}

	/// returns true if all childre are connected
	virtual bool isConnected() const {
		bool connected = true;
		foreach(AMControl* c, children_) {
			connected &= c->isConnected();
		}
		return connected;
	}

	/// returns true if all children canMeasure
	virtual bool canMeasure() const {
		bool canMeasure = true;
		foreach(AMControl* c, children_) {
			canMeasure &= c->canMeasure();
		}
		return canMeasure;
	}

	/// returns true if all children canMove
	virtual bool canMove() const {
		bool canMove = true;
		foreach(AMControl* c, children_) {
			canMove &= c->canMove();
		}
		return canMove;
	}


signals:

public slots:

protected slots:
	void onChildControlConnected();
	void onChildControlMovingChanged();

protected:
	/// used to detect changes in the moving state
	bool wasMoving_;

};

#endif // AMCOMPOSITECONTROL_H
