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


#ifndef AMEXPORTERGENERALASCII_H
#define AMEXPORTERGENERALASCII_H

#include "dataman/export/AMExporter.h"

class AMExporterOptionGeneralAscii;

class AMExporterGeneralAscii : public AMExporter
{
	Q_OBJECT
public:
 	virtual ~AMExporterGeneralAscii();
	Q_INVOKABLE explicit AMExporterGeneralAscii(QObject *parent = 0);

	const QMetaObject* getMetaObject();

	virtual QString description() const { return "Plain Text (Adjustable)"; }
	virtual QString longDescription() const {
		return "The Plain Text file format creates one or more data tables in a plain text file.  You can choose which data sources to include, and which tables to put them in.  You can also specify what to include in an optional header, and where to put higher-dimensional (2D, 3D, etc.) data.";
	}
	virtual QString exporterOptionClassName() const { return "AMExporterOptionGeneralAscii"; }


	// This version checks that all of the data sources in \c option marked as "required" are actually present in \c scan, and that \c option is an AMExporterOptionGeneralAscii
	virtual bool isValidFor(const AMScan *scan, const AMExporterOption *option) const;

	virtual QString exportScan(const AMScan *scan, const QString &destinationFolderPath, const AMExporterOption *option, int autoIndex = 0);

	virtual AMExporterOption* createDefaultOption() const;

signals:

public slots:


protected:
	QString mainFileName_;
	QList<int> mainTableDataSources_;
	QList<bool> mainTableIncludeX_;
	QList<int> separateSectionDataSources_;
	QList<bool> separateSectionIncludeX_;
	QList<int> separateFileDataSources_;
	QList<int> separateFileIncludeX_;
	const AMExporterOptionGeneralAscii* option_;

	/// Parse the data sources in option_, find their index within the scan, and fill mainTableDataSources_, separateSectionDataSources_, and separateFileDataSources_.  Returns true if all required data sources are found.
	virtual bool prepareDataSources();

	// These functions are virtual because you may want most of the options already existing in AMExporterGeneralAscii, but redefine certain behaviours.
	/// Method that writes the header piece of the exported file.
	virtual void writeHeader();
	/// Method that writes the data in the main table, including the function names.
	virtual void writeMainTable();
	/// Method that writes the separate sections of the data file.
	virtual void writeSeparateSections();
	/// Method that writes the separate files for other data sources.
	virtual bool writeSeparateFiles(const QString& destinationFolderPath);

	/// converts all "\r\n" windows style line endings in \c inputString to "\n"
	void normalizeLineEndings(QString& inputString);
	/// Converts all normalized line endings ("\n") to whatever \c eol end-of-line symbol you want
	void convertNormalizedLineEndingsTo(const QString& eol, QString& inputString);

};

#endif // AMEXPORTERGENERALASCII_H
