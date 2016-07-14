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


#include "AMControl.h"

 AMControl::~AMControl(){}
AMControl::AMControl(const QString& name, const QString& units, QObject* parent, const QString description) : QObject(parent), units_(units), description_(description)
{
	setObjectName(name);
	tolerance_ = AMCONTROL_TOLERANCE_DONT_CARE;
	attemptMoveWhenWithinTolerance_ = true;
	allowsMovesWhileMoving_ = false;
	displayPrecision_ = 3;

	moveProgressMinimum_ = 0;
	moveProgressValue_ = 0;
	moveProgressMaximum_ = 1;
	moveProgressPercent_ = 0;
}

QString AMControl::toString() const
{
	// Note this control's name.

	QString controlName = QString("Name: %1").arg(name());

	// Note this control's description.

	QString controlDescription = QString("Description: %1").arg(description());

	// Note this control's value.

	QString controlValue = (canMeasure()) ? QString("Value: %1").arg(value()) : QString("Value: Not measurable");

	// Note this control's minimum.

	QString controlMinimum = QString("Minimum: %1").arg(minimumValue());

	// Note this control's maximum.

	QString controlMaximum = QString("Maximum: %1").arg(maximumValue());

	// Note this control's value options.

	if (isEnum()) {

		QString controlValues = QString("\tEnum options:\n");

		int enumCount = enumNames().count();

		if (enumCount > 0) {

			for (int enumIndex = 0; enumIndex < enumCount; enumIndex++) {
				QString enumName = enumNames().at(enumIndex);

				if (!enumName.isEmpty())
					controlValues.append(QString("\t\t%1 - %2").arg(enumIndex).arg(enumName));
				else
					controlValues.append(QString("\t\t[Empty enum name]"));

				if (enumIndex < enumCount - 1)
					controlValues.append("\n");
			}

		} else {

			controlValues = QString("\t\t[No enum options]");
		}

		controlValue.append(QString("\n%1").arg(controlValues));
	}

	// Note the values of the control's children.

	int childCount = childControlCount();

	if (childCount > 0) {

		QString childValues = QString("\tChild values:\n");

		for (int childIndex = 0; childIndex < childCount; childIndex++) {
			QString childValue;
			AMControl *child = childControlAt(childIndex);

			if (child) {
				if (child->canMeasure())
					childValue = QString("\t\t%1: %2").arg(child->name()).arg(child->value());
				else
					childValue = QString("\t\t%1 cannot be measured.").arg(child->name());
			} else {
				childValue = QString("\t\t[Null child]");
			}

			childValues.append(childValue);

			if (childIndex < childCount - 1)
				childValues.append("\n");
		}

		controlValue.append(QString("\n%1").arg(childValues));
	}

	// Note this control's general connected state.

	QString controlConnected = QString("Connected: %1").arg(isConnected() ? "Yes" : "No");

	// Note the connected state of each of the child controls.

	if (childCount > 0) {

		QString childrenConnected;

		for (int childIndex = 0; childIndex < childCount; childIndex++) {
			QString childConnected;
			AMControl *child = childControlAt(childIndex);

			if (child)
				childConnected = QString("\t%1 connected: %2").arg(child->name()).arg(child->isConnected() ? "Yes" : "No");
			else
				childConnected = QString("\t[Null child]");

			childrenConnected.append(childConnected);

			if (childIndex < childCount - 1)
				childrenConnected.append("\n");
		}

		controlConnected.append(QString("\n%1").arg(childrenConnected));
	}

	// Note whether this control can move.

	QString controlCanMove = QString("Can move: %1").arg(canMove()? "Yes" : "No");

	// Note this control's moving state.

	QString controlMoving = QString("Moving: %1").arg(isMoving() ? "Yes" : "No");

	// Note the moving state of each of the child controls.

	QString childrenMoving;

	if (childCount > 0) {

		for (int childIndex = 0; childIndex < childCount; childIndex++) {
			QString childMoving;
			AMControl *child = childControlAt(childIndex);

			if (child)
				childMoving = QString("\t%1 moving: %2").arg(child->name()).arg(child->isMoving() ? "Yes" : "No");
			else
				childMoving = QString("\t[Null child]");

			childrenMoving.append(childMoving);

			if (childIndex < childCount - 1)
				childrenMoving.append("\n");
		}

	} else {

		childrenMoving = QString("\t[No children]");
	}

	controlMoving.append(QString("\n%1").arg(childrenMoving));

	// Create and return complete info string.

	QString result = QString("%1\n%2\n%3\n%4\n%5\n%6\n%7\n%8").arg(controlName).arg(controlDescription).arg(controlValue).arg(controlMinimum).arg(controlMaximum).arg(controlConnected).arg(controlCanMove).arg(controlMoving);

	return result;
}

bool AMControl::hasChildControl(AMControl *control) const
{
	bool result = false;

	if (control && !children_.isEmpty()) {
		bool controlFound = false;

		for (int i = 0, count = children_.count(); i < count && !controlFound; i++) {
			AMControl *child = children_.at(i);

			if (child) {
				if (child == control)
					controlFound = true;
				else
					controlFound = child->hasChildControl(control);
			}
		}

		result = controlFound;
	}

	return result;
}

bool AMControl::addChildControl(AMControl *control)
{
	if (control && !children_.contains(control)){
		children_ << control;
		return true;
	}
	return false;
}

bool AMControl::removeChildControl(AMControl *control)
{
	if (control && children_.contains(control)) {
		disconnect( control, 0, this, 0 );
		children_.removeOne(control);
		return true;
	}
	return false;
}

bool AMControl::clearChildControls()
{
	bool result = true;
	if(!children_.empty()){
		foreach (AMControl *child, children_){
			if(!removeChildControl(child))
				result = false;
		}
	}
	return result;
}

double AMControl::calculateMoveProgressPercent(double min, double value, double max) const
{
	if (max != min)
		return qAbs(value - min) / qAbs(max - min);
	else
		return 0;
}

void AMControl::updateMoveProgress()
{
	//updateMoveProgressMinimum();
	updateMoveProgressValue();
	updateMoveProgressMaximum();

	updateMoveProgressPercent();
}

void AMControl::setMoveProgressMinimum(double newValue)
{
	if (moveProgressMinimum_ != newValue) {
		moveProgressMinimum_ = newValue;
		updateMoveProgressPercent();
	}
}

void AMControl::updateMoveProgressMinimum()
{
	setMoveProgressMinimum( value() );
}

void AMControl::setMoveProgressValue(double newValue)
{
	if (moveProgressValue_ != newValue) {
		moveProgressValue_ = newValue;
		updateMoveProgressPercent();
	}
}

void AMControl::updateMoveProgressValue()
{
	setMoveProgressValue( value() );
}

void AMControl::setMoveProgressMaximum(double newValue)
{
	if (moveProgressMaximum_ != newValue) {
		moveProgressMaximum_ = newValue;
		updateMoveProgressPercent();
	}
}

void AMControl::updateMoveProgressMaximum()
{
	setMoveProgressMaximum( setpoint() );
}

void AMControl::setMoveProgressPercent(double newValue)
{
	if (moveProgressPercent_ != newValue) {
		moveProgressPercent_ = newValue;
		emit moveProgressChanged(moveProgressPercent_);
	}
}

void AMControl::updateMoveProgressPercent()
{
	setMoveProgressPercent( calculateMoveProgressPercent(moveProgressMinimum_, moveProgressValue_, moveProgressMaximum_) );
}
