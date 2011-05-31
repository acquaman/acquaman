#ifndef XRFPERIODICTABLEVIEW_H
#define XRFPERIODICTABLEVIEW_H

#include <QWidget>

#include "ui/AMPeriodicTableView.h"
#include "util/VESPERS/XRFPeriodicTable.h"

class XRFPeriodicTableView : public QWidget
{
	Q_OBJECT
public:
	/// Constructor.  Needs the table for viewing.  Does not create a new instance of the table.
	explicit XRFPeriodicTableView(XRFPeriodicTable *xrfTable, QWidget *parent = 0);

	/// Returns the current list of emission lines used by this view.
	const QList<QPair<int, QString> > roiList() const { return xrfTable_->checkedList(); }

signals:
	/// Passes on the signal that an element was selected.  Contains the element.
	void elementSelected(AMElement *);
	/// Passes on the signal that an element was selected.  Contains the element and a list of active elements in a QPair of <atomic number, line name>.
	void elementClicked(AMElement *, QList<QPair<int, QString> >);
	/// Signal to notify others that the new region of interest is acceptable.
	void addRegionOfInterest(AMElement *, QPair<QString, QString>);
	/// Signal to notify others that the selected region of interest was successfully removed.
	void removeRegionOfInterest(AMElement *, QPair<QString, QString>);
	/// Signal to notify that all regions of interest should be removed.
	void clearAllRegionsOfInterest();

public slots:
	/// Changes the color of the element button due to an ROI being added.
	void regionOfInterestAdded(AMElement *el, QPair<QString, QString> line);
	/// Reverts the color of the button back to its original color scheme due to an ROI being deleted.
	void regionOfInterestRemoved(AMElement *el, QPair<QString, QString> line);

protected slots:
	/// Handles the element clicked signal.
	void onElementSelected(AMElement *el) { emit elementClicked(el, xrfTable_->checkedList()); }
	/// Clears the entire list of regions of interest.
	void clearList();
	/// Helper function that disables the buttons based on the current values of maximumEnergy_ and minimumEnergy_.
	void disableElements();

protected:
	/// A pointer to the XRF periodic table.
	XRFPeriodicTable *xrfTable_;
	/// A generic periodic table view.
	AMPeriodicTableView *tableView_;
};

#endif // XRFPERIODICTABLEVIEW_H
