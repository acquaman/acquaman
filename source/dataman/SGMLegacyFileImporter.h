#ifndef SGMLEGACYFILEIMPORTER_H
#define SGMLEGACYFILEIMPORTER_H

#include "AMBiHash.h"
#include "beamline/AMAbstractDetector.h"


class AMXASScan;

/// This class encapsulates importing and exporting legacy SGM data files (XAS scans from ???? to 2010) to an AMXASScan
/*! \todo Should we implement a whole family of abstract and actual classes that encapsulate the dirty details of all file formats?
  */
class SGMLegacyFileImporter
{
public:
	SGMLegacyFileImporter(AMXASScan* scan = 0)
		: scan_(scan)
	{
		// this static storage can be shared across all instances, but if we're the first, need to populate it.
		if(columns2pvNames_.count() == 0) {
			columns2pvNames_.set("eV", "BL1611-ID-1:Energy");
			columns2pvNames_.set("ringCurrent", "PCT1402-01:mA:fbk");
			columns2pvNames_.set("I0_2", "A1611-4-13:A:fbk");
			columns2pvNames_.set("I0", "A1611-4-14:A:fbk");
			columns2pvNames_.set("tey", "A1611-4-15:A:fbk");
			columns2pvNames_.set("tfy", "A1611-4-16:A:fbk");
			columns2pvNames_.set("eV_fbk", "BL1611-ID-1:Energy:fbk");
			columns2pvNames_.set("pressure", "TCGC1611-423:pressure:fbk");

			columns2pvNames_.set("integrationTime", "A1611I1:cont_interval");
			columns2pvNames_.set("grating", "SG16114I1001:choice");
			columns2pvNames_.set("time", "Absolute-Time-Stamp");
		}
		if(sgmLegacyDetectors_.count() == 0){
			AMAbstractDetector *sgmDetector;
			sgmDetector = new AMAbstractDetector("tey", false);
			sgmLegacyDetectors_ << sgmDetector;
			sgmLegacyDetectorNames_ << sgmDetector->name();
			sgmDetector = new AMAbstractDetector("tfy", false);
			sgmLegacyDetectors_ << sgmDetector;
			sgmLegacyDetectorNames_ << sgmDetector->name();
			QStringList yElementNames;
			yElementNames << "count";
			sgmDetector = new AMAbstractSpectralOutputDetector("pgt", 1024, "eV", yElementNames);
			sgmLegacyDetectors_ << sgmDetector;
			sgmLegacyDetectorNames_ << sgmDetector->name();
			sgmDetector = new AMAbstractDetector("I0", false);
			sgmLegacyDetectors_ << sgmDetector;
			sgmLegacyDetectorNames_ << sgmDetector->name();
			sgmDetector = new AMAbstractDetector("eVFbk", false);
			sgmLegacyDetectors_ << sgmDetector;
			sgmLegacyDetectorNames_ << sgmDetector->name();
		}
	}


	/// load from SGM legacy file format
	bool loadFromFile(const QString& filepath);

	/// save to SGM legacy file format (\todo UNIMPLEMENTED)
	bool saveToFile(const QString& filepath) {
		Q_UNUSED(filepath)
		return false;
	}

	/// set the target of this importer/exporter.
/*! There are two ways to attach an importer/exporter to a scan:
\code
AMXASScan myScan;
AMXASScan scan2;

SGMLegacyFileImporter myImporter(&myScan);
or...
myImporter = scan2;
\endcode
*/

	void operator=(AMXASScan& scan) {
		scan_ = &scan;
	}

	/// translate a PV string from the file header to a meaningful name, if we recognize it.  Unrecognized PV strings are untouched and false is returned.
	static bool pv2columnName(QString& pv) {
		QString newName = columns2pvNames_.valueR(pv);
		if(newName.isEmpty())
			return false;
		pv = newName;
		return true;
	}


protected:

	/// The target operated on by this importer
	AMXASScan* scan_;

	/// A forward- and reverse-mapping from meaningful data column names to process variable strings
	static AMBiHash<QString, QString> columns2pvNames_;
	QList<AMAbstractDetector*> sgmLegacyDetectors_;
	QStringList sgmLegacyDetectorNames_;
};



#endif // SGMLEGACYFILEINFO_H
