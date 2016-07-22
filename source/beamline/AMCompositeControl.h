/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef AMCOMPOSITECONTROL_H
#define AMCOMPOSITECONTROL_H

#include "beamline/AMControl.h"

/// This subclass of AMControl monitors its childControls() and uses that to determine canMeasure(), canMove(), isConnected(), and isMoving().  Watches signals from the children to catch changes in connected() and movingChanged().
class AMCompositeControl : public AMControl
{
	Q_OBJECT
public:
	virtual ~AMCompositeControl();
	explicit AMCompositeControl(const QString& name, const QString& units = "", QObject* parent = 0, const QString description = "");

	virtual bool addChildControl(AMControl *control);

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
	virtual void onChildControlConnected();
	virtual void onChildControlMovingChanged();

protected:
	/// used to detect changes in the moving state
	bool wasMoving_;
	/// Used to detect changes in isConnected()
	bool wasConnected_;

};

#endif // AMCOMPOSITECONTROL_H
