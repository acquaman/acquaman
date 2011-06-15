#ifndef XRFFREERUN_H
#define XRFFREERUN_H

#include <QObject>

#include "beamline/VESPERS/XRFDetector.h"
#include "beamline/VESPERS/VESPERSBeamline.h"
#include "util/VESPERS/XRFPeriodicTable.h"

class XRFFreeRun : public QObject
{
	Q_OBJECT
public:
	/// Constructor.  Links the XRF periodic table model to the XRF detector.
	explicit XRFFreeRun(QObject *parent = 0);

	/// Returns the detector used in this configuration.
	XRFDetector *detector() const { return detector_; }
	/// Returns the XRF periodic table used by this configuration.
	XRFPeriodicTable *table() const { return xrfTable_; }

signals:

public slots:
	/// Sets the detector.
	void setDetector(XRFDetector *detector) { detector_ = detector; }

protected slots:
	/// Handles what happens when the detector becomes connected.
	void onRoisHaveValues();
	/// Handles when the regions of interest change from an external source.
	void onExternalRegionsOfInterestChanged();

protected:
	/// Helper function that takes in a region of interest name and adds it to the XRFPeriodicTable.  Takes in the ROI name, finds the element and line it is associated with and adds it to the XRFPeriodicTable.
	void addRegionOfInterestToTable(QString name);

	/// The detector itself.  This has live beamline communications.
	XRFDetector *detector_;

	/// The periodic table that holds information about the regions of interest.
	XRFPeriodicTable *xrfTable_;
};

#endif // XRFFREERUN_H
