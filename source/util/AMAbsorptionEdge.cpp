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


#include "AMAbsorptionEdge.h"

#include <QStringList>

 AMAbsorptionEdge::~AMAbsorptionEdge(){}
AMAbsorptionEdge::AMAbsorptionEdge(const QString &name, double energy)
{
	name_ = name;
	energy_ = energy;

	QStringList symbolAndEdge = name.split(" ");

	// The expected form of the name is "Symbol Edge".  If not in this form, the symbol and edge are empty.
	if (symbolAndEdge.size() == 2){

		symbol_ = symbolAndEdge.first();
		edge_ = symbolAndEdge.last();
	}

	else{

		symbol_ = "";
		edge_ = "";
	}
}

AMAbsorptionEdge::AMAbsorptionEdge(const QString &name, const QString &energy)
{
	name_ = name;
	energy_ = energy.toDouble();

	QStringList symbolAndEdge = name.split(" ");

	// The expected form of the name is "Symbol Edge".  If not in this form, the symbol and edge are empty.
	if (symbolAndEdge.size() == 2){

		symbol_ = symbolAndEdge.first();
		edge_ = symbolAndEdge.last();
	}

	else{

		symbol_ = "";
		edge_ = "";
	}
}

AMAbsorptionEdge::AMAbsorptionEdge(const AMAbsorptionEdge &original)
{
	name_ = original.name();
	energy_ = original.energy();
	symbol_ = original.elementSymbol();
	edge_ = original.edgeName();
}

AMAbsorptionEdge::AMAbsorptionEdge()
{
	name_ = QString();
	energy_ = -1;
	symbol_ = QString();
	edge_ = QString();
}

QString AMAbsorptionEdge::toString() const
{
	QString result;

	if (!isNull())
		result = name_ + ": " + energyString() + " eV";

	return result;
}

AMAbsorptionEdge &AMAbsorptionEdge::operator =(const AMAbsorptionEdge &other)
{
	if (this != &other){

		name_ = other.name();
		energy_ = other.energy();
		symbol_ = other.elementSymbol();
		edge_ = other.edgeName();
	}

	return *this;
}

bool AMAbsorptionEdge::operator ==(const AMAbsorptionEdge &other) const
{
	return (name_ == other.name())
			&& (energy_ == other.energy())
			&& (symbol_ == other.elementSymbol())
			&& (edge_ == other.edgeName());
}

bool AMAbsorptionEdge::operator !=(const AMAbsorptionEdge &other) const
{
	return !(this->operator ==(other));
}

bool AMAbsorptionEdge::operator <(const AMAbsorptionEdge &other) const
{
	return energy_ < other.energy();
}

bool AMAbsorptionEdge::operator <=(const AMAbsorptionEdge &other) const
{
	return energy_ <= other.energy();
}

bool AMAbsorptionEdge::operator >(const AMAbsorptionEdge &other) const
{
	return energy_ > other.energy();
}

bool AMAbsorptionEdge::operator >=(const AMAbsorptionEdge &other) const
{
	return energy_ >= other.energy();
}
