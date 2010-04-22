#include "AMXASScan.h"

#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QDateTime>


AMXASScan::AMXASScan(const QStringList& detectors, QObject *parent)
	: AMScan(parent),
	detectors_(detectors)
{
	// setup default columns in data structure:
	d_.setXName("eV");

	// create columns for each detector (ex: "tey", "tfy", "urinalWaterTemp", etc.)
	foreach(QString detect, detectors_)
		addDetector(detect);


}



/// load from SGM legacy file format (this may or may not be permanent; might want a general loading system)
bool AMXASScan::loadFromSGMLegacyFile(const QString& filepath) {

	QString comments;
	QDateTime datetime;
	double integrationTime;
	QString grating;
	double unknown1, motor1_fbk, unknown2;

	QFile f(filepath);
	if(!f.open(QIODevice::ReadOnly)) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Serious, -1, "AMXASScan: parse error while loading scan data from file. Missing file."));
		return false;
	}
	QTextStream fs(&f);

	// major assumption: all sgm files like this one from 2010-March. (static parsing, not dynamic based on comments)

	/// \todo delete existing columns?

	// ensure columns exist:
	d_.removeAll();	// remove all non-pricipal columns

	addDetector("ringCurrent");
	addDetector("tey");
	addDetector("tfy");
	addDetector("photodiode");
	/// \todo ? last picoammeter src?
	addDetector("amps4");
	addDetector("motor2_fbk");
	addDetector("eV_fbk");
	addDetector("motor3_fbk");
	addDetector("pressure");

	// Start reading the file. look for comment line.
	while( !fs.atEnd() && fs.readLine() != QString("# COMMENT"));
	if(fs.atEnd()) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Serious, -2, "AMXASScan: parse error while loading scan data from file. Missing comment."));
		return false;	// bad format; missing the comment string
	}

	// read the comment
	QString line = fs.readLine();
	// chop off the "# "
	comments = line.remove(0,1).trimmed();



	// find the '# 2, ' line containing some standalone information
	line.clear();
	while(!fs.atEnd() && !line.startsWith("# 2,"))
		line = fs.readLine();
	if(fs.atEnd()) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Serious, -2, "AMXASScan: parse error while loading scan data from file. Missing #(2) event line."));
		return false;	// bad format; missing the #2 event line
	}
	QStringList lp = line.split(QChar(','));
	if(lp.count() != 7) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Serious, -2, "AMXASScan: parse error while loading scan data from file. Wrong length of #(2) event line."));
		return false;	// bad format, missing some data
	}
	datetime = QDateTime::fromTime_t(lp.at(1).toDouble());
	unknown1 = lp.at(2).toDouble();
	motor1_fbk = lp.at(3).toDouble();
	integrationTime = lp.at(4).toDouble();
	grating = lp.at(5);
	unknown2 = lp.at(6).toDouble();

	// read all the data:
	while(!fs.atEnd()) {
		line = fs.readLine();
		if(line.startsWith("1,") && (lp = line.split(',')).count() == 11 ) {
			d_.append(lp.at(1).toDouble());	// insert eV
			d_.setLastValue("ringCurrent", lp.at(2).toDouble());
			d_.setLastValue("tey", lp.at(3).toDouble());
			d_.setLastValue("tfy", lp.at(4).toDouble());
			d_.setLastValue("photoDiode", lp.at(5).toDouble());
			d_.setLastValue("amps4", lp.at(6).toDouble());
			d_.setLastValue("motor2_fbk", lp.at(7).toDouble());
			d_.setLastValue("eV_fbk", lp.at(8).toDouble());
			d_.setLastValue("motor3_fbk", lp.at(9).toDouble());
			d_.setLastValue("pressure", lp.at(10).toDouble());
		}
	}


	// success. Set all scan paramenters, now that we know we've won.
	setComments(comments);
	setDateTime(datetime);
	// setSampleName() ?
	legacyIntegrationTime_ = integrationTime;


	emit updated(this);

	return true;
}


bool AMXASScan::saveToSGMLegacyFile(const QString& filepath) const {

	/// \todo
	return false;


}
