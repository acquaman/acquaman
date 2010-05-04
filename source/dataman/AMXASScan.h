#ifndef AMXASSCAN_H
#define AMXASSCAN_H


#include "dataman/AMScan.h"


/// This is a practical subclass of AMScan which provides the details to represent a very basic absorption scan (ex: eV setpoints and readings from 0-dimensional detectors such as the sample current (TEY) and fluorescence yield (TFY).
/*! blah blah blah
	\todo detailed doc.

	See AMXASSpectrumScan for scans which include 1D data (such as the SGM's silicon drift detector) for every eV datapoint.
  */
class AMXASScan : public AMScan {
Q_OBJECT
public:
	/// create a new XAS scan with the following named \c detectors. Each "detector" is a source of a datapoint, that will be stored/logged, available as a column of raw data, and accessible through channel(s).
	explicit AMXASScan(const QStringList& detectors = QStringList(), QObject *parent = 0);

	/// the detectors (raw data columns) available within this scan. Does not include the primary column (eV), which is always present.
	QStringList detectors() const { return detectors_; }

	/// Add a new named detector (returns false if detector already exists)
	bool addDetector(const QString& uniqueDetectorName);

	/// \todo access will be through channels which can access the AMDataTree raw columns (d_). Until channels are done, there's no way to peek inside.

	/// \todo append new data values... (from outside, or inside the class?)
	/// \todo clear data? (allow from outside the class?)
	/// \todo some way to encapsulate a bunch of information about detectors: units, detailed comment on where it comes from, whether to show or hide by default in user-facing views.

signals:
	// inherits updated(AMScan*)

public slots:

protected:
	QStringList detectors_;

	QString legacyGrating_;
	QString legacyIntegrationTime_;

	// called by friends after finished updating / loading from file, etc.
	void notifyUpdated() {
		emit updated(this);
	}

	friend class SGMLegacyFileImporter;
	friend class AMAcqScanOutput;
	friend class AMDacqScanController;

};

#endif // AMXASSCAN_H
