#include "CLSControl.h"

CLSControl::CLSControl(const QString &name, const QString &units, QObject *parent, const QString &description) :
	AMControl(name, units, parent, description)
{

}

CLSControl::~CLSControl()
{

}

double CLSControl::enumIndexNamed(const QString &name) const
{
	double result = -1;

	if (enumIndexStringMap_.values().contains(name))
		result = enumIndexStringMap_.key(name);

	return result;
}

QList<double> CLSControl::moveEnumIndices() const
{
	QList<double> result;

	foreach (double index, enumIndexStringMap_.keys()) {
		if (!readOnlyEnumIndices_.contains(index))
			result << index;
	}

	return result;
}

QStringList CLSControl::readOnlyEnumNames() const
{
	QStringList result;

	foreach (double readOnlyIndex, readOnlyEnumIndices_)
		result << enumIndexStringMap_.value(readOnlyIndex);

	return result;
}

QStringList CLSControl::moveEnumNames() const
{
	QStringList result;

	foreach (double index, moveEnumIndices())
		result << enumIndexStringMap_.value(index);

	return result;
}

void CLSControl::setConnected(bool isConnected)
{
	if (connected_ != isConnected) {
		connected_ = isConnected;
		emit connected(connected_);
	}
}

void CLSControl::updateConnected()
{
	setConnected( currentConnected() );
}

void CLSControl::setEnumerated(bool isEnumerated)
{
	if (enumerated_ != isEnumerated) {
		enumerated_ = isEnumerated;
		emit enumChanged();
	}
}

void CLSControl::updateEnumerated()
{
	setEnumerated( currentEnumerated() );
}

bool CLSControl::addReadOnlyEnumOption(int index, const QString &optionString)
{
	bool result = false;

	if (!readOnlyEnumIndices_.contains(index) && !optionString.isEmpty() && !enumIndexStringMap_.values().contains(optionString)) {
		addEnumOption(index, optionString);
		readOnlyEnumIndices_.append(index);
		result = true;
	}

	return result;
}

bool CLSControl::addEnumOption(int index, const QString &optionString)
{
	enumIndexStringMap_.insert(index, optionString);
	return true;
}

bool CLSControl::removeEnumOption(int index)
{
	bool result = false;

	if (hasEnumIndex(index)) {

		// Check to see if the index is a read-only enum.
		// If so, remove it.

		if (readOnlyEnumIndices_.contains(index))
			readOnlyEnumIndices_.removeOne(index);

		// Remove the index from the enum/string map.

		enumIndexStringMap_.remove(index);

		result = true;
	}

	return result;
}

bool CLSControl::clearEnumOptions()
{
	readOnlyEnumIndices_.clear();
	enumIndexStringMap_.clear();

	return true;
}
