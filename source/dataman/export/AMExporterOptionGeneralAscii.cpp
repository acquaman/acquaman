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


#include "AMExporterOptionGeneralAscii.h"
#include <QPushButton>
#include "ui/dataman/AMExporterOptionGeneralAsciiView.h"

AMExporterOptionGeneralAscii::~AMExporterOptionGeneralAscii(){}

AMExporterOptionGeneralAscii::AMExporterOptionGeneralAscii(QObject *parent) :
	AMExporterOptionGeneral(parent)
{
	columnDelimiter_ = "\t";
	newlineDelimiter_ = "\r\n";
	exportPrecision_ = 0;
	setModified(false);
}

AMExporterOptionGeneralAscii::AMExporterOptionGeneralAscii(const AMExporterOptionGeneralAscii &original)
	: AMExporterOptionGeneral(original)
{
	columnDelimiter_ = original.columnDelimiter();
	newlineDelimiter_ = original.newlineDelimiter();
	sourceExportPrecision_ = original.precisionMap();
	exportPrecision_ = original.exportPrecision();
}

AMExporterOption *AMExporterOptionGeneralAscii::createCopy() const
{
	AMExporterOption *option = new AMExporterOptionGeneralAscii(*this);
	option->dissociateFromDb(true);
	return option;
}

int AMExporterOptionGeneralAscii::exportPrecision() const
{
	return exportPrecision_;
}

int AMExporterOptionGeneralAscii::exportPrecision(const QString &source) const
{
	if(sourceExportPrecision_.contains(source)){
		return sourceExportPrecision_.value(source);
	}
	else
		return exportPrecision_;
}

void AMExporterOptionGeneralAscii::setExportPrecision(const QString &source, int precision)
{
	if(!source.isEmpty() && precision >= 1) {
		sourceExportPrecision_.insert(source, precision);
		setModified(true);
	}
}

void AMExporterOptionGeneralAscii::setExportPrecision(int precision)
{
	if(precision >= 1){
		exportPrecision_ = precision;
		setModified(true);
	}
}

QString AMExporterOptionGeneralAscii::loadPrecisionMap()
{
	QStringList precisionMapConversion;

	QMap<QString, int>::iterator i;

	for (i = sourceExportPrecision_.begin(); i != sourceExportPrecision_.end(); ++i)
		precisionMapConversion << i.key() << QString("%1").arg(i.value());

	return precisionMapConversion.join(":");

}

void AMExporterOptionGeneralAscii::readPrecisionMap(const QString &stringMap)
{
	QStringList precisionMap = stringMap.split(":", QString::SkipEmptyParts);

	if(precisionMap.size() % 2 != 0){
		return;
	}

	sourceExportPrecision_.clear();

	for(int i = 0; i < precisionMap.size(); i = i + 2){
		sourceExportPrecision_.insert(precisionMap.at(i), precisionMap.at(i+1).toInt());
	}
}

const QString AMExporterOptionGeneralAscii::mapToString()
{
	QMap<QString, int>::iterator i;
	QString map = "";

	for(i = sourceExportPrecision_.begin(); i != sourceExportPrecision_.end(); ++i)
		map = i.key() + " " + QString("%1").arg(i.value()) + "\n";

	return map;
}

const QMetaObject* AMExporterOptionGeneralAscii::getMetaObject(){
	return metaObject();
}

QWidget * AMExporterOptionGeneralAscii::createEditorWidget()
{
	return new AMExporterOptionGeneralAsciiView(this, availableDataSourcesModel_);
}
