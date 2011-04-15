#ifndef AMEXPORTCONTROLLER_H
#define AMEXPORTCONTROLLER_H

#include <QObject>
#include <QMetaObject>
#include <QHash>
#include <QUrl>
#include "util/AMErrorMonitor.h"



/// This helper class is used by AMExportController to register the available exporters.  You should normally not need to use it directly.
class AMExporterInfo {
public:
	/// Constructor (Default values make an invalid AMExporterInfo)
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


/// Construct an AMExportController to supervise exporting a set of AMScans from the user's database.  Used in conjunction with an AMExportWizard, the user will be given options of different exporters ("file formats") to choose from, and will be able to review the option/template for their chosen exporter.  Finally, a progress bar will be displayed as the scans are exported.
class AMExportController : public QObject
{
	Q_OBJECT
public:
	/// Construct an AMExportController with a list of Urls to the scan items you wish to export.  The URLs should be in the amd://databaseConnectionName/tableName/objectId format.  The controller will supervise the rest of the process, and delete itself when finished (or cancelled).
	explicit AMExportController(const QList<QUrl>& scansToExport);

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


	/// Access the set of currently-registered exporters
	static const QHash<QString, AMExporterInfo>& registeredExporters() { return registeredExporters_; }

	/// How many scans are being exporter?
	int scanCount() const { return scansToExport_.count(); }


	/// Call this to select and load an exporter subclass (using the class name of an exporter registered previously, using registerExporter()).  Returns true if valid.  Once this has been called and has returned true, exporter() will return a valid instance.
	bool chooseExporter(const QString& exporterClassName);

	/// The exporter instance to use.  This returns 0 until chooseExporter() is called with a valid exporter class name for the first time.
	AMExporter* exporter() { return exporter_; }

	/// Returns the complete path to the folder where exported files should be saved
	QString destinationFolderPath() const {
		return destinationFolderPath_;
	}



signals:

public slots:

	/// Set the destination folder path. Exported files will be saved to here.
	void setDestinationFolderPath(const QString& fullPathToFolder) {
		destinationFolderPath_ = fullPathToFolder;
	}

protected:

	/// The set of registered, available AMExporter classes
	static QHash<QString, AMExporterInfo> registeredExporters_;

	/// The list of scans to export
	QList<QUrl> scansToExport_;

	/// Instance of the AMExporter subclass to use.  0 until chooseExporter() is called with a valid exporter class name.
	AMExporter* exporter_;

	/// Where to save the exported files (complete path to folder/directory)
	QString destinationFolderPath_;

};

#endif // AMEXPORTCONTROLLER_H
