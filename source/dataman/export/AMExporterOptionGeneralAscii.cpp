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
	defaultExportPrecision_ = 19;
	setModified(false);
}

AMExporterOptionGeneralAscii::AMExporterOptionGeneralAscii(const AMExporterOptionGeneralAscii &original)
	: AMExporterOptionGeneral(original)
{
	columnDelimiter_ = original.columnDelimiter();
	newlineDelimiter_ = original.newlineDelimiter();
}

AMExporterOption *AMExporterOptionGeneralAscii::createCopy() const
{
	AMExporterOption *option = new AMExporterOptionGeneralAscii(*this);
	option->dissociateFromDb(true);
	return option;
}

int AMExporterOptionGeneralAscii::exportPrecision() const
{
	return defaultExportPrecision_;
}

int AMExporterOptionGeneralAscii::exportPrecision(const QString &source) const
{
	if(sourceExportPrecision_.contains(source))
		return sourceExportPrecision_.value(source);
	else
		return defaultExportPrecision_;
}

bool AMExporterOptionGeneralAscii::setExportPrecision(const QString &source, const int &precision)
{
	if(!source.isEmpty() && precision >= 1) {
		sourceExportPrecision_.insert(source, precision);
		return true;
	}
	else
		return false;
}

bool AMExporterOptionGeneralAscii::setExportPrecision(const int &precision)
{
	if(precision >= 1){
		defaultExportPrecision_ = precision;
		return true;
	}
	return false;
}

const QMetaObject* AMExporterOptionGeneralAscii::getMetaObject(){
	return metaObject();
}

QWidget * AMExporterOptionGeneralAscii::createEditorWidget()
{
	return new AMExporterOptionGeneralAsciiView(this, availableDataSourcesModel_);
}
