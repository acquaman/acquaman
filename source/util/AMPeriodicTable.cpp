#include "AMPeriodicTable.h"

#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QDebug>

AMPeriodicTable::AMPeriodicTable(QObject *parent) :
	QObject(parent)
{
	QFile file(":/ElementalInformation.txt");

	if (!file.open(QFile::ReadOnly | QFile::Text)){
		emit unableToLoad(file.fileName(), file.errorString());
		return;
	}

	QTextStream in(&file);

	AMElement *temp;
	QStringList current;

	while(!in.atEnd()){

		current << in.readLine().split(",");
		switch(current.size()){
		case 3:
			temp = new AMElement(current.first(), current.at(1), current.at(2), QStringList(), QStringList());
			break;
		case 4:
			temp = new AMElement(current.first(), current.at(1), current.at(2), ((QString)(current.at(3))).split(" "), QStringList());
			break;
		case 5:
			temp = new AMElement(current.first(), current.at(1), current.at(2), ((QString)(current.at(3))).split(" "), ((QString)(current.at(4))).split(" "));
			break;
		}
		periodicTable_ << temp;
		current.clear();
	}

	file.close();
}
