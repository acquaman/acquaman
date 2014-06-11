/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef AM_CONTROLSTATE_H
#define AM_CONTROLSTATE_H

#include <QObject>
#include <QList>
#include <QPair>
#include <qdebug.h>
#include "AMControl.h"

/// An AMControlStatus is used by AMControlState to represent an encapsulation of an AMControl.
/*!
  The AMControlStatus holds a small, persistant representation of an AMControl.
  All that is recorded is the name, whether the control can move/measure at the time, whether it should be able to move/measure in principle, the value at the time, and the tolerance.
  This class should not need to be used directly unless the AMControlState class is being subclassed.
  */
//class AMControlStatus : public QObject
//{
//Q_OBJECT
//public:
//	/// Constructor, capable of taking all parameters. Defaults are defined.
//	/// Name defaults to empty string
//	/// Can move/measure at the time defaults to no ("0")
//	/// Should be able to move/measure defaults to no ("0")
//	/// Value defaults to -1, which could be a legitimate value ... ?
//	/// Tolerance defaults to 0, which is not a legitimate value
// 	virtual ~AMControlStatus();
//	explicit AMControlStatus(QString name = "", int can = 0, int should = 0, double value = -1, double tolerance = 0, QObject *parent = 0) : QObject(parent){
//		name_ = name;
//		can_ = can;
//		should_ = should;
//		value_ = value;
//		tolerance_ = tolerance;
//	}

//	/// Returns the saved name.
//	QString name() const { return name_;}
//	/// Returns whether the control can move/measure at the time: 0 cannot move or measure, 1 can measure but not move, 2 can move but not measure, 3 can measure and move
//	int can() const { return can_;}
//	/// Returns whether the control should be able to move/measure: 0 should not move or measure, 1 should measure but not move, 2 should move but not measure, 3 should move and measure.
//	int should() const { return should_;}
//	/// Returns the value recorded for the control.
//	double value() const { return value_;}
//	/// Returns the tolerance for the control.
//	double tolerance() const { return tolerance_;}

//protected:
//	/// Holds the name of the control (from AMControl's objectName() function)
//	QString name_;
//	/// Holds whether the control can move/measure at the time: 0 cannot move or measure, 1 can measure but not move, 2 can move but not measure, 3 can measure and move
//	int can_;
//	/// Holds whether the control should be able to move/measure: 0 should not move or measure, 1 should measure but not move, 2 should move but not measure, 3 should move and measure.
//	int should_;
//	/// Holds the control's current value at the time.
//	double value_;
//	/// Holds the control's current tolerance at the time.
//	double tolerance_;
//};

///// An AMControlState holds a snapshot of an AMControl and its children.
///*!
//  The AMControlState can be used to save and restore the state of an AMControl.
//  Calling the constructor on a control saves the state of the control at that time.
//  Calling restore on a control sets it back to the saved state.
//  Calling restoreList on a control sets that list of children back to the saved state.
//  \todo Get rid of AMControl pointers in restore functions?
//  */
//class AMControlState : public QObject
//{
//Q_OBJECT
//public:
//	/// Constructor, takes an AMControl and saves its current state. Also takes a QObject as a parent.
// 	virtual ~AMControlState();
//	explicit AMControlState(AMControl *ctrl, QObject *parent = 0);

//signals:

//public slots:
//	void vomit();
//	/// Restores the control back to the saved state. Returns false if not the right AMControl.
//	bool restore(AMControl *ctrl);
//	/// Restores the control's children in the list back to the saved state. Returns false if not the right AMControl.
//	/// The mask list is a list of the names (objectName() function) for the children control in question.
//	bool restoreList(AMControl *ctrl, QList<QString> *maskList);

//protected:
////    QList< QPair<QString, QString> > state_;
//	/// Holds the name of the control (objectName() function).
//	QString name_;
//	/// Holds the list of AMControlStatus for direct children.
//	QList<AMControlStatus*> state_;
//	/// Holds a recursive list of AMControlState for the children.
//	/// That is, child number and a control state for that child. That way, the tree can go down without limit.
//	QList< QPair<int, AMControlState*> > subState_;

//	/// Protected function called in constructor to get state information. Called recursively on children with children.
//	bool searchChildren(AMControl *ctrl);
//	/// Protected function called by both restore functions. Allows for generalization through default parameters.
//	bool restoreListInternal(AMControl * ctrl, QList<QString> *maskList = NULL, bool useList = FALSE);
//};

#endif // AM_CONTROLSTATE_H
