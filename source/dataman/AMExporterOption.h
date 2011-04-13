#ifndef AMEXPORTEROPTION_H
#define AMEXPORTEROPTION_H

#include "dataman/AMDbObject.h"

/// An exporter object (AMExporter) writes a scan to file according to a set of "export options".  This class specifies the export options for the "general" class of file exporters, which are designed to be as flexible as possible in the data that they can write.  See AMExporterOptionGeneralAscii for a complete example.
class AMExporterOption : public AMDbObject
{
	Q_OBJECT
	Q_PROPERTY(QString fileName READ fileName WRITE setFileName)

public:
	explicit AMExporterOption(QObject *parent = 0);

	/// The file name to save at (not including the destination folder prefix).  Can include "\keyword" replacement tags.  If it includes "/" separators, subfolders will be generated as required within the destination folder.
	QString fileName() const { return fileName_; }

signals:

public slots:
	/// Set the file name to save at (not including the destination folder prefix).  Can include "\keyword" replacement tags.  If it includes "/" separators, subfolders will be generated as required within the destination folder.
	void setFileName(const QString& fileName) { fileName_ = fileName; setModified(true); }

protected:
	/// The file name to save at (not including the destination folder prefix).  Can include "\keyword" replacement tags.  If it includes "/" separators, subfolders will be generated as required within the destination folder.
	QString fileName_;



};

#endif // AMEXPORTEROPTION_H
