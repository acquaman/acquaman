/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef XRFPERIODICTABLEVIEW_H
#define XRFPERIODICTABLEVIEW_H

#include <QWidget>

#include "ui/util/AMPeriodicTableView.h"
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
	/// Helper slot that takes in an element that has recently had a region of interest added to or removed from it and changes the colour accordingly.
	void onRegionOfInterestChanged(XRFElement *el, QString line);

protected:
	/// A pointer to the XRF periodic table.
	XRFPeriodicTable *xrfTable_;
	/// A generic periodic table view.
	AMPeriodicTableView *tableView_;
};

#endif // XRFPERIODICTABLEVIEW_H
