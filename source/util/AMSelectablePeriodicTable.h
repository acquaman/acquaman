/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef AMSELECTABLEPERIODICTABLE_H
#define AMSELECTABLEPERIODICTABLE_H

#include <QObject>

#include "util/AMCustomizablePeriodicTable.h"

///	This class extends the customizable periodic table and extends it's functionality slightly by offering a "memory" of selected elements.
class AMSelectablePeriodicTable : public AMCustomizablePeriodicTable
{
	Q_OBJECT
public:
	/// Constructor.
	explicit AMSelectablePeriodicTable(QObject *parent = 0);

	/// Reimplemented to use AMSelectableElement instead of the base class AMElement.
	virtual void buildPeriodicTable();

	// Added features.
	///////////////////////////////////////

	/// Returns whether there are any selected elements in this periodic table.
	bool hasSelectedElements() const { return !savedElements_.isEmpty(); }
	/// Adds a selected element to the end of the list.
	void selectElement(AMElement *element);
	/// Removes a selected element from the list.
	void deselectElement(AMElement *element);
	/// Returns the list of selected elements.
	QList<AMElement *> selectedElements() const { return savedElements_; }

	/// Clear the list.
	void clearList();

signals:
	/// Notifier that an element has been selected.  Passes the element.
	void elementSelected(AMElement *);
	/// Notifier that an element has been deselected.  Passes the element.
	void elementDeselected(AMElement *);
	/// Notifier that all of the elments have been removed.
	void listCleared();

	/// Notifier that an absorption edge was selected.  Passes the element and the edge.
	void absorptionEdgeSelected(AMElement *, const AMAbsorptionEdge &);
	/// Notifier that an absorption edge was deselected.  Passes the element and the edge.
	void absorptionEdgeDeselected(AMElement *, const AMAbsorptionEdge &);
	/// Notifier that an emission line was selected.  Passes the element and emission line.
	void emissionLineSelected(AMElement *, const AMEmissionLine &);
	/// Notifier that an emission line was deselected.  Passes the element and emission line.
	void emissionLineDeselected(AMElement *, const AMEmissionLine &);

public slots:

protected slots:
	/// Handles passing on the signal that an absorption edge has been selected.
	void onAbsorptionEdgeSelected(const AMAbsorptionEdge &edge);
	/// Handles passing on the signal that an absorption edge has been deselected.
	void onAbsorptionEdgeDeselected(const AMAbsorptionEdge &edge);
	/// Handles passing on the signal that an emission line has been selected.
	void onEmissionLineSelected(const AMEmissionLine &line);
	/// Handles passing on the signal that an emission line has been deselected.
	void onEmissionLineDeselected(const AMEmissionLine &line);

protected:
	/// List of elements that have been saved.
	QList<AMElement *> savedElements_;
};

#endif // AMSELECTABLEPERIODICTABLE_H
