#ifndef AMXASSCAN_H
#define AMXASSCAN_H


#include "dataman/AMScan.h"
#include "beamline/AMDetector.h"


/// This is a practical subclass of AMScan which provides the details to represent a very basic absorption scan (ex: eV setpoints and readings from 0-dimensional detectors such as the sample current (TEY) and fluorescence yield (TFY).
/*! blah blah blah
	\todo detailed doc.

	See AMXASSpectrumScan for scans which include 1D data (such as the SGM's silicon drift detector) for every eV datapoint.
  */
class AMXASScan : public AMScan {
Q_OBJECT
public:
	/// create a new XAS scan with the following named \c detectors. Each "detector" is a source of a datapoint, that will be stored/logged, available as a column of raw data, and accessible through channel(s).
	explicit AMXASScan(const QList<AMDetectorInfo*> &detectors = QList<AMDetectorInfo*>(), QObject *parent = 0);

	/// Re-implemented from AMScan. Currently only the SGM2004 file format is supported.
	virtual bool loadData();

	/// the detectors (raw data columns) available within this scan. Does not include the primary column (eV), which is always present.
	QStringList detectorNames() const { return detectorNames_; }
	QList<const AMDetectorInfo*> detectors() const { return detectors_; }

	/// Add a new named detector (returns false if detector already exists)
	bool addDetector(const AMDetectorInfo* uniqueDetector);

	/// \todo access will be through channels which can access the AMDataTree raw columns (d_). Until channels are done, there's no way to peek inside.

	/// \todo append new data values... (from outside, or inside the class?)
	/// \todo clear data? (allow from outside the class?)
	/// \todo some way to encapsulate a bunch of information about detectors: units, detailed comment on where it comes from, whether to show or hide by default in user-facing views.

	/// Reimplemented from AMScan; provides a general human-readable description
	virtual QString typeDescription() const {
		return "Xray Absorption Scan";
	}

signals:
	// inherits dataChanged(AMScan*)

public slots:

protected slots:

protected:
	QStringList detectorNames_;
	QList<const AMDetectorInfo*> detectors_;

	QString legacyGrating_;
	QString legacyIntegrationTime_;

	friend class SGM2004FileLoader;

};

#endif // AMXASSCAN_H
