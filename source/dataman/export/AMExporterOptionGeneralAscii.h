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


#ifndef AMEXPORTEROPTIONGENERALASCII_H
#define AMEXPORTEROPTIONGENERALASCII_H

#include "dataman/export/AMExporterOptionGeneral.h"

class AMExporterOptionGeneralAscii : public AMExporterOptionGeneral
{
	Q_OBJECT

	Q_PROPERTY(QString columnDelimiter READ columnDelimiter WRITE setColumnDelimiter)
	Q_PROPERTY(QString newlineDelimiter READ newlineDelimiter WRITE setNewlineDelimiter)
	Q_PROPERTY(int exportPrecision READ exportPrecision WRITE setExportPrecision)

	Q_CLASSINFO("exportPrecision", "upgradeDefault=8")

public:
	/// Constructor.
	Q_INVOKABLE explicit AMExporterOptionGeneralAscii(QObject *parent = 0);
	/// Copy Constructor.
	AMExporterOptionGeneralAscii(const AMExporterOptionGeneralAscii &original);
	/// Destructor.
	virtual ~AMExporterOptionGeneralAscii();

	const QMetaObject* getMetaObject();

	virtual AMExporterOption* createCopy() const;

	/// The delimiter to use between columns
	QString columnDelimiter() const { return columnDelimiter_; }
	/// The delimiter to use between lines (newline character)
	QString newlineDelimiter() const { return newlineDelimiter_; }

	QMap<QString, int> precisionMap() const { return sourceExportPrecision_; }

	/// Returns the default precision if no source is specified.
	int exportPrecision() const;
	/// Returns the associated precision with the source name.
	int exportPrecision(const QString& source) const;
	/// Sets a precision for a source name. Duplicate naming is not allowed.
	bool setExportPrecision(const QString& source, const int precision);
	/// Sets the default precision with no AMDataSource name specified.
	bool setExportPrecision(const int precision);

	virtual QWidget* createEditorWidget();


signals:

public slots:
	/// Set delimiter to use between columns
	void setColumnDelimiter(const QString& t) { columnDelimiter_ = t; setModified(true); }
	/// Set the delimiter to use between lines (newline character)
	void setNewlineDelimiter(const QString& t) { newlineDelimiter_ = t; setModified(true); }

protected:
	/// The delimiter to use between columns
	QString columnDelimiter_;
	/// The delimiter to use between lines (newline character)
	QString newlineDelimiter_;
	/// A mapping of datasource names to the precision their data should be exported at.
	QMap<QString, int> sourceExportPrecision_;
	/// A default precision if a source is not given one specifically.
	int exportPrecision_ ;

};

#endif // AMEXPORTEROPTIONGENERALASCII_H
