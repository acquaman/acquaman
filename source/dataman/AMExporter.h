#ifndef AMEXPORTER_H
#define AMEXPORTER_H

#include <QObject>

class AMExporterOption;
class AMScan;

/// This class defines the interface for objects which can be used export scans in different formats.  For example, AMExporterGeneralAscii, AMExporterExcel, AMExporterOrigin, and AMExporterNexus all inherit this interface.
/*! To implement a new exporter, you must subclass AMExporter.  Additionally, you must declare a default constructor and tag it with the Q_INVOKABLE macro, and call AMExportController::registerExporter<MyExporterSubclass>() to make it known to the export system. */
class AMExporter : public QObject
{
	Q_OBJECT
public:
	explicit AMExporter(QObject *parent = 0) : QObject(parent) {}

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

	/// Exports the given \c scan object, using the option set \c option, to the complete destination file path \c destinationPath
	virtual bool exportScan(const AMScan* scan, const QString& destinationPath, const AMExporterOption* option) const = 0;

//	/// create an "exporter option" (an instance of an AMExporterOption subclass) that is a valid default for this type of exporter
//	virtual AMExporterOption* createDefaultOption() const = 0;

signals:

public slots:

};

#endif // AMEXPORTER_H
