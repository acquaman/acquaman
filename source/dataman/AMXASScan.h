#ifndef AMXASSCAN_H
#define AMXASSCAN_H


#include "dataman/AMScan.h"

/// This is a practical subclass of AMScan which provides the details to represent a very basic absorption scan (ex: eV setpoints and readings from 0-dimensional detectors such as the sample current (TEY) and fluorescence yield (TFY).
/*! \todo detailed doc.
	\sa AMXASSpectrumScan for scans which include 1D data (such as the SGM's silicon drift detector) for every eV datapoint.
  */

class AMXASScan : public AMScan {
Q_OBJECT
public:
	/// create a new XAS scan with the following named \c detectors. Each "detector" is a source of a datapoint, that will be stored/logged, available as a column of raw data, and accessible through channel(s).
	explicit AMXASScan(const QStringList& detectors = QStringList(), QObject *parent = 0);

	/// Add a new named detector (returns false if detector already exists)
	bool addDetector(const QString& uniqueDetectorName) {
		if(detectors_.contains(uniqueDetectorName))
			return false;
		detectors_ << uniqueDetectorName;
		d_.createColumn(uniqueDetectorName);
		return true;
	}

	/// the number of datapoints in the scan:
	unsigned count() const { return d_.count(); }

	/// load from SGM legacy file format (this may or may not be permanent; might want a general loading system)
	bool loadFromSGMLegacyFile(const QString& filepath);

	/// save to SGM legacy file format (this may or may not be permanent; might want a general loading system)
	bool saveToSGMLegacyFile(const QString& filepath) const;

signals:
	// inherits updated(AMScan*)

public slots:

protected:
	QStringList detectors_;

	double legacyIntegrationTime_;

};

#endif // AMXASSCAN_H
