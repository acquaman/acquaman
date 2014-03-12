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


#ifndef XRFELEMENT_H
#define XRFELEMENT_H

#include "util/AMElement.h"

#include <QStringList>
#include <QMap>

class XRFElement : public AMElement
{
	Q_OBJECT
public:
	/// Constructor.  Takes in an AMElement and builds a copy of it with the inclusion of selected lines for regions of interest.
 	virtual ~XRFElement();
	XRFElement(AMElement *el, QObject *parent = 0);

	/// Returns the list of checked off lines.  Contains the names of the lines (ie: Ka1).
	QStringList linesSelected() const { return selected_; }
	/// Boolean used to know whether there are any checked lines in the element.
	bool hasLinesSelected() const { return !selected_.isEmpty(); }
	/// Returns the number of selected lines in the element.
	int numberLinesSelected() { return selected_.size(); }

	/// Returns the energy of a given emission line.  Returns 0 if given an invalid line name.
	double lineEnergy(QString line) { return lineMap_.value(line); }

public slots:
	/// Add a selected line to the element.
	bool addLine(QString line);
	/// Removes a selected line from the element.
	bool removeLine(QString line);

protected:

	/// The list of the selected lines.
	QStringList selected_;
	/// Maps the line energy to the name.  This is meant as a convenience rather than searching through emission lines manually.
	QMap<QString, double> lineMap_;

private:
	// This turns the list that AMElements make into a string list so I can use the constructor.
	QStringList toStringList(QList<QPair<QString, QString> > list);
	QStringList interimEdgeList(QList<AMAbsorptionEdge> list);
	QStringList interimLineList(QList<AMEmissionLine> list);
};

#endif // XRFELEMENT_H
