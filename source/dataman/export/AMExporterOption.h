/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef AMEXPORTEROPTION_H
#define AMEXPORTEROPTION_H

#include "dataman/database/AMDbObject.h"

class QWidget;

/// An exporter object (AMExporter) writes a scan to file according to a set of "export options".  This class specifies the export options for the "general" class of file exporters, which are designed to be as flexible as possible in the data that they can write.  See AMExporterOptionGeneralAscii for a complete example.
class AMExporterOption : public AMDbObject
{
	Q_OBJECT
	Q_PROPERTY(QString fileName READ fileName WRITE setFileName)

public:
 	virtual ~AMExporterOption();
	explicit AMExporterOption(QObject *parent = 0);

	virtual const QMetaObject* getMetaObject();

	virtual AMExporterOption* createCopy() const = 0;

	/// The file name to save at (not including the destination folder prefix).  Can include "$keyword" replacement tags.  If it includes "/" separators, subfolders will be generated as required within the destination folder.
	QString fileName() const { return fileName_; }

	/// Create an editor widget to modify this option's settings.  If you can't make an editor, return 0 here.
	virtual QWidget* createEditorWidget() { return 0; }


	/// Notify the option that a different model has been provided to tell it what data sources are available in the set of scans to be exported
	void setAvailableDataSourcesModel(QStandardItemModel* availableDataSources) {
		availableDataSourcesModel_ = availableDataSources;
	}

signals:

public slots:
	/// Set the file name to save at (not including the destination folder prefix).  Can include "$keyword" replacement tags.  If it includes "/" separators, subfolders will be generated as required within the destination folder.
	void setFileName(const QString& fileName) { fileName_ = fileName; setModified(true); }

protected:
	/// The file name to save at (not including the destination folder prefix).  Can include "$keyword" replacement tags.  If it includes "/" separators, subfolders will be generated as required within the destination folder.
	QString fileName_;


	/// Sometimes options need to know what data sources are available in the set of scans to be exported. This model will either be 0, or be populated with those available data sources.
	/*! It contains:

- Qt::DisplayRole (QString): "[nD] name: description"

- AM::DescriptionRole (QString): description

- AM::NameRole (QString): name

- AM::UserRole (int): rank
*/
	QStandardItemModel* availableDataSourcesModel_;
};

#endif // AMEXPORTEROPTION_H
