#include "AMCustomizablePeriodicTable.h"

#include <QFile>
#include <QTextStream>

#include "util/AMErrorMonitor.h"

AMCustomizablePeriodicTable::AMCustomizablePeriodicTable(QObject *parent)
	: QObject(parent)
{
	QFile file(":/ElementalInformation.txt");

	if (!file.open(QFile::ReadOnly | QFile::Text)){
		AMErrorMon::error(this, AMCUSTOMIZABLEPERIODICTABLE_COULD_NOT_LOAD_ELEMENTAL_INFORMATION, "There was an error loading the elemental information for the periodic table.  Elemental information throughout the program may no longer be defined.");
		return;
	}

	QTextStream in(&file);
	QStringList elementalInformation;

	while(!in.atEnd())
		elementalInformation << in.readLine();

	file.close();

	buildPeriodicTable(elementalInformation);
}

void AMCustomizablePeriodicTable::buildPeriodicTable(const QStringList &list)
{
	foreach (QString element, list){

		QStringList current = element.split(",");

		switch(current.size()){

		case 3:
			periodicTable_ << new AMElement(current.first(), current.at(1), current.at(2), QStringList(), QStringList());
			break;

		case 4:
			periodicTable_ << new AMElement(current.first(), current.at(1), current.at(2), ((QString)(current.at(3))).split(" "), QStringList());
			break;

		case 5:
			periodicTable_ << new AMElement(current.first(), current.at(1), current.at(2), ((QString)(current.at(3))).split(" "), ((QString)(current.at(4))).split(" "));
			break;
		}
	}
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
