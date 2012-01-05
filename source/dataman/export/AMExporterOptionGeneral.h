/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef AMEXPORTEROPTIONGENERAL_H
#define AMEXPORTEROPTIONGENERAL_H


#include "dataman/export/AMExporterOption.h"

/// An exporter object (AMExporter) writes a scan to file according to a set of "export options".  This class specifies the export options for the "general" class of file exporters, which are designed to be as flexible as possible in the data that they can write.  See AMExporterOptionGeneralAscii for a complete example.
class AMExporterOptionGeneral : public AMExporterOption
{
	Q_OBJECT

	Q_PROPERTY(QString headerText READ headerText WRITE setHeaderText)
	Q_PROPERTY(bool headerIncluded READ headerIncluded WRITE setHeaderIncluded)

	Q_PROPERTY(QString columnHeader READ columnHeader WRITE setColumnHeader)
	Q_PROPERTY(bool columnHeaderIncluded READ columnHeaderIncluded WRITE setColumnHeaderIncluded)
	Q_PROPERTY(QString columnHeaderDelimiter READ columnHeaderDelimiter WRITE setColumnHeaderDelimiter)

	Q_PROPERTY(QString sectionHeader READ sectionHeader WRITE setSectionHeader)
	Q_PROPERTY(bool sectionHeaderIncluded READ sectionHeaderIncluded WRITE setSectionHeaderIncluded)

	Q_PROPERTY(bool includeAllDataSources READ includeAllDataSources WRITE setIncludeAllDataSources)
	Q_PROPERTY(bool firstColumnOnly READ firstColumnOnly WRITE setFirstColumnOnly)
	Q_PROPERTY(bool separateHigherDimensionalSources READ separateHigherDimensionalSources WRITE setSeparateHigherDimensionalSources)
	Q_PROPERTY(QStringList dataSources READ dataSources WRITE dbLoadDataSources)
	Q_PROPERTY(AMIntList dataSourceOrganizeModes READ dataSourceOrganizeModes WRITE dbLoadDataSourceOrganizeModes)
	Q_PROPERTY(AMIntList dataSourceIsRequired READ dataSourceIsRequired WRITE dbLoadDataSourceIsRequired)
	Q_PROPERTY(AMIntList dataSourceOmitAxisValueColumns READ dataSourceOmitAxisValueColumns WRITE dbLoadDataSourceOmitAxisValueColumns)

	Q_PROPERTY(QString separateSectionFileName READ separateSectionFileName WRITE setSeparateSectionFileName)

public:

	/// Describes where to place 2D or higher data source in the export output.
	enum DataSourceOrganizeMode {
		CombineInColumnsMode,	///< Create extra columns alongside the 1d data sources in their table.  This only works for 2D datasources.  Columns will be labelled with _________ ? (index? axisValue?)
		SeparateSectionsMode, ///< Make a table (or hyper-table) for each Nd data source, and put them sequentially into the file (with sectionDelimiter() in between them)
		SeparateFilesMode ///< Make a table for each 2d data source, and create a separate file for each. (Files named using separateFilesExtension())
	};

	explicit AMExporterOptionGeneral(QObject *parent = 0);

	const QMetaObject* getMetaObject();


	QString headerText() const { return headerText_; }
	bool headerIncluded() const { return headerIncluded_; }
	QString columnHeader() const { return columnHeader_; }
	bool columnHeaderIncluded() const { return columnHeaderIncluded_; }
	QString columnHeaderDelimiter() const { return columnHeaderDelimiter_; }
	QString sectionHeader() const { return sectionHeader_; }
	bool sectionHeaderIncluded() const { return sectionHeaderIncluded_; }

	QStringList dataSources() const { return dataSources_; }
	bool includeAllDataSources() const { return includeAllDataSources_; }
	bool firstColumnOnly() const { return firstColumnOnly_; }
	bool separateHigherDimensionalSources() const { return separateHigherDimensionalSources_; }
	AMIntList dataSourceOrganizeModes() const { return dataSourceOrganizeMode_; }
	AMIntList dataSourceIsRequired() const{ return dataSourceIsRequired_; }
	AMIntList dataSourceOmitAxisValueColumns() const { return dataSourceOmitAxisValueColumn_; }

	bool isDataSourceRequired(int index) const { return dataSourceIsRequired_.at(index); }
	bool isDataSourceAxisValueColumnOmitted(int index) const { return dataSourceOmitAxisValueColumn_.at(index); }
	int dataSourceOrganizeMode(int index) const { return dataSourceOrganizeMode_.at(index); }

	QString separateSectionFileName() const { return separateSectionFileName_; }


public slots:
	void setHeaderText(const QString& t) { headerText_ = t; setModified(true); }
	void setHeaderIncluded(bool isIncluded) { headerIncluded_ = isIncluded; setModified(true); }
	void setColumnHeader(const QString& t) { columnHeader_ = t; setModified(true); }
	void setColumnHeaderIncluded(bool isIncluded) { columnHeaderIncluded_ = isIncluded;  setModified(true); }
	void setColumnHeaderDelimiter(const QString& t) { columnHeaderDelimiter_ = t; setModified(true); }
	void setSectionHeader(const QString& t) { sectionHeader_ = t; setModified(true); }
	void setSectionHeaderIncluded(bool isIncluded) { sectionHeaderIncluded_ = isIncluded; setModified(true); }

	void setIncludeAllDataSources(bool includeAll) { includeAllDataSources_ = includeAll; setModified(true); }
	void setFirstColumnOnly(bool firstOnly) { firstColumnOnly_ = firstOnly; setModified(true); }
	void setSeparateHigherDimensionalSources(bool separateHigherDimensions) { separateHigherDimensionalSources_ = separateHigherDimensions; setModified(true); }

	void addDataSource(const QString& name, bool omitAxisValueColumn, int organizeMode = CombineInColumnsMode, bool isRequired = true) {
		dataSources_ << name;
		dataSourceOmitAxisValueColumn_ << omitAxisValueColumn;
		dataSourceOrganizeMode_ << organizeMode;
		dataSourceIsRequired_ << isRequired;
		setModified(true);
	}

	void removeDataSourceAt(int index) {
		dataSources_.removeAt(index);
		dataSourceOmitAxisValueColumn_.removeAt(index);
		dataSourceOrganizeMode_.removeAt(index);
		dataSourceIsRequired_.removeAt(index);
		setModified(true);
	}

	void setDataSourceRequired(int index, bool isRequired) { dataSourceIsRequired_[index] = isRequired; setModified(true); }
	void setDataSourceAxisValueColumnOmitted(int index, bool isOmitted) { dataSourceOmitAxisValueColumn_[index] = isOmitted; setModified(true); }
	void setDataSourceOrganizeMode(int index, int organizeMode) { dataSourceOrganizeMode_[index] = organizeMode; setModified(true); }


	void setSeparateSectionFileName(const QString& fileName) { separateSectionFileName_ = fileName; setModified(true); }

public:
	void dbLoadDataSources(const QStringList& sources) { dataSources_ = sources; setModified(true); }
	void dbLoadDataSourceOrganizeModes(const AMIntList& om) { dataSourceOrganizeMode_ = om; setModified(true);}
	void dbLoadDataSourceIsRequired(const AMIntList& reqd) { dataSourceIsRequired_ = reqd; setModified(true); }
	void dbLoadDataSourceOmitAxisValueColumns(const AMIntList& omit) { dataSourceOmitAxisValueColumn_ = omit; setModified(true); }

signals:

public slots:


protected:
	/// user-configurable header text. Can include "$keyword" replacement tags.  "\n" will be replaced with the newline delimiter, and "\t" will be replaced by the column delimiter.
	QString headerText_;

	/// True if the header text should be written
	bool headerIncluded_;

	/// Column header for 1D data sources.  "$d" will be replaced with the data source number.  "$keyword" replacement tags are allowed.  (Note: this string is repeated for each column)
	QString columnHeader_;
	/// True if the column header should be written
	bool columnHeaderIncluded_;

	/// Delimiter to insert between the column header and the actual columns
	QString columnHeaderDelimiter_;

	/// Delimiter to insert between sections (ie: separate tables of different data sources).  \todo Special tags okay?
	QString sectionHeader_;
	bool sectionHeaderIncluded_;


	/// list of the data sources to include, in order.  Data sources are specified by their name().
	QStringList dataSources_;
	/// If instead you want to include all data sources in the scan (and ignore dataSources_), set this:
	bool includeAllDataSources_;
	/// Additional flag for when including all data sources.  If you wish to only have the X-axis printed out for the first data source.
	bool firstColumnOnly_;
	/// Additional flag for separating higher dimensional data sources (2D and up) into their own files.
	bool separateHigherDimensionalSources_;

	/// parallel list to dataSources_: specifies where to place each data source (Interpret as AMExporterOptionGeneral::DataSourceLocation)
	AMIntList dataSourceOrganizeMode_;
	/// Parallel list to dataSources_: specifiy whether to fail or silently skip if a data source of this name isn't found in the scan.  ('1' if should fail when not found, '0' if okay to skip.)
	AMIntList dataSourceIsRequired_;
	/// Parallel list to dataSources_: specify whether to include the x-axis (axisValue()) of the data source in a column to the left of the actual column.
	AMIntList dataSourceOmitAxisValueColumn_;


	/// If data sources are to be saved to separate files, this gives the file name for the extra file.  ($keyword tags are allowed here)
	QString separateSectionFileName_;


};

#endif // AMEXPORTEROPTIONGENERAL_H
