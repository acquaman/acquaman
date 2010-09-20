#include "ALSBL8XASFileLoader.h"

#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QDateTime>
#include "dataman/AMXASScan.h"
#include "dataman/AMDetectorInfo.h"

#include <QDebug>


AMBiHash<QString, QString> ALSBL8XASFileLoader::columns2fileFormatHeaders_;


ALSBL8XASFileLoader::ALSBL8XASFileLoader(AMXASScan* scan) : AMAbstractFileLoader(scan)
{
	// this static storage can be shared across all instances, but if we're the first, need to populate it.
	if(columns2fileFormatHeaders_.count() == 0) {
		columns2fileFormatHeaders_.set("eV", "Mono Energy");
		columns2fileFormatHeaders_.set("ringCurrent", "Beam Current");
		columns2fileFormatHeaders_.set("I0", "Counter 0");
		columns2fileFormatHeaders_.set("tey", "Counter 1");
		columns2fileFormatHeaders_.set("tfy", "Counter 2");
	}
}


bool ALSBL8XASFileLoader::loadFromFile(const QString& filepath, bool extractMetaData, bool createChannels) {

	// not initialized to have a scan target, or scan target is not an AMXASScan...
	AMXASScan* scan = qobject_cast<AMXASScan*>(scan_);
	if(!scan)
		return false;

	// information about the scan we hope to locate:
	QString comments;
	double integrationTime;

	// used in parsing the data file
	QString line;
	QStringList lp;

	// names of the columns, taken from headers in the data file.
	QStringList colNames1;

	// open the file:
	QFile f(filepath);
	if(!f.open(QIODevice::ReadOnly)) {
		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Serious, -1, "ALSBL8XASFileLoader parse error while loading scan data from file. Missing file."));
		return false;
	}
	QTextStream fs(&f);


	if(extractMetaData) {
		// Start reading the file. look for the count-time line.
		while( !fs.atEnd() && !(line = fs.readLine()).startsWith("Count Time (s):") )
			;
		if(fs.atEnd()) {
			AMErrorMon::report(AMErrorReport(0, AMErrorReport::Debug, -2, "ALSBL8XASFileLoader parse error while loading scan data from file. Could not find the count time (integration time)."));
			fs.seek(0);
			// return false;	// bad format; missing the count time string
		}
		else {
			// read it
			integrationTime = line.split(':').at(1).trimmed().toDouble();
		}


		// Keep reading the file. look for the comment line.
		while( !fs.atEnd() && !(line = fs.readLine()).startsWith("Description Length:") )
			;
		if(fs.atEnd()) {
			AMErrorMon::report(AMErrorReport(0, AMErrorReport::Debug, -2, "ALSBL8XASFileLoader parse error while loading scan data from file. Could not find the description."));
			fs.seek(0);
			// return false;	// bad format; missing the comment string
		}
		else {

			// read the comment
			bool descriptionLengthOk;
			int descriptionLength = line.split(':').at(1).trimmed().toInt(&descriptionLengthOk);
			// read this many characters now...
			if(descriptionLengthOk)
				comments = fs.read(descriptionLength);
		}
	}


	// find out what columns exist. Looking for line starting with 'Time (s)'
	line.clear();
	while(!fs.atEnd() && !line.startsWith("Time (s)"))
		line = fs.readLine();
	if(fs.atEnd()) {
		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Serious, -2, "ALSBL8XASFileLoader parse error while loading scan data from file. Missing the Column Header line."));
		return false;	// bad format; missing the column header
	}
	colNames1 = line.split(QChar('\t'));

	// translate header names into meaningful column names (ex: "Counter 0" ---> "I0".... "Counter1"--->"tey")
	for(int i=0; i<colNames1.count(); i++)
		header2columnName(colNames1[i]);

	// ensure that we have the basic "eV" column
	int eVIndex = colNames1.indexOf("eV");
	if(eVIndex < 0) {
		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Serious, -3, "ALSBL8XASFileLoader parse error while loading scan data from file. I couldn't find the energy (eV) column."));
		return false;	// bad format; no primary column

	}

	// ensure raw data columns exist:
	scan->d_->removeAll();
	foreach(QString colName, colNames1) {
		if(colName != "eV"){
			scan->addDetector(new AMDetectorInfo(colName, false));
		}
	}

	// read all the data. Add to data columns or scan properties depending on the event-ID.
	while(!fs.atEnd()) {

		line = fs.readLine();

		// Data line. If there are the correct number of columns:
		if( (lp = line.split('\t', QString::SkipEmptyParts)).count() == colNames1.count() ) {

			// append a new datapoint to the data tree (supply primary eV value here)
			scan->d_->append(lp.at(eVIndex).toDouble());	// insert eV

			// add all columns (but ignore the eV column)
			for(int i=1; i<colNames1.count(); i++) {
				if(i!=eVIndex)
					scan->d_->setLastValue(colNames1.at(i), lp.at(i).toDouble());
			}
		}
	}


	if(extractMetaData) {
		scan->setNotes(comments);
		// for a date-time, there is no information saved inside the file format, so the best we can do is look at the file's creation date-time...
		QFileInfo fi(filepath);
		scan->setDateTime(fi.lastModified());
		/// \todo integration time... do what with?
	}

	// If the scan doesn't have any channels yet, it might be helpful to create some.
	if(createChannels) {
		/// \todo defaults for what channels to create?
		// scan->addChannel("eV", "eV");
		if(colNames1.contains("tey") && colNames1.contains("I0"))
			scan->addChannel("tey_n", "tey/I0");
		if(colNames1.contains("tfy") && colNames1.contains("I0"))
			scan->addChannel("tfy_n", "tfy/I0");
		if(colNames1.contains("tey"))
			scan->addChannel("tey_raw", "tey");
		if(colNames1.contains("tfy"))
			scan->addChannel("tfy_raw", "tfy");
		if(colNames1.contains("I0"))
			scan->addChannel("I0", "I0");
	}

	scan->onDataChanged();

	return true;
}
