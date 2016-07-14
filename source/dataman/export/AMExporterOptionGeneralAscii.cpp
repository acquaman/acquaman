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
#include <QDebug>
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
		qWarning("!!!Precision REQUESTED AND IS :: " + QString("%1").arg(exportPrecision_).toLocal8Bit());
		return exportPrecision_;
}

bool AMExporterOptionGeneralAscii::setExportPrecision(const QString &source, int precision)
{
	if(!source.isEmpty() && precision >= 1) {
		sourceExportPrecision_.insert(source, precision);
		return true;
	}
	else
		return false;
}

bool AMExporterOptionGeneralAscii::setExportPrecision(int precision)
{
	if(precision >= 1){
		qWarning("!!!Precision IS :: " + QString("%1").arg(exportPrecision_).toLocal8Bit());
		exportPrecision_ = precision;
		qWarning("!!!Precision has been set to :: " + QString("%1").arg(exportPrecision_).toLocal8Bit());
		setModified(true);
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
