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


#ifndef AMEXPORTCONTROLLER_H
#define AMEXPORTCONTROLLER_H

#include <QObject>
#include <QMetaObject>
#include <QHash>
#include <QUrl>
#include "util/AMErrorMonitor.h"

class QStandardItemModel;


#define AMSCANACTION_CANT_CREATE_EXPORT_FOLDER 103107

/// This helper class is used by AMExportController to register the available exporters.  You should normally not need to use it directly.
class AMExporterInfo {
public:
	/// Constructor (Default values make an invalid AMExporterInfo)
	virtual ~AMExporterInfo();
	AMExporterInfo(const QMetaObject* exporterMetaObject = 0, QString exporterDescription = QString(), QString exporterOptionClassName = QString(), QString exporterLongDescription = QString() )
		: description(exporterDescription),
		  optionClassName(exporterOptionClassName),
		  longDescription(exporterLongDescription)
	{
		metaObject = exporterMetaObject;
	}

	bool isValid() const { return metaObject != 0; }

	const QMetaObject* metaObject;
	QString description, optionClassName, longDescription;

};

class AMExporter;
class AMExporterOption;
class AMScan;


/// Construct an AMExportController to supervise exporting a set of AMScans from the user's database.  Used in conjunction with an AMExportWizard, the user will be given options of different exporters ("file formats") to choose from, and will be able to review the option/template for their chosen exporter.  Finally, a progress bar will be displayed as the scans are exported.
/*! <b>Steps to using AMExportController</b>
  - [If it hasn't been done yet] Register all the exporter classes you want to enable using the static registerExporter<>() template function.  (An exporter class is a subclass of AMExporter that is used to write a specific file format, usually with its own set of options).

  - Construct an export controller with the list of database URLs you want to export

  - Call setDestinationFolderPath() with the complete path to the folder where you want to put the exported data.

  - Call chooseExporter() using one of the exporter classes previously registered.  (See registeredExporters() for the ones available.)

  - Call setOption() with an instance of the exporter options to use.  (An option is a subclass of AMExporterOption, and tells the exporter how to write the file, what to include, etc.  There is usually a specific option subclass for each exporter subclass.)  The option provided should be compatible with the exporter.

  - Call startExport() to start writing the files.  You can monitor progress() and progressChanged(), as well as status() and statusChanged(), while the exporter is running.  You can also call cancel() to give up at the current point.

Note that creating an AMExportWizard on an export controller will handle all but the first two steps for you, through a user-driven GUI interface:
\code
AMExportController exportController(myListOfScansToExport);
AMExportWizard* wizard = new AMExportWizard(&exportController, parentWidget);
\endcode

However, you can also use the AMExportController API directly, without employing an AMExportWizard, by proceeding through all the steps above.
*/
class AMExportController : public QObject
{
	Q_OBJECT
public:

	/// Used to represent the state of the export process.
	enum AMExportControllerState {
		Preparing,
		Exporting,
		Paused,
		Finished,
		Cancelled
	};

	/// Construct an AMExportController with a list of Urls to the scan items you wish to export.  The URLs should be in the amd://databaseConnectionName/tableName/objectId format.  The controller will supervise the rest of the process, and delete itself when finished (or cancelled).
	explicit AMExportController(const QList<QUrl>& scansToExport, QObject *parent = 0);

	explicit AMExportController(const QList<AMScan*>& scansToExport, QObject *parent = 0);

	virtual ~AMExportController();

	/// The export system lets the user choose from the set of available "file formats", where each file format is defined by an AMExporter subclass.  Call this static function to register all available AMExporter subclasses with the export system first, before actually using an AMExportController instance.
	template <class T>
	static bool registerExporter() {
		const QMetaObject* mo = &(T::staticMetaObject);

		// is this a subclass of AMExporter? (Or an AMExporter itself?)
		const QMetaObject* superClass = mo;
		bool inheritsAMExporter = false;
		while( (superClass=superClass->superClass()) && inheritsAMExporter == false ) {
			inheritsAMExporter = (superClass->className() == QString("AMExporter"));
		}
		if(!inheritsAMExporter)
			return false;	// can't register a non AMDbObject subclass.


		// is it already registered? return true.
		QString className(mo->className());
		if(registeredExporters_.contains(className)) {
			AMErrorMon::report(AMErrorReport(0, AMErrorReport::Debug, 0, QString("Export Support: The class '%1' has already been registered as an Exporter. Skipping duplicate registration.").arg(className)));
			return true;
		}

		T* exporter = new T();

		registeredExporters_.insert(className,
									AMExporterInfo(mo,
												   exporter->description(),
												   exporter->exporterOptionClassName(),
												   exporter->longDescription()));

		delete exporter;

		return true;
	}

	template <class T>
	static bool unregisterExporter()
	{
		const QMetaObject* mo = &(T::staticMetaObject);

		QString className(mo->className());

		if(!registeredExporters_.contains(className)) {

			AMErrorMon::debug(0, 0, QString("Export Support: The class '%1' was not registered as an Exporter. Skipping unregistration.").arg(className));
			return false;
		}

		registeredExporters_.remove(className);

		return true;
	}

	/// Access the set of currently-registered exporters
	static const QHash<QString, AMExporterInfo>& registeredExporters() { return registeredExporters_; }

	/// How many scans are being exporter?
	int scanCount() const {
		if(usingScanURLs_)
			return scanURLsToExport_.count();
		if(usingScanObjects_)
			return scanObjectsToExport_.count();
		return 0;
	}

	/// Converts a database url into a scan
	AMScan* scanFromUrl(const QUrl& scanUrl);

	/// Call this to select and load an exporter subclass (using the class name of an exporter registered previously, using registerExporter()).  Returns true if valid.  Once this has been called and has returned true, exporter() will return a valid instance.
	bool chooseExporter(const QString& exporterClassName);

	/// The exporter instance to use.  This returns 0 until chooseExporter() is called with a valid exporter class name for the first time.
	AMExporter* exporter() { return exporter_; }

	/// Returns the complete path to the folder where exported files should be saved
	QString destinationFolderPath() const {
		return destinationFolderPath_;
	}

	/// Set the option to use for exporting this set of scans. The controller will take ownership of the option, and delete any previous option if it exists. Set to 0 to delete and clear the current option.
	void setOption(AMExporterOption* option);

	/// Returns the currently active option, which will be used for exporting this set of scans.  Returns 0 if no active option has been set.
	AMExporterOption* option() const {
		return option_;
	}

	/// Returns a pointer to a QStandardItemModel containing information about all the data sources available in the list of scans to export.  This model is only populated (and slowly in the background, at that!) after calling searchForAvailableDataSources().  It's merely provided to assist user interfaces in displaying the available data sources.  See availableDataSourcesModel_ for details.
	QStandardItemModel* availableDataSourcesModel() const { return availableDataSourcesModel_; }

	/// Returns the current state of the export process:
	AMExportControllerState state() const { return state_; }

	/// Returns the current progress of the export process: the current scan index (from 0 to scanCount())
	int progress() const { return exportScanIndex_; }

signals:

	void stateChanged(int exportControllerState);
	void statusChanged(const QString& status);
	void progressChanged(int currentScan, int totalScans);

public slots:

	/// Set the default destination folder path. Exported files will be saved to here.
	void setDefaultDestinationFolderPath();
	/// Set the destination folder path. Exported files will be saved to here.
	void setDestinationFolderPath(const QString& fullPathToFolder) { destinationFolderPath_ = fullPathToFolder; }

	/// Start searching the list of scans to export for all available data sources
	void searchForAvailableDataSources();

	/// Call to start the export process.  Will return false if either chooseExporter() or setOption() haven't been called yet, or if an export has already been started.
	bool start(bool autoExport = false);

	/// Call to cancel an export in progress. Will return false if the export is not currently in the Exporting or Paused state(). If successfully cancelled, the state() will become Cancelled.
	bool cancel();

	/// Call to pause an export in progress.  Will return false if the export is not currently in the Exporting state().
	bool pause();

	/// Call to resume an export in progress.  Will return false if the export is not currently in the Paused state().
	bool resume();

	/// Number of scans successfully exported so far
	int succeededCount() const { return succeededCount_; }
	/// Number of scans that failed so far with an error
	int failedCount() const { return failedCount_; }



protected:

	/// The set of registered, available AMExporter classes
	static QHash<QString, AMExporterInfo> registeredExporters_;

	/// The list of scans to export
	QList<QUrl> scanURLsToExport_;
	/// Alternatively, the list of actual scan objects to export
	QList<AMScan*> scanObjectsToExport_;
	bool usingScanURLs_;
	bool usingScanObjects_;

	/// Instance of the AMExporter subclass to use.  0 until chooseExporter() is called with a valid exporter class name.
	AMExporter* exporter_;

	/// Where to save the exported files (complete path to folder/directory)
	QString destinationFolderPath_;

	/// The option to use with the exporter
	AMExporterOption* option_;

	/// Used only to help users choose data sources, by letting them know what data sources are available in the list of scans.  This model is slowly filled in the background ONLY AFTER CALLING searchForAvailableDataSources(), since doing it all at once could take a long time.
	/*! It contains:

- Qt::DisplayRole (QString): "[nD] name: description"

- AM::DescriptionRole (QString): description

- AM::NameRole (QString): name

- AM::UserRole (int): rank
*/
	QStandardItemModel* availableDataSourcesModel_;

	/// Index of the current scan in scansToExport_ that we are searching for data sources
	int searchScanIndex_;

	/// Add an available data source we just found to the availableDataSourcesModel_, if it's not in there already.
	void addFoundAvailableDataSource(const QString& name, const QString& description, int rank);

	/// Index of the current scan in scansToExport_ that we are now exporting
	int exportScanIndex_;

	/// The current state of the export process: Preparing, Exporting, Finished, or Cancelled
	AMExportControllerState state_;

	/// Descriptive status of progress (ex: "Loading: My Scan #7")
	QString status_;

	/// Number of scans exported, failed
	int succeededCount_, failedCount_;

protected slots:
	/// Called periodically to search scans for their data sources
	void continueAvailableDataSourceSearch();

	/// Called repeatedly to export the current scan
	void continueScanExport();
};

#endif // AMEXPORTCONTROLLER_H
