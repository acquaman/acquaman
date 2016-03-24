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


#ifndef PERIODICTABLEVIEW_H
#define PERIODICTABLEVIEW_H

#include <QtGui/QWidget>

#include <QSignalMapper>
#include <QToolButton>

#include "AMQEvents.h"

#include "util/AMElement.h"

/*!
	This class builds a view for the periodic table made to look like the stereotypical view used by every scientist.  When an element is clicked, it emits a signal containing the pointer to that element.
  */
class AMPeriodicTableView : public QWidget
{
	Q_OBJECT

public:
	/// Builds the periodic table view using the information contained in periodic table.  It sets up the signals such that clicking on a particular element will emit a signal with the element contained within.
	AMPeriodicTableView(QWidget *parent = 0);
	virtual ~AMPeriodicTableView();

	/// Returns the mapped QToolButton for a given atomic number.
	QToolButton *button(int atomicNumber) { return qobject_cast<QToolButton *>(elementMapper_->mapping(atomicNumber)); }
	/// Returns the mapped QToolButton for a given element.
	QToolButton *button(AMElement *el) { return qobject_cast<QToolButton *>(elementMapper_->mapping(el->atomicNumber())); }

signals:
	/// Mapped signal that passes the atomic number of an element.
	void clicked(int);
	/// When an element is clicked on, this signal will be emitted carrying a pointer to the element.
	void elementSelected(AMElement *);

protected slots:
	/// Slot that emits a signal carrying a pointer to the particular Element.
	void onElementClicked(int number);

protected:
	/// This is a convenience function that takes an Element and returns a mapped QToolButton where the clicked signal is mapped to that element.  Must be called after elementMapper_ has been new'ed.
	QToolButton *mapElement(AMElement *element)
	{
		QToolButton *button = new QToolButton;
		button->setFont(QFont("Times New Roman", 12));
		button->setText(element->symbol());
		button->setFixedSize(30, 25);

		if (element->emissionLines().isEmpty() && element->absorptionEdges().isEmpty())
			button->setEnabled(false);

		connect(button, SIGNAL(clicked()), elementMapper_, SLOT(map()));
		elementMapper_->setMapping(button, element->atomicNumber());

		return button;
	}

	/// The signal mapper that maps a button to an element.
	QSignalMapper *elementMapper_;
};

#endif // PERIODICTABLEVIEW_H
