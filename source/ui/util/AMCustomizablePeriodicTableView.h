/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#ifndef AMCUSTOMIZABLEPERIODICTABLEVIEW_H
#define AMCUSTOMIZABLEPERIODICTABLEVIEW_H

#include <QWidget>

#include <QSignalMapper>
#include <QToolButton>

#include "util/AMCustomizablePeriodicTable.h"

/// This class builds a view for the periodic table made to look like the stereotypical view known to all scientists.  It is identical to the AMPeriodicView except it is meant to be subclassed and paired with the AMCustomizablePeriodicTable.
class AMCustomizablePeriodicTableView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.  Requires a customizable periodic table model to view.
 	virtual ~AMCustomizablePeriodicTableView();
	explicit AMCustomizablePeriodicTableView(AMCustomizablePeriodicTable *table, QWidget *parent = 0);

	/// This method actually builds all of the GUI elements for the periodic table view.  It can be reimplemented by subclasses for specific looks.  The default implementation is the typical periodic table look.  It expects mapElement to be implemented correctly to ensure proper mapping of buttons to elements and a valid periodic table model.
	virtual void buildPeriodicTableView();

	/// Returns the mapped QToolButton for a given atomic number.
	QAbstractButton *button(int atomicNumber) { return qobject_cast<QAbstractButton *>(elementMapper_->mapping(atomicNumber)); }
	/// Returns the mapped QToolButton for a given element.
	QAbstractButton *button(AMElement *el) { return qobject_cast<QAbstractButton *>(elementMapper_->mapping(el->atomicNumber())); }

signals:
	/// When an element is clicked on, this signal will be emitted carrying a pointer to the element.
	void elementSelected(AMElement *);

protected slots:
	/// Slot that emits a signal carrying a pointer to the particular Element.
	void onElementClicked(int number);

protected:
	/// This is a convenience function that takes an AMElement and returns a mapped QAbstractButton where the clicked signal is mapped to that element.  Must be called after elementMapper_ has been new'ed.  Can be reimplemented for customized views with different buttons, element subclasses, or other general behaviour.
	virtual QAbstractButton *mapElement(AMElement *element);

	/// The signal mapper that maps a button to an element.
	QSignalMapper *elementMapper_;
	/// The periodic table model that this view visualizes.
	AMCustomizablePeriodicTable *periodicTable_;
};

#endif // AMCUSTOMIZABLEPERIODICTABLEVIEW_H
