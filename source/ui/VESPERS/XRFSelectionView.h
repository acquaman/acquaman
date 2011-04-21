#ifndef XRFSELECTIONVIEW_H
#define XRFSELECTIONVIEW_H

#include <QWidget>

#include "util/AMElement.h"
#include "ui/VESPERS/XRFPeriodicTableView.h"
#include "beamline/VESPERS/VESPERSBeamline.h"
#include "ui/VESPERS/VESPERSXRFElementView.h"

class XRFSelectionView : public QWidget
{
	Q_OBJECT
public:
	explicit XRFSelectionView(QWidget *parent = 0);

signals:
	/// Passes on the signal that an element was selected.  Contains the element.
	void elementSelected(AMElement *);

public slots:

protected slots:

	/// Adds a region of interest from the detector.
	void addRegionOfInterest(AMElement *el, QPair<QString, QString> line);
	/// Removes a region of interest from the detector.
	void removeRegionOfInterest(AMElement *el, QPair<QString, QString> line);

protected:

	/// A pointer to the detector.
	XRFDetector *detector_;

	/// A pointer to the xrf periodic table view.
	XRFPeriodicTableView *tableView_;
};

#endif // XRFSELECTIONVIEW_H
