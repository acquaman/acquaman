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


#ifndef AMABSORPTIONEDGE_H
#define AMABSORPTIONEDGE_H

#include <QString>

/// Basic encapsulation of an absorption edge.
class AMAbsorptionEdge
{
public:
	/// Builds an absorption edge from the name and energy provided.  The name is expected to be in the form "Fe L1".  Failure to use this convention will result in undefined behaviour.
 	virtual ~AMAbsorptionEdge();
	AMAbsorptionEdge(const QString &name, double energy);
	/// Builds an absorption edge from the name and energy (energy is provided as a string) provided.  The name is expected to be in the form "Fe L1".  Failure to use this convention will result in undefined behaviour.
	AMAbsorptionEdge(const QString &name, const QString &energy);
	/// Copy constructor.
	AMAbsorptionEdge(const AMAbsorptionEdge &original);
	/// Null constructor.  Creates a null AMAbsorptionEdge.
	AMAbsorptionEdge();

	/// Returns the name of the edge.
	QString name() const { return name_; }
	/// Returns the energy of the edge as a double.
	double energy() const { return energy_; }
	/// Returns the energy of the edge as a string.
	QString energyString(char format = 'f', int precision = 2) const { return QString::number(energy_, format, precision); }
	/// Returns the symbol for the element associated with this absorption edge.
	QString elementSymbol() const { return symbol_; }
	/// Returns the edge name for this absorption edge.
	QString edgeName() const { return edge_; }
	/// Returns whether the AMAbsorptionEdge is null or not.
	bool isNull() const { return name_.isNull(); }

	/// Returns a string representation of the absorption edge.
	virtual QString toString() const;

	/// The equals operator.  Sets all parameters to the same as the provided AMAbsorptionEdge.
	AMAbsorptionEdge &operator =(const AMAbsorptionEdge &other);
	/// The equivalent operator.  Checks all of the parameters of the provided AMAbsorptionEdge.
	bool operator ==(const AMAbsorptionEdge &other) const;
	/// The not-equivalent operator.  Logical not of the equivalent operator.
	bool operator !=(const AMAbsorptionEdge &other) const;
	/// The less-than operator.  Returns true if energy of this absorption edge is less than other's.
	bool operator <(const AMAbsorptionEdge &other) const;
	/// The greater-than operator.  Returns true if the energy of this absorption edge is greater than other's.
	bool operator >(const AMAbsorptionEdge &other) const;
	/// The less-than-or-equal-to operator.  Returns true if the energy of this absorption edge is less than or equal to other's.
	bool operator <=(const AMAbsorptionEdge &other) const;
	/// The greater-than-or-equal-to operator.  Returns true if the energy of this absorption edge is greater than or equal to other's.
	bool operator >=(const AMAbsorptionEdge &other) const;

protected:
	/// The name of the edge.
	QString name_;
	/// The energy of the edge.
	double energy_;
	/// The element symbol for this particular edge.
	QString symbol_;
	/// The edge name for the edge.
	QString edge_;
};

#endif // AMABSORPTIONEDGE_H
