#include "AMNameAndRangeValidator.h"

 AMNameAndRangeValidator::~AMNameAndRangeValidator(){}
AMNameAndRangeValidator::AMNameAndRangeValidator(QObject *parent)
	: QObject(parent)
{
	range_ = AMRange();
}

AMNameAndRangeValidator::AMNameAndRangeValidator(const AMRange &range, QObject *parent)
	: QObject(parent)
{
	range_ = range;
}

AMNameAndRangeValidator::AMNameAndRangeValidator(const QStringList &filters, QObject *parent)
	: QObject(parent)
{
	range_ = AMRange();

	foreach (QString filter, filters)
		nameFilters_ << QRegExp(filter);
}

AMNameAndRangeValidator::AMNameAndRangeValidator(const QList<QRegExp> &filters, QObject *parent)
	: QObject(parent)
{
	nameFilters_ = filters;
	range_ = AMRange();
}

AMNameAndRangeValidator::AMNameAndRangeValidator(const QStringList &filters, const AMRange &range, QObject *parent)
	: QObject(parent)
{
	range_ = range;

	foreach (QString filter, filters)
		nameFilters_ << QRegExp(filter);
}

AMNameAndRangeValidator::AMNameAndRangeValidator(const QList<QRegExp> &filters, const AMRange &range, QObject *parent)
	: QObject(parent)
{
	range_ = range;
	nameFilters_ = filters;
}

bool AMNameAndRangeValidator::isValid(const QString &name) const
{
	bool isValid = true;

	foreach (QRegExp nameFilter, nameFilters_)
		isValid = isValid && name.contains(nameFilter);

	return isValid;
}

bool AMNameAndRangeValidator::isValid(double value) const
{
	return (!range_.isValid()
			|| (range_.isValid() && range_.withinRange(value)));
}

bool AMNameAndRangeValidator::isValid(const QString &name, double value) const
{
	return isValid(name) && isValid(value);
}

void AMNameAndRangeValidator::setRange(const AMRange &newRange)
{
	if (range_ != newRange){

		range_ = newRange;
		emit validatorChanged();
	}
}

void AMNameAndRangeValidator::setRange(double minimum, double maximum)
{
	setRange(AMRange(minimum, maximum));
}

void AMNameAndRangeValidator::setMinimum(double newMinimum)
{
	if (range_.minimum() != newMinimum){

		range_.setMinimum(newMinimum);
		emit validatorChanged();
	}
}

void AMNameAndRangeValidator::setMaximum(double newMaximum)
{
	if (range_.maximum() != newMaximum){

		range_.setMaximum(newMaximum);
		emit validatorChanged();
	}
}

void AMNameAndRangeValidator::addNameFilter(const QString &newNameFilter)
{
	addNameFilter(QRegExp(newNameFilter));
}

void AMNameAndRangeValidator::addNameFilter(const QRegExp &newNameFilter)
{
	if (!nameFilters_.contains(newNameFilter)){

		nameFilters_ << newNameFilter;
		emit validatorChanged();
	}
}

bool AMNameAndRangeValidator::removeNameFilter(int index)
{
	bool successfullyRemoved = (index >= 0) && (index < nameFilters_.size());

	if (successfullyRemoved){

		nameFilters_.removeAt(index);
		emit validatorChanged();
	}

	return successfullyRemoved;
}

bool AMNameAndRangeValidator::removeNameFilter(const QString &filter)
{
	return removeNameFilter(QRegExp(filter));
}

bool AMNameAndRangeValidator::removeNameFilter(const QRegExp &filter)
{
	bool successfullyRemoved = nameFilters_.removeOne(filter);

	if (successfullyRemoved)
		emit validatorChanged();

	return successfullyRemoved;
}
