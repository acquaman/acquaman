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

protected slots:
	/// Changes the color of the button associated with the element based on the emission lines it has.  Returns to a normal look if there are no selected emission lines.
	void changeColor(XRFElement *el);
	/// Resets all the colors of the buttons to their original state.  Called when the trash button is clicked.
	void resetAllColors();
	/// Handles the element clicked signal.
	void onElementSelected(AMElement *el) { xrfTable_->setCurrentElement(xrfTable_->elementByAtomicNumber(el->atomicNumber())); }
	/// Helper function that disables the buttons based on the current values of maximumEnergy_ and minimumEnergy_.
	void disableElements();

protected:
	/// A pointer to the XRF periodic table.
	XRFPeriodicTable *xrfTable_;
	/// A generic periodic table view.
	AMPeriodicTableView *tableView_;
};

#endif // XRFPERIODICTABLEVIEW_H
