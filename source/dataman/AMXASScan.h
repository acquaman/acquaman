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
	Q_CLASSINFO("AMDbObject_Attributes", "shareTableWithClass=AMScan;description=XRay Absorption Scan")

public:
	/// create a new XAS scan with the following named \c detectors. Each "detector" is a source of a datapoint, that will be stored/logged, available as a column of raw data, and accessible through channel(s).
	Q_INVOKABLE explicit AMXASScan(QObject *parent = 0);

	/// Re-implemented from AMScan. Currently only the SGM2004 and ALS Bl8.0.1 file formats are supported.
	virtual bool loadDataImplementation();

signals:
	// inherits dataChanged(AMScan*)

public slots:

protected slots:

protected:

	QString legacyGrating_;
	QString legacyIntegrationTime_;

	friend class SGM2004FileLoader;
	friend class ALSBL8XASFileLoader;

};

#endif // AMXASSCAN_H
