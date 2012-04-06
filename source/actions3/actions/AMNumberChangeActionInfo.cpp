#include "AMNumberChangeActionInfo.h"

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
