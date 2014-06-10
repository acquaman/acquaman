#include "AMCustomizablePeriodicTable.h"

#include "util/AMErrorMonitor.h"
#include "util/AMPeriodicTable.h"

 AMCustomizablePeriodicTable::~AMCustomizablePeriodicTable(){}
AMCustomizablePeriodicTable::AMCustomizablePeriodicTable(QObject *parent)
	: QObject(parent)
{
}

void AMCustomizablePeriodicTable::buildPeriodicTable()
{
	periodicTable_ = AMPeriodicTable::table()->elements();
}

AMElement *AMCustomizablePeriodicTable::elementByName(QString name) const
{
	for (int i = 0; i < periodicTable_.size(); i++)
		if (periodicTable_.at(i)->name() == name)
			return periodicTable_.at(i);

	return 0;
}

AMElement *AMCustomizablePeriodicTable::elementBySymbol(QString symbol) const
{
	for (int i = 0; i < periodicTable_.size(); i++)
		if (periodicTable_.at(i)->symbol() == symbol)
			return periodicTable_.at(i);

	return 0;
}
