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


#include "REIXSSampleMoveActionInfo.h"

REIXSSampleMoveActionInfo::REIXSSampleMoveActionInfo(QObject *parent) :
	AMActionInfo("REIXS Sample Move", "REIXS Sample Move", ":/32x32/gnome-display-properties.png", parent)
{
	// safe nominal measurement positions
	positions_.append(AMControlInfo("sampleX", 3, 0, 0, "mm"));
	positions_.append(AMControlInfo("sampleY", 4, 0, 0, "mm"));
	positions_.append(AMControlInfo("sampleZ", 2, 0, 0, "mm"));
	positions_.append(AMControlInfo("sampleTheta", 45, 0, 0, "deg"));

	updateDescriptions();
}


REIXSSampleMoveActionInfo::REIXSSampleMoveActionInfo(const AMControlInfoList &targetPosition, QObject *parent) :
	AMActionInfo("REIXS Sample Move", "REIXS Sample Move", ":/32x32/gnome-display-properties.png", parent)
{
	positions_.setValuesFrom(targetPosition);

	updateDescriptions();
}

REIXSSampleMoveActionInfo::REIXSSampleMoveActionInfo(const REIXSSampleMoveActionInfo &other) :
	AMActionInfo(other)
{
	positions_.setValuesFrom(other.positions_);	// remember: setValuesFrom() maintains unique database identifies and just copies the values.  If we used positions_ = other.positions_, we would end up sharing a database identity with other's AMControlInfoList, which is not what we want.
}

void REIXSSampleMoveActionInfo::updateDescriptions()
{
	if(!isValid()) {
		setShortDescription("REIXS Sample Move to [Invalid Position]");
		setLongDescription("REIXS Sample Move to [Invalid Position]");
		return;
	}

	const AMControlInfo& x = positions_.controlNamed("sampleX");
	const AMControlInfo& y = positions_.controlNamed("sampleY");
	const AMControlInfo& z= positions_.controlNamed("sampleZ");
	const AMControlInfo& theta = positions_.controlNamed("sampleTheta");

	QString shortDesc = QString("REIXS Sample Move: X: %1 %2  Y: %3 %4  Z: %5 %6  Theta: %7 %8")
			.arg(x.value()).arg(x.units())
			.arg(y.value()).arg(y.units())
			.arg(z.value()).arg(z.units())
			.arg(theta.value()).arg(theta.units());
	QString longDesc = QString("This action moves the REIXS sample manipulator to a predefined position:\n   X: %1 %2\n   Y: %3 %4\n   Z: %5 %6\n   Theta: %7 %8")
			.arg(x.value()).arg(x.units())
			.arg(y.value()).arg(y.units())
			.arg(z.value()).arg(z.units())
			.arg(theta.value()).arg(theta.units());

	setShortDescription(shortDesc);
	setLongDescription(longDesc);
}

bool REIXSSampleMoveActionInfo::isValid() const
{
	if(positions_.indexOf("sampleX") == -1
			|| positions_.indexOf("sampleY") == -1
			|| positions_.indexOf("sampleZ") == -1
			|| positions_.indexOf("sampleTheta") == -1)
		return false;

	return true;
}

void REIXSSampleMoveActionInfo::setTargetPositions(const AMControlInfoList &targetPositions)
{
	positions_.setValuesFrom(targetPositions);
	setModified(true);

	updateDescriptions();
}
