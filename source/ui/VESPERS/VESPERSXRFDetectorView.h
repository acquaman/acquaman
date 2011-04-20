#ifndef VESPERSXRFDETECTORVIEW_H
#define VESPERSXRFDETECTORVIEW_H

#include <QWidget>

#include "ui/VESPERS/XRFDetectorView.h"
#include "util/AMElement.h"
#include "ui/AMPeriodicTableView.h"
#include "beamline/VESPERS/VESPERSBeamline.h"

class VESPERSXRFDetectorView : public QWidget
{
	Q_OBJECT
public:
	/// Constructor for the detector view.  Handles setting regions of interest and other such things.  Contains a generic XRFDetector view and adds periodic tables and element views.
	explicit VESPERSXRFDetectorView(XRFDetector *detector, QWidget *parent = 0);

signals:

public slots:

protected slots:

	/// Adds a region of interest from the detector.
	void addRegionOfInterest(AMElement *el, QPair<QString, QString> line);
	/// Removes a region of interest from the detector.
	void removeRegionOfInterest(AMElement *el, QPair<QString, QString> line);

protected:

	/// The detector view.
	XRFDetailedDetectorView *detectorView_;
	/// A pointer to the detector.
	XRFDetector *detector_;

	/// The periodic table view.
	AMPeriodicTableView *tableView_;
};

#endif // VESPERSXRFDETECTORVIEW_H
