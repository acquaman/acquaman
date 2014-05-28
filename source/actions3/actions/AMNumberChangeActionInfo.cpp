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


#include "AMNumberChangeActionInfo.h"

 AMNumberChangeActionInfo::~AMNumberChangeActionInfo(){}
AMNumberChangeActionInfo::AMNumberChangeActionInfo(int newNumber, int indexToChange, QObject *parent) :
	AMActionInfo3("Number Change", "Number Change", ":/system-run.png", parent)
{
	newNumber_ = newNumber;
	indexToChange_ = indexToChange;
	canCopy_ = true;
	updateDescriptionText();
}

AMNumberChangeActionInfo::AMNumberChangeActionInfo(const AMNumberChangeActionInfo &other) :
	AMActionInfo3(other)
{
	newNumber_ = other.newNumber();
	indexToChange_ = other.indexToChange();
	canCopy_ = other.canCopy();
	updateDescriptionText();
}

AMActionInfo3* AMNumberChangeActionInfo::createCopy() const{
	return new AMNumberChangeActionInfo(*this);
}

QString AMNumberChangeActionInfo::typeDescription() const{
	return "Number Change";
}

int AMNumberChangeActionInfo::newNumber() const{
	return newNumber_;
}

int AMNumberChangeActionInfo::indexToChange() const{
	return indexToChange_;
}

void AMNumberChangeActionInfo::setNewNumber(int newNumber){
	newNumber_ = newNumber;
	updateDescriptionText();
}

void AMNumberChangeActionInfo::setIndexToChange(int indexToChange){
	indexToChange_ = indexToChange;
	updateDescriptionText();
}

void AMNumberChangeActionInfo::updateDescriptionText(){
	QString description = QString("Changing number at index %1 to %2").arg(indexToChange_).arg(newNumber_);
	setShortDescription(description);
	setLongDescription(description);
}
