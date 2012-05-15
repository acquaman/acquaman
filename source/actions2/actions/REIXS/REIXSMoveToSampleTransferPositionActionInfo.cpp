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


#include "REIXSMoveToSampleTransferPositionActionInfo.h"

REIXSMoveToSampleTransferPositionActionInfo::REIXSMoveToSampleTransferPositionActionInfo(QObject *parent) :
	AMActionInfo("Move to Sample Transfer Position", "Move the REIXS sample manipulator to the transfer position", ":/32x32/media-eject.png", parent)
{
	positions_.append(AMControlInfo("sampleX", 0));
	positions_.append(AMControlInfo("sampleY", 0));
	positions_.append(AMControlInfo("sampleZ", 0));
	positions_.append(AMControlInfo("sampleTheta", 0));
}

REIXSMoveToSampleTransferPositionActionInfo::REIXSMoveToSampleTransferPositionActionInfo(const REIXSMoveToSampleTransferPositionActionInfo &other) :
	AMActionInfo(other)
{
	positions_.setValuesFrom(other.positions_);
}
