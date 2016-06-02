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


#ifndef AMEXPORTER_H
#define AMEXPORTER_H

#include <QObject>
#include <QHash>


class AMExporterOption;
class AMScan;
class QFile;

class AMTagReplacementParser;
class AMAbstractTagReplacementFunctor;

/// This class defines the interface for objects which can be used export scans in different formats.  For example, AMExporterGeneralAscii, AMExporterExcel, AMExporterOrigin, and AMExporterNexus all inherit this interface.
/*! To implement a new exporter, you must subclass AMExporter.  Additionally, you must declare a default constructor and tag it with the Q_INVOKABLE macro, and call AMExportController::registerExporter<MyExporterSubclass>() to make it known to the export system. */
class AMExporter : public QObject
{
	Q_OBJECT
public:
	/// Enum flag for determining the overwrite options for the exporter.
	/*!
		All will overwrite all files that have matching file names.
		None will overwrite none of the files that have matching file names.
		Default will ask whether the file should be overwritten for every file that matches.
	  */
	enum OverwriteOption { All, None, Default };

	/// Constructor.
	explicit AMExporter(QObject *parent = 0);
	virtual ~AMExporter();

	virtual const QMetaObject* getMetaObject();

	/// A human-readable description of this file format (Will be used to let the user choose an exporter from the set of available ones.)
	virtual QString description() const = 0;

	/// A long-form description of this file format and its capabilities (Will be provided as additional information. This can be as long as a paragraph if you want... Or it can be empty, like the base class implementation)
	virtual QString longDescription() const { return QString(); }

	/// Return the class name of the preferred AMExporterOption subclass that is compatible with this exporter.
	/*! AMExporterOption subclasses are used to tell an AMExporter how to save a file.  (In some cases they're like templates; in other cases they contain things like offsite database accounts or server login information.)

The user can save their previous "export options" for quick access, in their user database.  While the isValidFor() function can check whether an actual AMExporterOption instance is OK to use with this exporter...
This function returns the class name of the <i>preferred</i> AMExporterOption subclass to use with this exporter.  We use it to search the user database for all saved "export options" that are compatible with this exporter.

For example: AMExporterGeneralAscii::exporterOptionClassName() would probably return "AMExporterOptionGeneralAscii" here.*/
	virtual QString exporterOptionClassName() const = 0;

	/// Can be used to test whether this exporter is valid for a given scan and option set, without actually exporting it.  Returns true if possible.
	virtual bool isValidFor(const AMScan* scan, const AMExporterOption* option) const = 0;

	/// Exports the given \c scan object, using the option set \c option.  The file name is given inside \c option, but should be placed within the folder \c destinationFolderPath.  Returns the name of the file that was written, or a null QString on error.
	virtual QString exportScan(const AMScan* scan, const QString& destinationFolderPath, const AMExporterOption* option, int autoIndex = 0) = 0;

	/// create an "exporter option" (an instance of an AMExporterOption subclass) that is a valid default for this type of exporter
	virtual AMExporterOption* createDefaultOption() const = 0;

	/// Returns whether the exporter will overwrite all files with matching filenames.
	OverwriteOption overwriteOption() const { return overwriteAll_; }
	/// Sets whether the exporter will overwrite all files with matching filenames.
	void setOverwriteOption(OverwriteOption overwrite) { overwriteAll_ = overwrite; }

	/// Helper function: set the current scan, which is used for keyword replacement parsing.  Does not take ownership of the scan
	void setCurrentScan(const AMScan* scan) { currentScan_ = scan; }
	/// returns the file path for the current run
	QString currentRunExportFilePath();

signals:

public slots:

protected:
	/// Helper function: takes a complete directory path + file name, and attempts to open it for writing using the protected file_ object. Will create all subfolders within the path if required.  Returns 0 if there's a permissions error creating the folders, or if a file already exists at that location.
	/*! If file_ is already open, this will QFile::close() it first. */
	bool openFile(const QString& filePath);

	/// Helper function: behaves just like openFile() but operates on a programmer-specified file object.
	bool openFile(QFile* file, const QString& filePath);

	/// File access object.  Open with openFile() and close with closeFile()
	QFile* file_;


	///////////////////////////////////
	// Functions to support the keyword replacement system
	//////////////////////////////////////

	const AMScan* currentScan_;

	/// Helper function: set the current datasource index, which is used for keyword replacement parsing. (ex: "$dataSet" will return the information for the current data source)
	void setCurrentDataSource(int dataSourceIndex) { currentDataSourceIndex_ = dataSourceIndex; }
	int currentDataSourceIndex_;

	/// Helper function: set the current row in the main data table. This is only used for keyword replacement parsing.
	void setCurrentTableRow(int rowIndex) { currentRowIndex_ = rowIndex; }
	int currentRowIndex_;

	/// Helper function: set the current column in the main data table. This is only used for keyword replacement parsing. For 2D data sources, this will be the column index within the currentDataSource
	void setCurrentColumnIndex(int colIndex) { currentColumnIndex_ = colIndex; }
	int currentColumnIndex_;

	/// Parse an input string for recognizable "$keyword" tags, and return a converted string. The results depend on the currentScan_ and currentDataSourceIndex_
	QString parseKeywordString(const QString& inputString);

	/// A dictionary of function pointers we've built to support the "$keyword" replacement system. The functors return the replacement text for a given keyword tag, possibly depending on the argument.  If you want to add more keywords to the parsing system, add them to this.
	QHash<QString, AMAbstractTagReplacementFunctor*> keywordDictionary_;

	/// Initializes the keywordDictionary_ with the functions you see below. You can always add more directly to keywordDictionary_.
	void loadKeywordReplacementDictionary();

	AMTagReplacementParser* keywordParser_;

	/// Helper function: set the current export controller index (if the user wishes to auto-increment the files just by the order the export writes them)
	void setCurrentAutoIndex(int autoIndex) { autoIndex_ = autoIndex; }
	int autoIndex_;

	/// Helper function: returns the filename.
	void setCurrentFilename(const QString &name) { filename_ = name; }
	QString filename_;

	/// Helper function: returns the destination folder path.
	void setDestinationFolderPath(const QString &path) { destinationFolderPath_ = path; }
	QString destinationFolderPath_;

	/// Helper member for Darren... bit hackish.
	QString currentlyParsing_;

	/// Flag used if a scan should overwrite all pre-existing files with the same name.
	OverwriteOption overwriteAll_;

	///////////////////////////////
	// functions to implement the keyword replacement system
	///////////////////////////////

	QString krName(const QString& arg = QString());
	QString krNumber(const QString& arg = QString());
	QString krNotes(const QString& arg = QString());
	QString krDelimitedNotes(const QString& arg = QString());
	QString krSimplifiedNotes(const QString& arg = QString());
	QString krDate(const QString& arg = QString());
	QString krTime(const QString& arg = QString());
	QString krDateTime(const QString& arg = QString());
	QString krSerial(const QString& arg = QString());


	QString krRun(const QString& arg = QString());
	QString krRunName(const QString& arg = QString());
	QString krRunStartDate(const QString& arg = QString());
	QString krRunNotes(const QString& arg = QString());

	QString krFacilityName(const QString& arg = QString());
	QString krFacilityDescription(const QString& arg = QString());

	QString krScanConfiguration(const QString& propertyName);

	QString krControl(const QString& controlName);
	QString krControlName(const QString &controlName);
	QString krControlDescription(const QString& controlName);
	QString krControlValue(const QString& controlName);
	QString krControlUnits(const QString& controlName);
	QString krControlEnum(const QString& controlName);


	QString krAllControls(const QString& arg = QString());

	QString krSample(const QString& arg = QString());
	QString krSampleName(const QString& arg = QString());
	QString krSampleElements(const QString& arg = QString());
	QString krSampleCreationDate(const QString& arg = QString());
	QString krSampleNotes(const QString& arg = QString());

	QString krDataSource(const QString& dataSourceName = QString());
	QString krDataSourceName(const QString& dataSourceName = QString());
	QString krDataSourceDescription(const QString& dataSourceName = QString());	// returns description if it has one, otherwise defaults to name
	QString krDataSourceUnits(const QString& dataSourceName = QString());
	QString krDataSourceSize(const QString& dataSourceName = QString());
	QString krDataSourceInfoDescription(const QString& dataSourceName = QString()); // returns info description if it has one, otherwise nothing4

	/// can only be used on the current data source, and only on 2D sources. Referring to the second dimension here (Columns in our output table)
	QString krDataSourceAxisValue(const QString& unused = QString());
	/// can only be used on the current data source, and only on 2D sources. Referring to the second dimension here (Columns in our output table)
	QString krDataSourceAxisUnits(const QString& unused = QString());

	QString krExporterAutoIncrement(const QString& arg = QString());

	QString krFileSystemAutoIncrement(const QString &arg = QString());

	/// Helper function to remove non-printable characters from a file name
	QString removeNonPrintableCharacters(const QString &fileName) const;

};

#endif // AMEXPORTER_H
